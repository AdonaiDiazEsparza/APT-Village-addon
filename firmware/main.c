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
 * Wait until flash operation finishes (or timeout)
 */
static int flash_wait(uint32_t timeout)
{
	while ((FLASH->STATR & FLASH_FLAG_BSY) && timeout)
		timeout--;

	if (!timeout)
		return FLASH_TIMEOUT;

	if (FLASH->STATR & FLASH_FLAG_WRPRTERR)
		return FLASH_ERROR_WRP;

	return FLASH_COMPLETE;
}

/**
 * Unlock flash + option bytes
 */
static void flash_unlock(void)
{
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
	FLASH->OBKEYR = FLASH_KEY1;
	FLASH->OBKEYR = FLASH_KEY2;
	FLASH->MODEKEYR = FLASH_KEY1;
	FLASH->MODEKEYR = FLASH_KEY2;
}

/**
 * Deactivate NRST on PD7 so it can be used as GPIO (LED_1).
 * Writes option bytes once; skips if already disabled.
 */
void deactivate_nrst(void)
{
	// RST_MODE == 11b (OB_RST_NoEN) already set?
	if ((FLASH->OBR & FLASH_OBR_RST_MODE) == FLASH_OBR_RST_MODE)
		return;

	flash_unlock();

	int status = flash_wait(EraseTimeout);
	if (status != FLASH_COMPLETE)
		return;

	// Erase option bytes (must rewrite RDPR afterwards) 
	FLASH->OBKEYR = FLASH_KEY1;
	FLASH->OBKEYR = FLASH_KEY2;
	FLASH->CTLR |= CR_OPTER_Set;
	FLASH->CTLR |= CR_STRT_Set;
	status = flash_wait(EraseTimeout);

	if (status != FLASH_COMPLETE)
	{
		if (status != FLASH_TIMEOUT)
			FLASH->CTLR &= CR_OPTER_Reset;
		return;
	}

	FLASH->CTLR &= CR_OPTER_Reset;
	FLASH->CTLR |= CR_OPTPG_Set;
	OB->RDPR = RDP_Key;
	status = flash_wait(ProgramTimeout);

	if (status == FLASH_TIMEOUT)
		return;

	FLASH->CTLR &= CR_OPTPG_Reset;

	// USER: SW IWDG, no STOP/STDBY reset, NRST disabled, boot from user code 
	const uint16_t user =
		OB_IWDG_SW |
		OB_STOP_NoRST |
		OB_STDBY_NoRST |
		OB_RST_NoEN |
		OB_STARTMODE_USER |
		0xC0; /* reserved bits must stay 1 */

	FLASH->OBKEYR = FLASH_KEY1;
	FLASH->OBKEYR = FLASH_KEY2;
	status = flash_wait(ProgramTimeout);

	if (status != FLASH_COMPLETE)
		return;

	FLASH->CTLR |= CR_OPTPG_Set;
	OB->USER = user;
	status = flash_wait(ProgramTimeout);

	if (status != FLASH_TIMEOUT)
		FLASH->CTLR &= CR_OPTPG_Reset;

	// Option bytes apply after reset 
	NVIC_SystemReset();
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
}

/**
 * Function to init all
 */
void InitAddon()
{
	SystemInit(); // Init system

	deactivate_nrst(); // PD7 as GPIO (not NRST)

    funGpioInitAll(); // Init all gpio

	timer_init(); // Start timer

    systick_init(); // Init the systick system

    InitLeds(); // Iniciar todos los leds
}

/**
 * All leds turn off
 */
void all_off(){
    SET_OFF(led_1);
    SET_OFF(led_2);
    SET_OFF(led_3);
    SET_OFF(led_4);
    SET_OFF(led_5);
    SET_OFF(led_6);
    SET_OFF(led_7);
    SET_OFF(led_8);
}

/** 
 * Set all leds pwm value 
 */
void set_all_pwm(uint16_t pwm)
{
    SET_PWM(led_1, pwm);
    SET_PWM(led_2, pwm);
    SET_PWM(led_3, pwm);
    SET_PWM(led_4, pwm);
    SET_PWM(led_5, pwm);
    SET_PWM(led_6, pwm);
    SET_PWM(led_7, pwm);
    SET_PWM(led_8, pwm);

    delay_ms(10);
}

/**
 * First leds sequence
 */
void first_sequence(void)
{
    // First will be the trail on the leds around
    led_t *leds_trail[] = {&led_3, &led_5, &led_7, &led_4, &led_6, &led_8, &led_1, &led_2};
    
    for (int i = 0; i < LEDS_COUNT-2; i++)
    {
        SET_ON(*leds_trail[i]);
        delay_ms(500);
    }

    // Fade in the eyes
    for(int i = 0; i < 1000; i+=10)
    {
        SET_PWM(led_1, i);
        SET_PWM(led_2, i);
        delay_ms(10);
    }

    // A delay
    delay_ms(2000);

    // Turning off the rest of the leds
    for(int i = LEDS_COUNT-1; i > 0; i--)
    {
        SET_OFF(*leds_trail[i-1]);
        delay_ms(500);
    }

    // Fade out the eyes
    for(int i = 1010; i > 0; i-=10)
    {
        SET_PWM(led_1, i-10);
        SET_PWM(led_2, i-10);
        delay_ms(10);
    }

    delay_ms(1000);
    
    all_off();
}

