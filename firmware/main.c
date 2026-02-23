#include "main.h"

// Defines for the systick
#define millis() (systick_millis)
#define micros() (SysTick->CNT / DELAY_US_TIME)

volatile uint32_t systick_millis;

// Counter
volatile uint16_t counter = 0;

// PWM PERIOD
#define PWM_PERIOD 100
#define PWM_DUTY   20  

// counter for pwm
volatile uint16_t pwm_counter = 0;

// Timer Interrupt
void TIM1_UP_IRQHandler(void) __attribute__((interrupt));
void TIM1_UP_IRQHandler(void)
{
    if (TIM1->INTFR & TIM_FLAG_Update)
    {
        TIM1->INTFR &= ~TIM_FLAG_Update;

        pwm_counter++;

        if (pwm_counter >= PWM_PERIOD)
            pwm_counter = 0;

        if (pwm_counter < PWM_DUTY)
            funDigitalWrite(LED_3, 0);
        else
            funDigitalWrite(LED_3, 1);
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
    TIM1->ATRLR = 100 - 1;

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
 * Function to init all
 */
void InitAddon()
{
	SystemInit(); // Init system
    funGpioInitAll(); // Init all gpio

	timer_init(); // Start timer

    // Initialize leds
    INIT_LED(LED_3);

}

/**
 * Main Function
 */
int main()
{

	InitAddon(); // Init All functions

	while(1)
	{
	}
}