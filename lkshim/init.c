/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 * Copyright (c) 2011-2014, Xiaomi Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <debug.h>
#include <reg.h>
#include <board.h>
#include <gsbi.h>
#include <platform/gpio.h>
#include <platform/iomap.h>

#define LINUX_MACHTYPE_8960_CDP     3396
#define LINUX_MACHTYPE_8064_MTP     3949
#define LINUX_MACHTYPE_8064_MITWO   4180
#define MSM_BOOT_UART_SWITCH_GPIO_MITWOA    33
#define MSM_BOOT_UART_SWITCH_GPIO_MITWO     62

#define GPIO_BASE_REG(x) (TLMM_BASE_ADDR + 0x3000 + (x))

/* output value */
#define GPIO_OUT_0      GPIO_BASE_REG(0x00) /* gpio  31-0   */
#define GPIO_OUT_1      GPIO_BASE_REG(0x04) /* gpio  63-31  */
#define GPIO_OUT_2      GPIO_BASE_REG(0x08) /* gpio  89-64  */

/* same pin map as above, output enable */
#define GPIO_OE_0       GPIO_BASE_REG(0x80)
#define GPIO_OE_1       GPIO_BASE_REG(0x84)
#define GPIO_OE_2       GPIO_BASE_REG(0x88)

/* same pin map as above, input read */
#define GPIO_IN_0       GPIO_BASE_REG(0x60)
#define GPIO_IN_1       GPIO_BASE_REG(0x64)
#define GPIO_IN_2       GPIO_BASE_REG(0x68)

/* GPIO_IN_OUT: OUTPUT LEVEL */
#define GPIO_IN_OUT_HIGH    0x2
#define GPIO_IN_OUT_LOW     0x00

typedef struct gpioregs gpioregs;

struct gpioregs {
    unsigned out;
    unsigned in;
    unsigned oe;
};

static gpioregs GPIO_REGS[] = {
    {
        .out =  GPIO_OUT_0,
        .in =   GPIO_IN_0,
        .oe =   GPIO_OE_0,
    },
    {
        .out =  GPIO_OUT_1,
        .in =   GPIO_IN_1,
        .oe =   GPIO_OE_1,
    },
    {
        .out =  GPIO_OUT_2,
        .in =   GPIO_IN_2,
        .oe =   GPIO_OE_2,
    },
};

static gpioregs *find_gpio(unsigned n, unsigned *bit)
{
    if (n > 89) {
        dprintf(CRITICAL, "Wrong GPIO %d\n", n);
        return 0;
    }
    if (n > 63) {
        *bit = 1 << (n - 64);
        return GPIO_REGS + 2;
    }
    if (n > 31) {
        *bit = 1 << (n - 32);
        return GPIO_REGS + 1;
    }
    *bit = 1 << n;
    return GPIO_REGS + 0;
}

int gpio_direction(unsigned n, unsigned flags)
{
    gpioregs *r;
    unsigned b;
    unsigned v;

    if ((r = find_gpio(n, &b)) == 0)
        return -1;

    v = readl(r->oe);
    if (flags & GPIO_OUTPUT)
        writel(v | b, r->oe);
    else
        writel(v & (~b), r->oe);
    return 0;
}

void gpio_output_value(unsigned n, unsigned on)
{
    gpioregs *r;
    unsigned b;
    unsigned v;

    if ((r = find_gpio(n, &b)) == 0)
        return;

    v = readl(r->out);
    if (on)
        writel(v | b, r->out);
    else
        writel(v & (~b), r->out);
}

int gpio_input_value(unsigned n)
{
    gpioregs *r;
    unsigned b;

    if ((r = find_gpio(n, &b)) == 0)
        return 0;

    return (readl(r->in) & b) ? 1 : 0;
}

/* Configure gpio for uart - based on gsbi id */
void gpio_config_uart_dm(uint8_t id)
{

    if (board_target_id() == LINUX_MACHTYPE_8960_CDP) {
        gpio_tlmm_config(MSM_BOOT_UART_SWITCH_GPIO_MITWOA, 0, GPIO_OUTPUT,
                         GPIO_NO_PULL, GPIO_16MA, GPIO_ENABLE);
        gpio_direction(MSM_BOOT_UART_SWITCH_GPIO_MITWOA, GPIO_OUTPUT);
        gpio_set(MSM_BOOT_UART_SWITCH_GPIO_MITWOA, GPIO_IN_OUT_HIGH);

    } else if (board_target_id() == LINUX_MACHTYPE_8064_MTP
               || board_target_id() == LINUX_MACHTYPE_8064_MITWO) {
        gpio_tlmm_config(MSM_BOOT_UART_SWITCH_GPIO_MITWO, 0, GPIO_OUTPUT,
                         GPIO_NO_PULL, GPIO_16MA, GPIO_ENABLE);
        gpio_direction(MSM_BOOT_UART_SWITCH_GPIO_MITWO, GPIO_OUTPUT);
        gpio_set(MSM_BOOT_UART_SWITCH_GPIO_MITWO, GPIO_IN_OUT_HIGH);
    }

    if (board_platform_id() == MPQ8064 || board_platform_id() == MSM8260AB) {
        switch (id) {

            case GSBI_ID_5:
                /* configure rx gpio */
                gpio_tlmm_config(23, 1, GPIO_INPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                /* configure tx gpio */
                gpio_tlmm_config(22, 1, GPIO_OUTPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                break;

            default:
                ASSERT(0);
        }
    } else if ((board_platform_id() == APQ8064) ||
               (board_platform_id() == APQ8064AA) ||
               (board_platform_id() == APQ8064AB)) {
        switch (id) {

            case GSBI_ID_1:
                /* configure rx gpio */
                gpio_tlmm_config(19, 1, GPIO_INPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                /* configure tx gpio */
                gpio_tlmm_config(18, 1, GPIO_OUTPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                break;


            case GSBI_ID_7:
                /* configure rx gpio */
                gpio_tlmm_config(83, 1, GPIO_INPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                /* configure tx gpio */
                gpio_tlmm_config(82, 2, GPIO_OUTPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                break;

            default:
                ASSERT(0);
        }
    } else {
        switch (id) {

            case GSBI_ID_3:
                /* configure rx gpio */
                gpio_tlmm_config(15, 1, GPIO_INPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                /* configure tx gpio */
                gpio_tlmm_config(14, 1, GPIO_OUTPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                break;

            case GSBI_ID_5:
                /* configure rx gpio */
                gpio_tlmm_config(23, 1, GPIO_INPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                /* configure tx gpio */
                gpio_tlmm_config(22, 1, GPIO_OUTPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                break;

            case GSBI_ID_8:
                /* configure rx gpio */
                gpio_tlmm_config(35, 1, GPIO_INPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                /* configure tx gpio */
                gpio_tlmm_config(34, 1, GPIO_OUTPUT, GPIO_NO_PULL,
                                 GPIO_8MA, GPIO_DISABLE);
                break;

            default:
                ASSERT(0);
        }
    }
}
