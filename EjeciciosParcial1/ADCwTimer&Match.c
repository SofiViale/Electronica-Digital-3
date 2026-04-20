/*
Realizar un programa que interrumpa por timer0 cada 250ms y que en los primeros 50ms se prenda un led y que comience a convertir el adc por el canal 0 en modo burst.
La frecuencia del core es de 60Mhz. El led estara conectado al pin 1.16.
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#define TIMER0 1

void config_ports();
void config_adc();
void config_timer();
void config_systick();

int main(void){

    config_ports();
    config_adc();  
    config_timer();
    config_systick();

    while(1);

    return 0;
}

void config_ports(){

    LPC_PINCON -> PINSEL3 &= ~ (3 << 0);// PONGO EN GPIO EL PIN 1.16

    LPC_GPIO1 -> FIODIR |= (1 << 16);//PONGO EL PIN 1.16 COMO SALIDA
    LPC_GPIO1 -> FIOSET |= (1 << 16);// EL LED COMIENZA PRENDIDO

    //CONFIGURO EL PIN PARA ENTRADA AL ADC
    LPC_PINCON ->PINSEL1 &= ~ (3 << 14);
    LPC_PINCON ->PINSEL1 |=  (1 << 14);
    
}

void config_timer(){

    LPC_TIM0 -> PR = 59999;
    LPC_TIM0 -> MR0 = 249;
    LPC_TIM0 -> MCR |= (3 << 0);
    LPC_TIM0 -> TCR |= (1 << 0);

    //HABILITO LA INTERRUPCION EN EL NVIC
    NVIC -> ISER[0] |= (1 << TIMER0);

}

void config_adc(){

    LPC_SC -> PCONP |= (1 << 12);
    LPC_SC -> PCLKSEL0 &= ~(3 << 24);

    LPC_ADC -> ADCR |= (1 << 0);
    LPC_ADC -> ADCR &= ~ (0xFF << 8);
    LPC_ADC -> ADCR |=  (0x4A << 8);
    LPC_ADC -> ADCR |= (1 << 16);

}

void config_systick(){
    
    SysTick -> LOAD = 2999999;
    SysTick -> VAL = 0;
    SysTick -> CTRL |= (1 << 0) | (1 << 1) | (1 << 2);

    //NO DEBO HABILITAR LA INTERRUP DEL SYSTICK EN EL NVIC
}

void SysTick_Handler(){

    LPC_GPIO1 -> FIOCLR |= (1 << 16);// APAGO EL LED LUEGO DE LOS 50 MS
    LPC_ADC -> ADCR &= ~(1 << 16); //DESACTIVO EL MODO BURST
    LPC_ADC -> ADCR &= ~(7 << 24);//NO CONVIERTE EL ADC
    SysTick -> CTRL &= ~ (1 << 1);//DESHABILITO INTERRUPCION DEL SYSTICK

}

void TIMER0_IRQHandler(){

    LPC_TIM0->IR = 1;

    SysTick -> CTRL |= (1 << 1);//HABILITO INTERRUPCION DEL SYSTICK

    //COMIENZO A CONVERTIR 
    
    LPC_ADC -> ADCR &= ~(7 << 24);
    LPC_ADC -> ADCR |= (1 << 24);//START NOW
    LPC_GPIO1 -> FIOSET |= (1 << 16);// PRENDO EL LED
    SysTick->VAL = 0;                  // reinicio cuenta

}