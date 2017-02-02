#include <debug.h>
#include <string.h>
#include <dev/keys.h>
#include <dev/ssbi.h>
#include <dev/gpio_keypad.h>
#include <dev/pm8921.h>
#include <platform/gpio.h>

#include <lib/atagparse.h>
#include <lib/cmdline.h>

#define BITS_IN_ELEMENT(x) (sizeof(x) * 8)
#define KEYMAP_INDEX(row, col) (row)* BITS_IN_ELEMENT(unsigned int) + (col)

typedef enum {
    HW_REV_EVB1 = 0,
    HW_REV_EVB2,
    HW_REV_A,
    HW_REV_B,
    HW_REV_C,
    HW_REV_D,
    HW_REV_E,
    HW_REV_F,
    HW_REV_G,
    HW_REV_H,
    HW_REV_1_0,
    HW_REV_1_1,
    HW_REV_1_2,
    HW_REV_MAX
} hw_rev_type;

unsigned int apq8064_qwerty_keymap[] = {
    [KEYMAP_INDEX(0, 0)] = KEY_VOLUMEDOWN,    /* Volume key on the device/CDP */
    [KEYMAP_INDEX(0, 1)] = KEY_VOLUMEUP,  /* Volume key on the device/CDP */
};

unsigned int apq8064_pm8921_keys_gpiomap[] = {
    [KEYMAP_INDEX(0, 0)] = PM_GPIO(1),  /* Volume key on the device/CDP */
    [KEYMAP_INDEX(0, 1)] = PM_GPIO(2),  /* Volume key on the device/CDP */
};

struct qwerty_keypad_info apq8064_pm8921_qwerty_keypad = {
    .keymap = apq8064_qwerty_keymap,
    .gpiomap = apq8064_pm8921_keys_gpiomap,
    .mapsize = ARRAY_SIZE(apq8064_qwerty_keymap),
    .key_gpio_get = &pm8921_gpio_get,
    .settle_time = 5 /* msec */,
    .poll_time = 20 /* msec */,
};

/* for board revision */
static hw_rev_type lge_bd_rev = HW_REV_EVB1;

static void board_revno_setup(void)
{
    /* CAUTION: These strings are come from LK. */
    const char *rev_str[] = {"evb1", "evb2", "rev_a", "rev_b", "rev_c", "rev_d",
                             "rev_e", "rev_f", "rev_g", "rev_h", "rev_10", "rev_11", "rev_12",
                             "reserved"
                            };
    int i;
    const char *rev_info = cmdline_get(lkargs_get_command_line_list(), "lge.rev");

    dprintf(INFO, "BOARD : LGE input %s \n", rev_info);
    for (i=0; i< HW_REV_MAX; i++) {
        if ( !strncmp(rev_info, rev_str[i], 6)) {
            lge_bd_rev = (hw_rev_type) i;
            break;
        }
    }

    dprintf(INFO, "BOARD : LGE matched %s \n", rev_str[lge_bd_rev]);
}

hw_rev_type lge_get_board_revno(void)
{
    return lge_bd_rev;
}

void apq8064_keypad_init(void)
{
    board_revno_setup();
    if (lge_get_board_revno() < HW_REV_C) {
        apq8064_qwerty_keymap[KEYMAP_INDEX(0, 0)] = KEY_VOLUMEUP;
        apq8064_qwerty_keymap[KEYMAP_INDEX(0, 1)] = KEY_VOLUMEDOWN;
    }

    apq8064_keypad_gpio_init();
    ssbi_gpio_keypad_init(&apq8064_pm8921_qwerty_keypad);
}
