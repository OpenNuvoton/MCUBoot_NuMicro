#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__

/* Size of a Secure and of a Non-secure image */
#define FLASH_S_PARTITION_SIZE          (0x40000 - 0x20000)
#define FLASH_NS_PARTITION_SIZE         (0)
#define FLASH_MAX_PARTITION_SIZE        (0x40000)

/* Sector size of the flash hardware; same as FLASH0_SECTOR_SIZE */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x400)
/* Same as FLASH0_SIZE */
#define FLASH_TOTAL_SIZE                (0x00080000)

/* Flash layout info for BL2 bootloader */
/* Same as FLASH0_BASE_S */
#define FLASH_BASE_ADDRESS              (0x00000000)

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_PRIMARY and
 * IMAGE_SECONDARY, SCRATCH is used as a temporary storage during image
 * swapping.
 */
#define FLASH_AREA_BL2_OFFSET      (0x0)
#define FLASH_AREA_BL2_SIZE        (0x15000)

// #if (MCUBOOT_IMAGE_NUMBER == 1)

/* primary slot */
#define FLASH_AREA_0_ID            (1)
#define FLASH_AREA_0_OFFSET        (0x20000)
#define FLASH_AREA_0_SIZE          (0x8000)

/* Secondary slot is in embedded flash */
#define FLASH_AREA_2_ID            (FLASH_AREA_0_ID + 1)
#define FLASH_AREA_2_OFFSET        (0x60000)
#define FLASH_AREA_2_SIZE          (0x8000)

/* Scratch area */
#define FLASH_AREA_SCRATCH_ID      (FLASH_AREA_2_ID + 1)
#define FLASH_AREA_SCRATCH_OFFSET  (0x0F100000 + FLASH_AREA_IMAGE_SECTOR_SIZE)
#define FLASH_AREA_SCRATCH_SIZE    (FLASH_AREA_IMAGE_SECTOR_SIZE)

/* The maximum number of status entries supported by the bootloader. */
#define MCUBOOT_STATUS_MAX_ENTRIES ((FLASH_S_PARTITION_SIZE + \
                                     FLASH_NS_PARTITION_SIZE) / \
                                    FLASH_AREA_SCRATCH_SIZE)

/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS    ((FLASH_S_PARTITION_SIZE + \
                                     FLASH_NS_PARTITION_SIZE) / \
                                    FLASH_AREA_IMAGE_SECTOR_SIZE)

// #endif

/* Flash device name used by BL2
 * Name is defined in flash driver file: Driver_Flash.c
 */

#define FLASH_DEV_NAME      Driver_FLASH0

#define FLASH_DEV_NAME_0    FLASH_DEV_NAME
#define FLASH_DEVICE_ID_0   (888) // # FLASH_DEVICE_ID // PH

#define FLASH_DEV_NAME_2    FLASH_DEV_NAME
#define FLASH_DEVICE_ID_2   (888) // # FLASH_DEVICE_ID // PH

#endif