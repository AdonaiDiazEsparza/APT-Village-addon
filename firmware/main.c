#include "main.h"

/**
 * Define the structs for the leds
 */
led_t led_1 = {
    .led = LED_1,
    .pwm = 0
};

led_t led_2 = {
    .led = LED_2,
    .pwm = 0
};

led_t led_3 = {
    .led = LED_3,
    .pwm = 0
};

led_t led_4 = {
    .led = LED_4,
    .pwm = 0
};

led_t led_5 = {
    .led = LED_5,
    .pwm = 0
};

led_t led_6 = {
    .led = LED_6,
    .pwm = 0
}; 

led_t led_7 = {
    .led = LED_7,
    .pwm = 0
}; 

led_t led_8 = {
    .led = LED_8,
    .pwm = 0
}; 


led_t led_nose = {
    .led = NOSE,
    .pwm = 0
}; 


led_t led_right = {
    .led = RIGHT_EYE,
    .pwm = 0
}; 

led_t led_left = {
    .led = LEFT_EYE,
    .pwm = 0
}; 

// ====================================== HANDLE FOR INTERRUPTS ============

// Defines for the systick
#define millis() (systick_millis)
#define micros() (SysTick->CNT / DELAY_US_TIME)

volatile uint32_t systick_millis;

// Counter
volatile uint16_t counter = 0;

// Timer Interrupt
void TIM1_UP_IRQHandler(void) __attribute__((interrupt));
void TIM1_UP_IRQHandler(void)
{
    if (TIM1->INTFR & TIM_FLAG_Update)
    {
        TIM1->INTFR &= ~TIM_FLAG_Update;

        counter++;

        if (counter >= 1000)
            counter = 0;

        funDigitalWrite(led_1.led, counter<led_1.pwm);
        funDigitalWrite(led_2.led, counter<led_1.pwm);
        funDigitalWrite(led_3.led, counter<led_3.pwm);
        funDigitalWrite(led_4.led, counter<led_4.pwm);
        funDigitalWrite(led_5.led, counter<led_5.pwm);
        funDigitalWrite(led_6.led, counter<led_6.pwm);
        funDigitalWrite(led_7.led, counter<led_7.pwm);
        funDigitalWrite(led_8.led, counter<led_8.pwm);
        funDigitalWrite(led_left.led, counter<led_nose.pwm);
        funDigitalWrite(led_right.led, counter<led_nose.pwm);
        funDigitalWrite(led_nose.led, counter<led_nose.pwm);
    }
}

// Systick interrupt handler
void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
	SysTick->CMP += DELAY_MS_TIME;
	SysTick->SR = 0x00000000;
	systick_millis++;
}

/**
 * Delay functions
 */

void delay_ms(uint32_t time)
{
    uint32_t last_time = millis();

    while(millis() - last_time < time);
}

/**
 * Init the Timer
 */
void timer_init(void)
{
    RCC->APB2PCENR |= RCC_APB2Periph_TIM1;

    TIM1->CTLR1 = 0;
    TIM1->CTLR2 = 0;
    TIM1->RPTCR = 0;

    // 48MHz / 48 = 1MHz (1us)
    TIM1->PSC = 47;

    // 1MHz / 1000 = 1kHz (1ms)
    TIM1->ATRLR = 10 - 1;

    TIM1->SWEVGR = TIM_PSCReloadMode_Update;

    TIM1->INTFR &= ~TIM_FLAG_Update;
    TIM1->DMAINTENR |= TIM_IT_Update;

    NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM1->CTLR1 |= TIM_CEN;
}

/**
 * Init the Systick
 */
void systick_init(void)
{
	SysTick->CTLR = 0x0000;
	SysTick->CMP = DELAY_MS_TIME - 1;
	SysTick->CNT = 0x00000000;
	systick_millis = 0x00000000;
	
	SysTick->CTLR |= SYSTICK_CTLR_STE   |  // Enable Counter
	                 SYSTICK_CTLR_STIE  |  // Enable Interrupts
	                 SYSTICK_CTLR_STCLK ;  // Set Clock Source to HCLK/1
	
	NVIC_EnableIRQ(SysTick_IRQn);
}

/**
 * Init all the leds
 */
void InitLeds(void){
    INIT_LED(LED_1);
    INIT_LED(LED_2);
    INIT_LED(LED_3);
    INIT_LED(LED_4);
    INIT_LED(LED_5);
    INIT_LED(LED_6);
    INIT_LED(LED_7);
    INIT_LED(LED_8);

    INIT_LED(RIGHT_EYE);
    INIT_LED(LEFT_EYE);
    INIT_LED(NOSE);
}

