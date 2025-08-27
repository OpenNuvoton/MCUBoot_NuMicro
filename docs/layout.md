# Flash layout and partition sizes configuration

Flash partition configuration is managed through the flash_layout.h file specific to your target platform. 
For example, M460's layout is defined in [here](../bl2/platform/m460/partition/flash_layout.h).
This file defines:

- Bootloader partition (`FLASH_AREA_BL2`)
- Primary and secondary slot (`FLASH_AREA_0` and `FLASH_AREA_2`)
- Scratch area configuration (`FLASH_AREA_SCRATCH`)

Modify the parameters to adjust partition sizes and memory layout according to your requirements.
