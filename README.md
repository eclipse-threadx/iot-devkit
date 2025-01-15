<!--
  Copyright (c) 2024 Eclipse Foundation
 
  This program and the accompanying materials are made available 
  under the terms of the MIT license which is available at
  https://opensource.org/license/mit.
 
  SPDX-License-Identifier: MIT
 
  Contributors: 
      Frédéric Desbiens - Initial version.

-->

# Eclipse ThreadX IoT DevKit Starter Application

This starter application is an adaptation of a sample developed originally by Microsoft. The original code can be found here:

[https://github.com/eclipse-threadx/getting-started](https://github.com/eclipse-threadx/getting-started)

We removed code providing support for Azure IoT Cloud. The only board supported is the MXChip AZ3166 for the time being.

## Cloning this repository
Eclipse ThreadX and Eclipse ThreadX NetX Duo are included as submodules.

When cloning, you must specify the `--recurse-submodules` option to get the code for the submodules. If you forget this option, just run the following commands in the root folder of your clone. 

```
git submodule init
git submodule update --recursive
```

## Prerequisites

### Computer
Theoretically, any recent laptop running Windows 11, Linux, or MacOS should do.

We tested on the following environments:
- Windows 11 24H2 (Version 10.0.26100.2161)
- Ubuntu 22.04.5 LTS (Windows Subsystem for Linux version 2.3.24.0)


### IoT DevKit
This sample is preconfigured to work with the [MXChip AZ3166 board](https://docs.mxchip.com/en/nr6ggk/blyezpv6gkqicywi.html).

This board can run Eclipse ThreadX but is also Arduino compatible.

The board features a [STM32F412RG MCU](https://www.st.com/en/microcontrollers-microprocessors/stm32f412rg.html) from STMicroelectronics. The MCU is clocked at 100Mhz and comes with 1 Mbyte of flash memory and 256Kbytes of SRAM.

The AZ3166 has the following hardware onboard:
- 128*64 dot matrix OLED display: VGM128064
- RGB LED lights controlled by P9813:
- Temperature and Humidity Sensor: HTS221
- Atmospheric pressure sensor: LPS22HB
- Bidirectional mono audio ADC/DAC: NAU88C10, microphone and 3.5mm headphone jack
- Six-axis accelerometer: LSM6DSL
- Geomagnetic sensor: LIS2MDL
- DC motor
- Two buttons
- three LED indicators

The board also features WiFi connectivity. However, only 2.4Ghz networks are supported.

This starter application is preconfigured to provide access to some, but not all, of the peripherals above.

### Developer tools
In terms of tooling, all you need to work on the challenge is CMake, Ninja, and a suitable C compiler. Naturally, having Git installed could help as well. ;-)

The source code for ThreadX and related modules is very portable and compliant with all "required" and "mandatory" rules of MISRA-C:2004 and MISRA C:2012. Most modern C compilers should be able to compile it. The official build pipelines rely on Arm's embedded GNU toolchain.

Below are instructions to install the tools.

**Ubuntu**
```
apt install ninja-build cmake 
```

Then, download and install Arm's embedded GNU toolchain, available at [https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

The following will download and unpack version 13.3.rel1 of the software to `/opt`.
``` 
wget https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz
sudo tar xJf arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz -C /opt
```

To test, you can run the following commands:
```
export PATH=$PATH:/opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin
arm-none-eabi-gcc --version
```

**Windows**
```
winget install --id=Arm.GnuArmEmbeddedToolchain  -e
winget install --id=Ninja-build.Ninja  -e
winget install --id=Kitware.CMake  -e
```

## Compiling and running the application
To compile the application, simply execute the relevant script found in the `MXChip/AZ3166/scripts` folder.

To deploy your code on the AZ3166, just plug the board on your computer. When you do so, this will create a virtual drive and a serial port over USB. On my Windows laptop, the board appears as the `d:` drive and the `COM4` serial port. 

Once compilation is finished, you will find the executable in the `MXChip/AZ3166/build/app` folder. The default filename is `mxchip_threadx.bin`. Just copy that file to the virtual drive and the AZ3166's boot loader will reset the board and execute your code. There is also a deploy script in the `MXChip/AZ3166/scripts` folder.

You can use any terminal application to connect to the serial port and monitor your application's output. Personally, I use Tera Term, which you can install using `winget`. Just make sure you set the baud rate to **115,200**.

If you deployed this application without any changes, you will get the following output in your terminal:

> ```
> Scanning I2C bus
> ..........................0x1a...0x1e.0x20...........................0x3c...............................0x5c..0x5f..........0x6a.....................
>
> Starting Eclipse ThreadX thread
> 
>
> Initializing WiFi
> ERROR: wifi_ssid is empty
> ERROR: wifi_init (0x00000043)
> ERROR: Failed to initialize the network (0x00000043)
> ```

### WiFi configuration
To connect the board to a WiFi network, edit the following constants found in `cloud_config.h`:

- `HOSTNAME`
- `WIFI_SSID`
- `WIFI_PASSWORD`

Make sure to select an appropriate value for `WIFI_MODE` as well.

If the WiFi is properly congiured, you will get the output below at application startup:

> ```
> Initializing WiFi
>       MAC address: C8:93:46:88:6F:9E
> SUCCESS: WiFi initialized
> ```

## Where to go from here
The starter application provided in this repository simply initiates the board and WiFi connectivity.

We will eventually deliver updates showing how to access the sensors and publish data over MQTT and HTTP. Stay tuned!


### Networking support
NetX Duo is a comprehensive TCP/IPv4 and v6 network stack. It offers built-in HTTP and MQTT clients, among other things.

If you need a local MQTT broker for testing, we recommend using Eclipse Mosquitto. Here is how to install it.

You will find the Eclipse ThreadX documentation here: [https://github.com/eclipse-threadx/rtos-docs](https://github.com/eclipse-threadx/rtos-docs)

**Ubuntu**
```
apt install mosquitto mosquitto mosquitto-clients
```

The mosquitto-clients package installs the `mosquitto_pub`and `mosquitto_sub` utilities.

**Windows**
```
winget install --id=EclipseFoundation.Mosquitto
```

The Windows installer provides the `mosquitto_pub`and `mosquitto_sub` utilities. The default installation directory is `Program Files\mosquittoC:\Program Files\mosquitto`.