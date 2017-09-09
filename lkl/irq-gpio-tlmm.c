#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/device/irq_gpio_tlmm.h>

void lkl_gpio_tlmm_impl_irq_setup(int gpio);
void lkl_gpio_tlmm_impl_irq_ack(int gpio);
void lkl_gpio_tlmm_impl_irq_mask(int gpio);
void lkl_gpio_tlmm_impl_irq_unmask(int gpio);
int lkl_gpio_tlmm_impl_irq_set_type(int gpio, unsigned int type);

extern int lkl_gpio_tlmm_impl_irq_map[];
extern int lkl_gpio_tlmm_impl_irq_map_size;

static void lkl_gpio_tlmm_irq_ack(struct irq_data *data) {
    int gpio;

    if (data->irq>lkl_gpio_tlmm_impl_irq_map_size) {
        __WARN();
        return;
    }

    gpio = lkl_gpio_tlmm_impl_irq_map[data->irq];
    if (!gpio) {
        __WARN();
        return;
    }

    lkl_gpio_tlmm_impl_irq_ack(gpio);
}

static void lkl_gpio_tlmm_irq_mask(struct irq_data *data) {
    int gpio;

    if (data->irq>lkl_gpio_tlmm_impl_irq_map_size) {
        __WARN();
        return;
    }

    gpio = lkl_gpio_tlmm_impl_irq_map[data->irq];
    if (!gpio) {
        __WARN();
        return;
    }

    lkl_gpio_tlmm_impl_irq_mask(gpio);
}

static void lkl_gpio_tlmm_irq_unmask(struct irq_data *data) {
    int gpio;

    if (data->irq>lkl_gpio_tlmm_impl_irq_map_size) {
        __WARN();
        return;
    }

    gpio = lkl_gpio_tlmm_impl_irq_map[data->irq];
    if (!gpio) {
        __WARN();
        return;
    }

    lkl_gpio_tlmm_impl_irq_unmask(gpio);
}

static int lkl_gpio_tlmm_irq_set_type(struct irq_data *data, unsigned int type) {
    int gpio;

    if (data->irq>lkl_gpio_tlmm_impl_irq_map_size) {
        __WARN();
        return -1;
    }

    gpio = lkl_gpio_tlmm_impl_irq_map[data->irq];
    if (!gpio) {
        __WARN();
        return -1;
    }

    return lkl_gpio_tlmm_impl_irq_set_type(gpio, type);
}

static int lkl_gpio_tlmm_irq_set_wake(struct irq_data *data, unsigned int on)
{
    int gpio;

    if (data->irq>lkl_gpio_tlmm_impl_irq_map_size) {
        __WARN();
        return -1;
    }

    gpio = lkl_gpio_tlmm_impl_irq_map[data->irq];
    if (!gpio) {
        __WARN();
        return -1;
    }

    return 0;
}

static struct irq_chip lkl_gpio_tlmm_irq_chip = {
	.name		= "lkl",
	.irq_ack	= lkl_gpio_tlmm_irq_ack,
	.irq_mask	= lkl_gpio_tlmm_irq_mask,
	.irq_unmask	= lkl_gpio_tlmm_irq_unmask,
	.irq_set_type   = lkl_gpio_tlmm_irq_set_type,
	.irq_set_wake   = lkl_gpio_tlmm_irq_set_wake,
};

int lkl_gpio_tlmm_get_free_irq(const char *user, int gpio) {
    int irq;

    if (gpio>lkl_gpio_tlmm_impl_irq_map_size) {
        __WARN();
        return -EINVAL;
    }

    if(lkl_gpio_tlmm_impl_irq_map[gpio] != 0) {
        __WARN();
        return -EBUSY;
    }

    // setup gpio irq
    lkl_gpio_tlmm_impl_irq_setup(gpio);

    // allocate new soft irq
    irq = lkl_get_free_irq(user);

    // use level irq handler by default
    irq_set_chip_and_handler(irq, &lkl_gpio_tlmm_irq_chip, handle_level_irq);

    // store gpio number
    lkl_gpio_tlmm_impl_irq_map[irq] = gpio;

    return irq;
}

void lkl_gpio_tlmm_put_irq(int i, const char *user)
{
    if (i>lkl_gpio_tlmm_impl_irq_map_size) {
        __WARN();
        return;
    }

    if (!lkl_gpio_tlmm_impl_irq_map[i]) {
        __WARN();
        return;
    }

    lkl_put_irq(i, user);
    lkl_gpio_tlmm_impl_irq_map[i] = 0;
}
