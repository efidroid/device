#ifndef _IRQ_GPIO_TLMM_H
#define _IRQ_GPIO_TLMM_H

int lkl_gpio_tlmm_get_free_irq(const char *user, int gpio);
void lkl_gpio_tlmm_put_irq(int i, const char *user);

#endif