/**
 * Second leds sequence
 */
void second_sequence(void)
{
    led_t *leds_trail[] = {&led_3, &led_5, &led_7, &led_4, &led_6, &led_8, &led_1, &led_2};

    uint16_t value_feet = 0;
    uint8_t pivot_feet = 0;
    int8_t direction_eyes = 1;

    uint32_t last_time = millis();
    uint32_t last_time_light_feet = millis();

    // Here start to fade in all the leds around the feet
    while(value_feet <200)
    {
        uint32_t current_time = millis();

        if(current_time - last_time > 100){
            value_feet += 5;
            last_time = current_time;
        }

        if(current_time - last_time_light_feet > 100){
            for(int i = 0; i < 6; i++)
            {
                SET_PWM(*leds_trail[pivot_feet], 0);
            }
            
            pivot_feet += direction_eyes;

            SET_PWM(*leds_trail[pivot_feet], value_feet);
            SET_PWM(*leds_trail[pivot_feet+3], value_feet);

            last_time_light_feet = current_time;

            if(pivot_feet == 0) direction_eyes = 1;
            if(pivot_feet == 2) direction_eyes = -1;
        }
            
    }
 
    uint16_t value_eyes = 0;
    uint32_t last_time_light_eyes = millis();

    // Here start to fade in the eyes
    while(value_eyes <= 500)
    {
        uint32_t current_time = millis();

        // For the feet =====================================
        if(current_time - last_time_light_feet > 150)
        {
            for(int i = 0; i < 6; i++)
            {
                SET_PWM(*leds_trail[pivot_feet], 0);
            }
            
            pivot_feet += direction_eyes;

            SET_PWM(*leds_trail[pivot_feet], value_feet);
            SET_PWM(*leds_trail[pivot_feet+3], value_feet);

            last_time_light_feet = current_time;

            if(pivot_feet == 0) direction_eyes = 1;
            if(pivot_feet == 2) direction_eyes = -1;
        }

        // For the eyes =====================================
        if(current_time - last_time_light_eyes > 50)
        {
            SET_PWM(led_1, value_eyes);
            SET_PWM(led_2, value_eyes);

            value_eyes += 10;   
        }
    }

    uint32_t timer_sequence = millis();
    
    // Stay for a time
    while(millis() - timer_sequence < 2000)
    {
        uint32_t current_time = millis();

        if(current_time - last_time_light_feet > 150)
        {
            for(int i = 0; i < 6; i++)
            {
                SET_PWM(*leds_trail[pivot_feet], 0);
            }
            
            pivot_feet += direction_eyes;

            SET_PWM(*leds_trail[pivot_feet], value_feet);
            SET_PWM(*leds_trail[pivot_feet+3], value_feet);

            last_time_light_feet = current_time;

            if(pivot_feet == 0) direction_eyes = 1;
            if(pivot_feet == 2) direction_eyes = -1;
        }
    }

    // Start a fade down the feet
    while(value_feet != 0)
    {
        uint32_t current_time = millis();

        if(current_time - last_time > 100){
            value_feet -= 5;
            last_time = current_time;
        }

        if(current_time - last_time_light_feet > 100){
            for(int i = 0; i < 6; i++)
            {
                SET_PWM(*leds_trail[pivot_feet], 0);
            }
            
            pivot_feet += direction_eyes;

            SET_PWM(*leds_trail[pivot_feet], value_feet);
            SET_PWM(*leds_trail[pivot_feet+3], value_feet);

            last_time_light_feet = current_time;

            if(pivot_feet == 0) direction_eyes = 1;
            if(pivot_feet == 2) direction_eyes = -1;
        }
    }

    // Start fade down the eyes
    while(value_eyes > 10)
    {

        if(millis() - last_time_light_eyes > 50)
        {
            SET_PWM(led_1, value_eyes-10);
            SET_PWM(led_2, value_eyes-10);

            value_eyes -= 10;   
        }
    }


    all_off();

    delay_ms(2000);
    
}


/** 
 * Helper sequence
 */
void lighting_sequence(void)
{
    
}

/**
 * Third leds sequence
 */
void third_sequence(void)
{
    
}

/**
 *  Squence
 */
void sequence()
{
    
}

/**
 * Main Function
 */
int main()
{

	InitAddon(); // Init All functions

    all_off(); // Turn all leds off

    delay_ms(100);

	while(1)
	{
        sequence(); // Run the sequence
	}
}