#ifndef _GPIO_LKL_H
#define _GPIO_LKL_H

struct lkl_gpio_callback {
	int		(*get_direction)(void *user);
	int		(*direction_input)(void *user);
	int		(*direction_output)(void *user, int value);
	int		(*get)(void *user);
	void		(*set)(void *user, int value);
};

int lkl_gpio_register_gpio(struct lkl_gpio_callback *cb, int irq, void *user);
void lkl_gpio_remove_gpio(int offset);

#endif