/**
 * Function to init all
 */
void InitAddon()
{
	SystemInit(); // Init system

    funGpioInitAll(); // Init all gpio

	timer_init(); // Start timer

    systick_init(); // Init the systick system

    InitLeds(); // Iniciar todos los leds
}

/**
 * All leds turn off
 */
void all_off(){
    led_1.pwm = 0;
    led_2.pwm = 0;
    led_3.pwm = 0;
    led_4.pwm = 0;
    led_5.pwm = 0;
    led_6.pwm = 0;
    led_7.pwm = 0;
    led_8.pwm = 0;
    led_nose.pwm = 0;
    led_left.pwm = 0;
    led_right.pwm = 0;
}

/**
 * Set all
 */
void set_all(uint16_t pwm){

    if(pwm > 1000) return;

    led_1.pwm = pwm;
    led_2.pwm = pwm;
    led_3.pwm = pwm;
    led_4.pwm = pwm;
    led_5.pwm = pwm;
    led_6.pwm = pwm;
    led_7.pwm = pwm;
    led_8.pwm = pwm;
    led_nose.pwm = pwm;
    led_left.pwm = pwm;
    led_right.pwm = pwm;
}

/**
 * First Sequence
 */

void eyes_sequence()
{
    for(uint8_t i = 0; i<7 ; i++){

        uint32_t last_time = millis();

        while(led_nose.pwm < 1000)
        {
            if(millis() - last_time > 5){
                led_nose.pwm+=10;

                led_nose.pwm = led_nose.pwm > 1000 ? 1000 : led_nose.pwm; 
                last_time = millis();
            }
        }

        last_time = millis();

        while(led_nose.pwm > 0){
            
            if(millis() - last_time > 5){
                led_nose.pwm-=10;
                last_time = millis();
            }
        }
    }
    
    all_off();
}

/**
 *  Squence
 */
