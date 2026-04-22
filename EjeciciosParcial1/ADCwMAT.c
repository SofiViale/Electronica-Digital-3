

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#define ADC 22

void config_ports();
void config_timer();
void config_adc();


int main(void){
    
    config_ports();
    config_timer();
    config_adc();


    while(1);
    return 0;
}


void config_timer(){

    LPC_TIM0 -> PR = 59999;
    LPC_TIM0 -> MR0 = 999;
    LPC_TIM0 -> MCR |= (1 << 1);
    LPC_TIM0 -> EMR |= (3 << 6);    
    LPC_TIM0 -> TCR |= (1 << 0);

}

void config_adc(){

    LPC_SC -> PCONP |= (1 <<12);
    LPC_SC -> PCLKSEL0 &= ~(3 <<24);//15m

    LPC_ADC -> ADCR |= (1 << 0);
    LPC_ADC -> ADCR &= ~(0xFF << 8);
    LPC_ADC -> ADCR |= ~(0x1 << 8);
    LPC_ADC -> ADCR &= ~(1 << 16);
    LPC_ADC -> ADCR &= ~(7 << 24);
    LPC_ADC -> ADCR |= (7 << 26);
    LPC_ADC -> ADCR &= ~(1 << 27); //SUBIDA
    
    //HABILITO INTERRUPCION EN ADC Y NVIC
    LPC_ADC -> ADINTEN |= (1 << 0);
    NVIC -> ISER[0] |= (1 << ADC);
    
}

