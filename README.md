# MCUBoot for NuMicro Series

MCUboot is a secure bootloader for 32-bits microcontrollers.

## Features

* Secure firmware verification using cryptographic signatures
* Support for multiple image upgrade strategies

## Get started

### Requirements

* NuMicro microcontroller ([Supported Platform](#supported-platform))
* Toolchain: `CMake`, `Ninja` and `Arm Compiler 6`
* Flashing Tool: `PyOCD`

### Cloning the Repository

Make sure to include submodules when cloning:
```
git clone https://github.com/OpenNuvoton/MCUBoot_NuMicro.git --recurse-submodules
```

### Build

The following commands demonstrate how to build for the M480 series.
Replace `m480` and `cortex-m4` to match your target.
```
mkdir build
cd build
cmake -G Ninja \
  -DCMAKE_SYSTEM_PROCESSOR=cortex-m4 \
  -DPLATFORM_NAME=m480 \
  --toolchain ../toolchain/armclang.cmake \
  -S ..
cmake --build .
```

### Installation

```
pyocd load -t m487jidae build/bin/bl2.bin
```

### Customization

The bootloader behavior can be customized using CMake variables, passed directly via the command line during CMake configuration.

Enable more logging message:
```
-DMCUBOOT_LOG_LEVEL=DEBUG
```

Change upgrade strategies:
```
-DMCUBOOT_UPGRADE_STRATEGY=DIRECT_XIP
```

Check available configuration options in [mcuboot_default_config.cmake](https://github.com/OpenNuvoton/MCUBoot_NuMicro/blob/master/bl2/ext/mcuboot/mcuboot_default_config.cmake).

### Supported Platform

| Device | Core  |
| --- | --- |
| M2354 | Cortex-M23 |
| M480 | Cortex-M4 |
| M55M1 | Cortex-M55 |
| M261 | Cortex-M23 |
| M460 | Cortex-M4 |