void foot()
{
    led_t* leds_right[] = {&led_4, &led_6, &led_5};
    led_t* leds_left[] = {&led_3, &led_7, &led_8};

    uint8_t pivot = 0;
    int8_t direction = 1, pivot_nose_eyes = 0;

    uint32_t last_time_eyes = millis() ,last_time_feet = millis(), last_time_nose = millis(), last_time_light_feet = millis(); 

    leds_left[0]->pwm = 1000;
    leds_right[0]->pwm = 1000;

    uint16_t value_foot = 0;

    all_off();

    // ======================== At this point the leds around need to start growing thier values ======================

    last_time_light_feet = millis();

    while(value_foot < 200){
        uint32_t current_time = millis();

        if(current_time - last_time_light_feet > 100){
            value_foot += 5;
            last_time_light_feet = current_time;
        }

        if(current_time - last_time_feet > 150 ){

            for(uint8_t i = 0; i<3; i++){
                leds_left[i]->pwm = 0;
                leds_right[i]->pwm = 0;
            }

            pivot+=direction;

            leds_left[pivot]->pwm = value_foot;
            leds_right[pivot]->pwm = value_foot;

            if(pivot == 0) direction = 1;
            if(pivot == 2) direction = -1;

            last_time_feet = current_time;
        }
    }

    // ======================== Start growing the values of the eyes on the spider ======================

    last_time_eyes = millis();

    while(led_1.pwm < 500)
    {
        uint32_t current_time = millis();

        if(current_time - last_time_eyes > 50 ){
            led_1.pwm += 10;
            last_time_eyes = current_time;
        }

        if(current_time - last_time_feet > 150 ){

            for(uint8_t i = 0; i<3; i++){
                leds_left[i]->pwm = 0;
                leds_right[i]->pwm = 0;
            }

            pivot+=direction;

            leds_left[pivot]->pwm = value_foot;
            leds_right[pivot]->pwm = value_foot;

            if(pivot == 0) direction = 1;
            if(pivot == 2) direction = -1;

            last_time_feet = current_time;
        }
    }

    // ======================== Start iluminating the eyes and nose for the skull ======================
    
    last_time_nose = millis();

    while(led_nose.pwm < 1000)
    {
        uint32_t current_time = millis();

        if(current_time - last_time_nose > 25 ){
            led_nose.pwm += 10;
            last_time_nose = current_time;
        }

        if(current_time - last_time_feet > 150 ){

            for(uint8_t i = 0; i<3; i++){
                leds_left[i]->pwm = 0;
                leds_right[i]->pwm = 0;
            }

            pivot+=direction;

            leds_left[pivot]->pwm = value_foot;
            leds_right[pivot]->pwm = value_foot;

            if(pivot == 0) direction = 1;
            if(pivot == 2) direction = -1;

            last_time_feet = current_time;
        }
    }

    // ======================== A time fade in out ======================

    last_time_eyes = millis();

    while(true){
        if(led_nose.pwm == 1000) pivot_nose_eyes = -20;
        if(led_nose.pwm == 20) pivot_nose_eyes = 20;

        uint32_t current_time = millis();

        if(current_time - last_time_nose > 10){
            led_nose.pwm += pivot_nose_eyes;
            last_time_nose = current_time;
        }

        if(current_time - last_time_feet > 150 ){

            for(uint8_t i = 0; i<3; i++){
                leds_left[i]->pwm = 0;
                leds_right[i]->pwm = 0;
            }

            pivot+=direction;

            leds_left[pivot]->pwm = value_foot;
            leds_right[pivot]->pwm = value_foot;

            if(pivot == 0) direction = 1;
            if(pivot == 2) direction = -1;

            last_time_feet = current_time;
        }

        if(current_time - last_time_eyes > 20000){
            break;
        }
    }

    // ======================== Start going down the eyes of the spider ======================

    last_time_light_feet = millis();

    while(value_foot != 0){
        uint32_t current_time = millis();

        if(led_nose.pwm == 1000) pivot_nose_eyes = -20;
        if(led_nose.pwm == 20) pivot_nose_eyes = 20;

        if(current_time - last_time_light_feet > 100){
            value_foot -= 5;
            last_time_light_feet = current_time;
        }

        if(current_time - last_time_feet > 150 ){

            for(uint8_t i = 0; i<3; i++){
                leds_left[i]->pwm = 0;
                leds_right[i]->pwm = 0;
            }

            pivot+=direction;

            leds_left[pivot]->pwm = value_foot;
            leds_right[pivot]->pwm = value_foot;

            if(pivot == 0) direction = 1;
            if(pivot == 2) direction = -1;

            last_time_feet = current_time;
        }
    
        if(current_time - last_time_nose > 10){
            led_nose.pwm += pivot_nose_eyes;
            last_time_nose = current_time;
        }
    }

    leds_left[pivot]->pwm = value_foot;
    leds_right[pivot]->pwm = value_foot;

    // ======================== start turning off the lights ======================

    last_time_eyes = millis();

    while (led_1.pwm != 0)
    {
        uint32_t current_time = millis();

        if(led_nose.pwm == 1000) pivot_nose_eyes = -20;
        if(led_nose.pwm == 20) pivot_nose_eyes = 20;
        
        if(current_time - last_time_eyes > 50){
            led_1.pwm -= 25;
            last_time_eyes = current_time;
        }

        if(current_time - last_time_nose > 10){
            led_nose.pwm += pivot_nose_eyes;
            last_time_nose = current_time;
        }
    }
    
    // ======================== play a little with the eyes and the nose ======================

    last_time_eyes = millis();

    while(true){
        if(led_nose.pwm == 1000) pivot_nose_eyes = -20;
        if(led_nose.pwm == 20) pivot_nose_eyes = 20;

        uint32_t current_time = millis();

        if(current_time - last_time_nose > 10){
            led_nose.pwm += pivot_nose_eyes;
            last_time_nose = current_time;
        }

        if(current_time - last_time_eyes > 5000){
            break;
        }
    }

    while(led_nose.pwm < 1000){
        if(led_nose.pwm == 20) pivot_nose_eyes = 20;

        uint32_t current_time = millis();

        if(current_time - last_time_nose > 10){
            led_nose.pwm += pivot_nose_eyes;
            last_time_nose = current_time;
        }
    }

    // ========================== fade down =================================================

    while(led_nose.pwm != 0){
        uint32_t current_time = millis();

        if(current_time - last_time_nose > 100){
            led_nose.pwm -= 20;
            last_time_nose = current_time;
        }
    }
    
    delay_ms(1000);
}

/**
 * Main Function
 */
int main()
{

	InitAddon(); // Init All functions

    // all_off();

    delay_ms(100);

	while(1)
	{
        // eyes_sequence();
        foot();
	}
}