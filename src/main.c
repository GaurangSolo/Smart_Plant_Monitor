#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <stdio.h> // Used for sensor_value_to_double
#include <stdbool.h> // Used for boolean type

/* --- BME280 Setup --- */
// Get the device pointer from the devicetree node defined in the overlay
#define BME280_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(bosch_bme280)
static const struct device *bme280_dev = DEVICE_DT_GET(BME280_NODE);

/* --- LED Setup (Using Blue LED - led2) --- */
// Use the alias "led2" defined for the board (Blue LED on XIAO - P0.06)
#define LED_NODE DT_ALIAS(led2)
#if DT_NODE_HAS_STATUS(LED_NODE, okay)
// If the node exists and is okay, create the gpio_dt_spec structure
static const struct gpio_dt_spec led_spec = GPIO_DT_SPEC_GET(LED_NODE, gpios);
#else
// If led2 alias isn't found or okay, generate a compile-time error
#error "Unsupported board: led2 devicetree alias is not defined or enabled"
#endif

/* --- Main Application Entry Point --- */
void main(void)
{
    // Variables for sensor readings and function return codes
    struct sensor_value temp, press, humidity;
    int ret;
    bool bme280_ready = false; // Flag to track sensor status

    printk("Zephyr BME280 + Blinky (Blue LED) on XIAO nRF52840\n");

    /* --- Check if LED is ready --- */
    // Check LED GPIO controller readiness
    if (!gpio_is_ready_dt(&led_spec)) {
        printk("Error: LED GPIO controller %s is not ready\n", led_spec.port->name);
        return; // Exit if LED GPIO not ready - Blinking is essential
    }

    /* --- Configure LED Pin --- */
    // Configure the LED pin as an output, start with it OFF (respecting ACTIVE_LOW)
    ret = gpio_pin_configure_dt(&led_spec, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        printk("Error %d: failed to configure LED pin\n", ret);
        return; // Exit if configuration fails
    }
    printk("Blue LED pin configured.\n");

    /* --- Check if BME280 is ready (but don't exit if not) --- */
    if (!device_is_ready(bme280_dev)) {
        printk("Warning: BME280 sensor device (%s) is not ready. Will skip sensor readings.\n", bme280_dev->name);
        bme280_ready = false; // Sensor is not ready
    } else {
        printk("BME280 sensor ready.\n");
        bme280_ready = true; // Sensor is ready
    }

    /* --- Infinite Loop for Reading and Blinking --- */
    while (1) {
        /* --- Toggle LED --- */
        // Flip the state of the LED (ON -> OFF or OFF -> ON)
        ret = gpio_pin_toggle_dt(&led_spec);
        if (ret < 0) {
            printk("Error %d: failed to toggle LED pin\n", ret);
            // Decide if you want to continue or stop on toggle failure
        }

        /* --- Read BME280 (only if it was ready during startup) --- */
        if (bme280_ready) {
            // Fetch a new sample from the sensor
            ret = sensor_sample_fetch(bme280_dev);
            if (ret != 0) {
                printk("BME280: Sample fetch failed (%d)\n", ret);
                // Optional: Set bme280_ready = false here if fetch fails repeatedly?
            } else {
                // Get specific channel values
                sensor_channel_get(bme280_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
                sensor_channel_get(bme280_dev, SENSOR_CHAN_HUMIDITY, &humidity);
                sensor_channel_get(bme280_dev, SENSOR_CHAN_PRESS, &press);

                // Print the fetched values using printk (requires CONFIG_CBPRINTF_FP_SUPPORT=y)
                printk("Temp: %.2f C; Press: %.2f kPa; Humidity: %.2f %%\n",
                       sensor_value_to_double(&temp),
                       sensor_value_to_double(&press),
                       sensor_value_to_double(&humidity));
            }
        }
        // If bme280_ready is false, the code simply skips the fetch/read block

        /* Wait - this delay controls both the blink rate AND sensor reading frequency */
        /* The Blue LED will be ON for 2 seconds, then OFF for 2 seconds */
        k_sleep(K_MSEC(2000));
    }
}
