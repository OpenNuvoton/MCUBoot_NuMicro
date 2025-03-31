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

Enable more verbose logging:
```
-DMCUBOOT_LOG_LEVEL=DEBUG
```

Change upgrade strategies:
```
-DMCUBOOT_UPGRADE_STRATEGY=DIRECT_XIP
```

Check available configuration options in [mcuboot_default_config.cmake](https://github.com/OpenNuvoton/MCUBoot_NuMicro/blob/master/bl2/ext/mcuboot/mcuboot_default_config.cmake).

## Supported Platform

| Device | Core  |
| --- | --- |
| M2354 | Cortex-M23 |
| M480 | Cortex-M4 |
| M55M1 | Cortex-M55 |
| M261 | Cortex-M23 |
| M460 | Cortex-M4 |
| M2L31 | Cortex-M23 |
| M2351 | Cortex-M23 |

## Using CMake Tools (a VS Code extension)

CMake Tools provides a convenient interface for working with CMake-based project in VS Code.
If you're unsure how to get started,
the JSON files in [.vscode](https://github.com/OpenNuvoton/MCUBoot_NuMicro/tree/master/.vscode) folder offer a starting point.
For more details, see the [CMake Tools documentation](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/README.md)
```
cd .vscode
cp cmake-kits.json.sample cmake-kits.json
cp settings.json.sample settings.json
```
