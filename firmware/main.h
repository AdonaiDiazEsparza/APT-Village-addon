#include "ch32fun.h"
#include <stdbool.h>
#include <stdio.h>

#define RIGHT_EYE   PA1
#define LEFT_EYE    PD0
#define NOSE        PA2

#define LED_1       PD4
#define LED_2       PD5
#define LED_3       PD2
#define LED_4       PD3
#define LED_5       PC0
#define LED_6       PD6
#define LED_7       PC1
#define LED_8       PC2

// Init leds
#define INIT_LED(led) funPinMode(led, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);

// Struct to control the pwm led
typedef struct {
    uint8_t led;
    uint8_t pwm;
} led_t;

