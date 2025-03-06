#ifndef __FLASH_MAP_BACKEND_H__
#define __FLASH_MAP_BACKEND_H__

#include "flash_map/flash_map.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provides abstraction of flash regions for type of use.
 *
 * System will contain a map which contains flash areas. Every
 * region will contain flash identifier, offset within flash and length.
 */

/*
 * Retrieve a memory-mapped flash device's base address.
 *
 * On success, the address will be stored in the value pointed to by
 * ret.
 *
 * Returns 0 on success, or an error code on failure.
 */
int flash_device_base(uint8_t fd_id, uintptr_t *ret);

int flash_area_id_from_image_slot(int slot);
int flash_area_id_from_multi_image_slot(int image_index, int slot);

/**
 * Converts the specified flash area ID to an image slot index.
 *
 * Returns image slot index (0 or 1), or -1 if ID doesn't correspond to an image
 * slot.
 */
int flash_area_id_to_image_slot(int area_id);

/**
 * Converts the specified flash area ID and image index (in multi-image setup)
 * to an image slot index.
 *
 * Returns image slot index (0 or 1), or -1 if ID doesn't correspond to an image
 * slot.
 */
int flash_area_id_to_multi_image_slot(int image_index, int area_id);

/*
 * Returns the value expected to be read when accessing any erased
 * flash byte.
 */
uint8_t flash_area_erased_val(const struct flash_area *fap);

/*
 * Reads len bytes from off, and checks if the read data is erased.
 *
 * Returns 1 if erased, 0 if non-erased, and -1 on failure.
 */
int flash_area_read_is_empty(const struct flash_area *fa, uint32_t off,
        void *dst, uint32_t len);


static inline int flash_area_get_sector(const struct flash_area* fap, uint32_t off,
    struct flash_sector* sector)
{
    uint32_t fs_off;
    uint32_t page_size;

    /* The offset is out of area reange */
    if(off >= fap->fa_off + fap->fa_size)
        return -1;

    fs_off = fap->fa_off + off;
    page_size = DRV_FLASH_AREA(fap)->GetInfo()->page_size;

    sector->fs_off = (fs_off / page_size) * page_size;
    sector->fs_size = page_size;

    return 0;
}

static inline uint8_t flash_area_get_id(const struct flash_area *fa)
{
    return fa->fa_id;
}

static inline uint8_t flash_area_get_device_id(const struct flash_area *fa)
{
    return fa->fa_device_id;
}

static inline uint32_t flash_area_get_off(const struct flash_area *fa)
{
    return fa->fa_off;
}

static inline uint32_t flash_area_get_size(const struct flash_area *fa)
{
    return fa->fa_size;
}

static inline uint32_t flash_sector_get_off(const struct flash_sector *fs)
{
    return fs->fs_off;
}

static inline uint32_t flash_sector_get_size(const struct flash_sector *fs)
{
    return fs->fs_size;
}


#ifdef __cplusplus
}
#endif

#endif /* __FLASH_MAP_BACKEND_H__ */