# XIAO Smart Plant Monitor Project (or Blinky Example)

This project uses the Zephyr RTOS on a Seeed Studio XIAO nRF52840 board. Development is done using VS Code with the nRF Connect Extension Pack.

## Project Structure

This is set up as a "Freestanding" Zephyr application. It relies on the Zephyr installation identified by the `ZEPHYR_BASE` environment variable but does not contain its own `.west` workspace configuration directory.

## Prerequisites

* Zephyr RTOS and SDK installed (follow official Zephyr Getting Started Guide).
* `ZEPHYR_BASE` environment variable set correctly at the **System or User level** so that external tools invoked by VS Code (like the task runner) can find the Zephyr installation. (This is needed for `west` extension commands like `flash` to be recognized).
* VS Code with the nRF Connect Extension Pack installed.

## Building the Code

1.  Open this project folder (`blinky_working` or `XIAO_SPM_v1.0`) in VS Code.
2.  Use the nRF Connect extension view (Nordic logo icon in the sidebar).
3.  Under "APPLICATIONS", find this project.
4.  Ensure a "Build Configuration" exists targeting the correct board (`xiao_ble` or `xiao_ble/nrf52840/sense`). If not, add one.
5.  Click the "Build" action (hammer icon) next to the build configuration.

## Flashing the Board (Using Custom UF2 Task)

The Seeed Studio XIAO nRF52840 uses a UF2 bootloader, which allows flashing by copying a `.uf2` file to the board when it's acting as a USB drive.

Due to this project being a freestanding application and the desire to avoid setting system-wide defaults, a custom VS Code task is defined in `.vscode/tasks.json` to handle UF2 flashing specifically for this project.

**Steps to Flash:**

1.  **Connect** the XIAO nRF52840 board to your computer via USB C.
2.  **Enter UF2 Bootloader Mode:** Quickly **double-press the `RST` (Reset) button** on the board. The onboard yellow/orange LED should start pulsing slowly, and your computer should mount a USB drive named `XIAO BLE`.
3.  **Run the Custom Task:**
    * In VS Code, open the Command Palette: `Ctrl+Shift+P` (or `Cmd+Shift+P` on Mac).
    * Type `Tasks: Run Task` and select it.
    * Choose the task labeled **`Flash XIAO with UF2`** from the list that appears.
4.  **Monitor Output:** Check the VS Code terminal output. You should see `west flash --runner uf2` being executed and confirmation of the file being copied. The board will automatically reset and run the new firmware after flashing.

**(Optional) Binding Task to Flash Button:** If you prefer to use the standard "Flash" button in the nRF Connect extension view, you can try binding the custom task to it by adding the following to your `.vscode/settings.json` file:

```json
{
    "nrf-connect.taskBindings": {
        "Flash": "Flash XIAO with UF2"
    }
}


---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
.. zephyr:code-sample:: blinky
   :name: Blinky
   :relevant-api: gpio_interface

   Blink an LED forever using the GPIO API.

Overview
********

The Blinky sample blinks an LED forever using the :ref:`GPIO API <gpio_api>`.

The source code shows how to:

#. Get a pin specification from the :ref:`devicetree <dt-guide>` as a
   :c:struct:`gpio_dt_spec`
#. Configure the GPIO pin as an output
#. Toggle the pin forever

See :zephyr:code-sample:`pwm-blinky` for a similar sample that uses the PWM API instead.

.. _blinky-sample-requirements:

Requirements
************

Your board must:

#. Have an LED connected via a GPIO pin (these are called "User LEDs" on many of
   Zephyr's :ref:`boards`).
#. Have the LED configured using the ``led0`` devicetree alias.

Building and Running
********************

Build and flash Blinky as follows, changing ``reel_board`` for your board:

.. zephyr-app-commands::
   :zephyr-app: samples/basic/blinky
   :board: reel_board
   :goals: build flash
   :compact:

After flashing, the LED starts to blink and messages with the current LED state
are printed on the console. If a runtime error occurs, the sample exits without
printing to the console.

Build errors
************

You will see a build error at the source code line defining the ``struct
gpio_dt_spec led`` variable if you try to build Blinky for an unsupported
board.

On GCC-based toolchains, the error looks like this:

.. code-block:: none

   error: '__device_dts_ord_DT_N_ALIAS_led_P_gpios_IDX_0_PH_ORD' undeclared here (not in a function)

Adding board support
********************

To add support for your board, add something like this to your devicetree:

.. code-block:: DTS

   / {
   	aliases {
   		led0 = &myled0;
   	};

   	leds {
   		compatible = "gpio-leds";
   		myled0: led_0 {
   			gpios = <&gpio0 13 GPIO_ACTIVE_LOW>;
                };
   	};
   };

The above sets your board's ``led0`` alias to use pin 13 on GPIO controller
``gpio0``. The pin flags :c:macro:`GPIO_ACTIVE_HIGH` mean the LED is on when
the pin is set to its high state, and off when the pin is in its low state.

Tips:

- See :dtcompatible:`gpio-leds` for more information on defining GPIO-based LEDs
  in devicetree.

- If you're not sure what to do, check the devicetrees for supported boards which
  use the same SoC as your target. See :ref:`get-devicetree-outputs` for details.

- See :zephyr_file:`include/zephyr/dt-bindings/gpio/gpio.h` for the flags you can use
  in devicetree.

- If the LED is built in to your board hardware, the alias should be defined in
  your :ref:`BOARD.dts file <devicetree-in-out-files>`. Otherwise, you can
  define one in a :ref:`devicetree overlay <set-devicetree-overlays>`.
