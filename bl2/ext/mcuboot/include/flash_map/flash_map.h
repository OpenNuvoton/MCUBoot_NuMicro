#ifndef H_UTIL_FLASH_MAP_
#define H_UTIL_FLASH_MAP_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "region_defs.h"
#include "Driver_Flash.h"

/*
 * For now, we only support one flash device.
 *
 * Pick a random device ID for it that's unlikely to collide with
 * anything "real".
 */
#define FLASH_DEVICE_ID                 100
#define FLASH_DEVICE_BASE               FLASH_BASE_ADDRESS

/**
 * @brief Structure describing an area on a flash device.
 *
 * Multiple flash devices may be available in the system, each of
 * which may have its own areas. For this reason, flash areas track
 * which flash device they are part of.
 */
struct flash_area {
    /**
     * This flash area's ID; unique in the system.
     */
    uint8_t fa_id;

    /**
     * ID of the flash device this area is a part of.
     */
    uint8_t fa_device_id;

    uint16_t pad16;

    /**
     * Pointer to driver
     */
    ARM_DRIVER_FLASH *fa_driver;

    /**
     * This area's offset, relative to the beginning of its flash
     * device's storage.
     */
    uint32_t fa_off;

    /**
     * This area's size, in bytes.
     */
    uint32_t fa_size;
};

/**
 * @brief Structure describing a sector within a flash area.
 *
 * Each sector has an offset relative to the start of its flash area
 * (NOT relative to the start of its flash device), and a size. A
 * flash area may contain sectors with different sizes.
 */
struct flash_sector {
    /**
     * Offset of this sector, from the start of its flash area (not device).
     */
    uint32_t fs_off;

    /**
     * Size of this sector, in bytes.
     */
    uint32_t fs_size;
};

/**
 * @brief Macro retrieving driver from struct flash area
 *
 */
#define DRV_FLASH_AREA(area) ((area)->fa_driver)

/*
 * Start using flash area.
 */
int flash_area_open(uint8_t id, const struct flash_area **area);

void flash_area_close(const struct flash_area *area);

/*
 * Read/write/erase. Offset is relative from beginning of flash area.
 */
int flash_area_read(const struct flash_area *area, uint32_t off, void *dst,
                    uint32_t len);

int flash_area_write(const struct flash_area *area, uint32_t off,
                     const void *src, uint32_t len);

int flash_area_erase(const struct flash_area *area, uint32_t off, uint32_t len);

/*
 * Alignment restriction for flash writes.
 */
uint32_t flash_area_align(const struct flash_area *area);

/*
 * Given flash area ID, return info about sectors within the area.
 */
int flash_area_get_sectors(int fa_id, uint32_t *count,
  struct flash_sector *sectors);

/*
 * Similar to flash_area_get_sectors(), but return the values in an
 * array of struct flash_area instead.
 */
__attribute__((deprecated))
int flash_area_to_sectors(int idx, int *cnt, struct flash_area *ret);

#ifdef __cplusplus
}
#endif

#endif