#include "ch32fun.h"
#include <stdbool.h>
#include <stdio.h>

#define LED_1       PD7     // Led eye left
#define LED_2       PA1     // Led eye right
#define LED_3       PD6     // Upper left
#define LED_4       PC0     // Upper right
#define LED_5       PD5     // Middle left
#define LED_6       PC7     // Middle right
#define LED_7       PD4     // Lower left
#define LED_8       PD2     // Lower right

// Init leds
#define INIT_LED(led) funPinMode(led, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);

#define LEDS_COUNT 8

// Struct to control the pwm led
typedef struct {
    uint8_t led;
    volatile uint16_t pwm;
} led_t;


// Set off
#define SET_OFF(led) (led).pwm = 0;

// Set on
#define SET_ON(led) (led).pwm = 1000;

// Set led pwm value
#define SET_PWM(led, value) (led).pwm = value;
