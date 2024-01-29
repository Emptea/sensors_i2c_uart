#include "calibration.h"
#include "flash.h"
#include "tools.h"

#define CALIBRATION_PAGE_NUM      31
#define CALIBRATION_PAGE_SIZE     FLASH_PAGE_SIZE
#define CALIBRATION_PAGE_ADDR     (flash_get_page_addr(CALIBRATION_PAGE_NUM))
#define CALIBRATION_PAGE_ADDR_END (CALIBRATION_PAGE_ADDR + CALIBRATION_PAGE_SIZE - 1)

#define CALIBRATION_MAX_CNT       (CALIBRATION_PAGE_SIZE / sizeof(union offset))

#if defined __ARMCC_VERSION
#define __place2flash __attribute__((section(".ARM.__at_0x08007C00")))
#elif defined __GNUC__
#define __place2flash __attribute__((section(".keys")))
#endif

volatile struct calibration_head {
    uint32_t cnt;
    union offset *new_offset;
} calibration_head;

static uint32_t addr_is_invalid(union offset *offset)
{
    uint32_t addr = (uint32_t)offset;
    return (addr + sizeof(*offset) - 1) > CALIBRATION_PAGE_ADDR_END;
}

static union offset *calibration_get(uint32_t idx)
{
    return (union offset *)CALIBRATION_PAGE_ADDR + idx;
}

union offset calibration_init(void)
{
    static union offset null_offset = {0};
    union offset *offset = calibration_get(0);
    while (1) {
        if (addr_is_invalid(offset)) {
            break;
        }
        if (offset->raw[0] == 0xFFFFFFFF) {
            break;
        }
        offset++;
        calibration_head.cnt++;
    }
    calibration_head.new_offset = offset;

    if (calibration_head.cnt) {
        offset--;
        return *offset;
    } else {
        return null_offset;
    }
}

uint32_t calibration_get_count(void)
{
    return calibration_head.cnt;
}

void calibration_save(union offset *offset)
{
    if (addr_is_invalid(calibration_head.new_offset)) {
        calibration_clear();
    }
    flash_memcpy_u16(offset, calibration_head.new_offset, sizeof(*offset));
    calibration_head.new_offset++;
    calibration_head.cnt++;
}

uint32_t calibration_clear(void)
{
    flash_erase_page(CALIBRATION_PAGE_ADDR);
    calibration_head.new_offset = calibration_get(0);
    calibration_head.cnt = 0;
    return CALIBRATION_MAX_CNT;
}
