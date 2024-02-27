// First Steps
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // Contains thread delay function
#include "driver/gpio.h"   // GPIO pin controls

// Running faster
#include "soc/soc.h"
#include "soc/gpio_reg.h"
#define BLINK_GPIO 0 // Change this to whatever GPIO pin you're using

void setToggle()
{
    while (1)
    {
        gpio_set_level(BLINK_GPIO, 1);
        printf("1\n");
        gpio_set_level(BLINK_GPIO, 0);
        printf("1\n");
    }
}

void fastToggle()
{
    uint32_t gpio_bit = 1 << BLINK_GPIO;
    while (1)
    {
        REG_WRITE(GPIO_OUT_W1TS_REG, gpio_bit); // W1TS: Write 1 to Set Register
        REG_WRITE(GPIO_OUT_W1TC_REG, gpio_bit); // W1TC: Write 1 to Clear Register
    }
}

void app_main()
{
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    // setToggle();
    // fastToggle();
    while (1)
    {
        printf("Hello World!\n");

    }
}
