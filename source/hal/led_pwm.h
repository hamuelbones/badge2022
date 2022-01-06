//
// Created by Samuel Jones on 12/27/21.
//

#ifndef BADGE2022_C_LED_PWM_H
#define BADGE2022_C_LED_PWM_H

#include <stdint.h>

typedef enum {
    BADGE_LED_RGB_RED = 0,
    BADGE_LED_RGB_GREEN,
    BADGE_LED_RGB_BLUE,
    BADGE_LED_STANDALONE_WHITE,
    BADGE_LED_DISPLAY_BACKLIGHT,
    BADGE_LED_MAX,
} BADGE_LED;

void led_pwm_init_gpio();

void led_pwm_enable(BADGE_LED led, uint8_t duty);

void led_pwm_disable(BADGE_LED led);

#endif //BADGE2022_C_LED_PWM_H