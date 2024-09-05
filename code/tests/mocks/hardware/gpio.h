#pragma once 

// GPIO functionality
#define GPIO_OUT 1
#define GPIO_IN 0
void gpio_init(unsigned int gpio);
void gpio_set_dir(unsigned int gpio, bool out);
void gpio_put(unsigned int gpio, bool val);
