# MCUBoot for NuMicro Series

MCUboot is a secure bootloader for 32-bits microcontrollers.

## Features

* Secure firmware verification using cryptographic signatures
* Support for multiple image upgrade strategies

## Get started

### Requirements

* NuMicro microcontroller ([Supported Platform](#supported-platform))
* Toolchain: `CMake`, `Ninja` and `Arm Compiler 6` / `GCC`
* Flashing Tool: `PyOCD`

### Cloning the Repository

Cloning all submodules recursively may impact performance and consume significant disk space. It is recommended to selectively clone only the submodules needed.
1. Cloning necessary submodules
```
git submodule update --init -- lib/ext/mcubootlib/mcuboot
git submodule update --init -- lib/ext/mbedtlslib/mbedtls
```
2. Clone the corresponding BSP based on your board model. For example, with M460:
```
git submodule update --init -- bl2/platform/m460/bsplib/bsp
```

### Configuration

It is recommended to use CMake Presets to handle CMake configuration. Using M460 as an example:

```
mkdir build
cp CMakeUserPresets.json.sample CMakeUserPresets.json
cmake --preset default .
```

#### Example

For example, to configure M55M1 with:

* ARM Compiler
* Build type: RELWITHDEBINFO
* EC-P256 image signature verification

Add a new section in CMakeUserPresets.json:
```
{
    "name": "m55-relwithdbg",
    "inherits": "common",
    "cacheVariables": {
        "CMAKE_BUILD_TYPE":"RELWITHDEBINFO",
        "CMAKE_TOOLCHAIN_FILE": "toolchain/armclang.cmake",
        "CMAKE_SYSTEM_PROCESSOR": "cortex-m55",
        "PLATFORM_NAME": "m55m1",
        "MCUBOOT_SIG_TYPE": "EC",
        "MCUBOOT_SIG_LEN": "256"
    }
}
```

And make sure you specify the correct preset during configuration:
```
cmake --preset m55-relwithdbg .
```

### Build

```
cmake --build build/
```

### Installation

```
pyocd load -t m467hjhae build/bin/bl2.bin
```

### Customization

The bootloader behavior can be customized using CMake variables.
CMake variables can be configured directly via command line arguments or specified within CMakeUserPresets.json.

Change the algorithm used for signature validation:
```
-DMCUBOOT_SIG_TYPE=EC
-DMCUBOOT_SIG_LEN=256
```

Use SPI flash as the secondary slot. The SPI flash uses the SPI interface by default. To enable the SPIM interface instead, configure `USE_SPIM=ON`.
> [!WARNING]
> The SPI flash driver is currently supported on M460 only.

```
-DSECONDARY_SLOT_IN_SPI_FLASH=ON
```

Enable more verbose logging:
```
-DMCUBOOT_LOG_LEVEL=DEBUG
```

Change upgrade strategies:
```
-DMCUBOOT_UPGRADE_STRATEGY=DIRECT_XIP
```

Check available configuration options in [mcuboot_default_config.cmake](https://github.com/OpenNuvoton/MCUBoot_NuMicro/blob/master/bl2/ext/mcuboot/mcuboot_default_config.cmake).

#### Flash Layout

The flash layout file configures image slot parameters, such as the address or the size of secondary slot.
For M460, this sets the primary slot to 64 KiB starting at 0x30000:
```C
// bl2/platform/m460/partition/flash_layout.h
#define FLASH_AREA_0_OFFSET        (0x30000)
#define FLASH_AREA_0_SIZE          (0x10000)
```

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

## MCUBoot-Compatible Firmware

To work with MCUBoot, the repository [MCUBoot-Compatible-Template](https://github.com/OpenNuvoton/MCUBoot-Compatible-Template) provides template firmware code for validating MCUBoot firmware upgrade strategies. Users can reference the template and customize the provided code according to their needs. By programming the images to the flash address of primary and secondary slot, MCUBoot will jump to the correct image based on the chosen strategy.

## Using CMake Tools (a VS Code extension)

CMake Tools provides a convenient interface for working with CMake-based project in VS Code.
For more details, see the [CMake Tools documentation](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/README.md)
