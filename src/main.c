#include "stm8s.h"
#include "milis.h"

#include <stdio.h>
#include "stm8s_adc2.h"
#include "uart1.h"
#include "spse_stm8.h"

#define _ISOC99_SOURCE
#define _GNU_SOURCE

#define LED_PORT GPIOC
#define LED_PIN  GPIO_PIN_5
#define LED_HIGH   GPIO_WriteHigh(LED_PORT, LED_PIN)
#define LED_LOW  GPIO_WriteLow(LED_PORT, LED_PIN)
#define LED_REVERSE GPIO_WriteReverse(LED_PORT, LED_PIN)

#define BTN_PORT GPIOE
#define BTN_PIN  GPIO_PIN_4
#define BTN_PUSH (GPIO_ReadInputPin(BTN_PORT, BTN_PIN)==RESET) 


void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(BTN_PORT, BTN_PIN, GPIO_MODE_IN_FL_NO_IT);

    init_milis();
    init_uart1();


    // Adc setup
    // na pinech/vstupech ADC_IN2 (PB4) a ADC_IN3 (PB5) vypneme vstupní buffer
    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL4, DISABLE);
    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL5, DISABLE);

    // při inicializaci volíme frekvenci AD převodníku mezi 1-4MHz při 3.3V
    // mezi 1-6MHz při 5V napájení
    // nastavíme clock pro ADC (16MHz / 4 = 4MHz)
    ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);

    // volíme zarovnání výsledku (typicky vpravo, jen vyjmečně je výhodné vlevo)
    ADC2_AlignConfig(ADC2_ALIGN_RIGHT);
    
    // nasatvíme multiplexer na některý ze vstupních kanálů
    ADC2_Select_Channel(ADC2_CHANNEL_4);
    // rozběhneme AD převodník
    ADC2_Cmd(ENABLE);
    // počkáme než se AD převodník rozběhne (~7us)
    ADC2_Startup_Wait();
}

int main(void)
{
    uint32_t time = 0;
    uint16_t nap;
    uint16_t ADCx;
    uint16_t tep;
    setup();

    while (1) {

        if (milis() - time > 333 && BTN_PUSH) {
            LED_REVERSE; 
            time = milis();
            ADCx = ADC_get(ADC2_CHANNEL_4);
            nap = (uint32_t)3300 * ADCx /1024;
            tep = ((uint32_t)33000* ADCx - 4096000) / 19968;
            printf("ADCx = %d; U = %dmV T=%d.%d \n\r", ADCx, nap, tep/10, tep%10);
        }

        /*LED_REVERSE; */
        /*_delay_ms(333);*/
        /*printf("Funguje to!!!\n");*/
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
