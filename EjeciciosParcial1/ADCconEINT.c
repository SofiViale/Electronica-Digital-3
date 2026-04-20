/*Realizar un codigo para que el ADC convierta el valor de entrada del canal 0 de un potenciometro, con un voltaje que varia de 0 a 1V
El ADC cuando dicho voltaje sea menor a 0,5V solo convertira cuando se presione un pulsador conectado a la EINT0 con logica negativa. Cuando
dicho voltaje sea mayor a 0,5V el adc convertira en modo burst.

FRECUENCIA DEL CORE CLOCK 60MHZ
FRECUENCIA DEL ADC 200KHZ

EL ADC TIENE 12 BITS Y UN VOLTAJE DE REFERENCIA MAXIMO DE 3,3
ENTONCES 0V = 0
        0,5 = 621
        1v = 1242
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#define ADC 22

void config_ports();
void config_adc();
void config_interrup();

int main(void){

    config_ports();
    config_adc();
    config_interrup();

    while(1);
    return 0;
}

void config_ports(){

    LPC_PINCON->PINSEL4 &= ~(3 << 20);//LO PONGO EN 00 A AMBOS BITS
    LPC_PINCON->PINSEL4 |= (1 << 20); //PONGO EN 1 EL 20 PARA EINT0
    LPC_PINCON->PINMODE4 &= ~(3 << 20);//LOGICA NEGATIVA

    LPC_PINCON->PINSEL1 &= ~(3 << 14);//PONGO EN 00 EL P0.23
    LPC_PINCON->PINSEL1 |= (1 << 14);//PONGO UN 1 PARA USAR EL PIN COMO ENTRADA AL ADC0

}

void config_adc(){

    LPC_SC->PCONP |= (1 << 12);//PRENDO EL ADC
    LPC_SC->PCLKSEL0 &= ~(3 << 24);//LO DEJO EN 00 /4
    
    LPC_ADC->ADCR |= (1 << 0);//PRENDO EL CANAL 0
    LPC_ADC->ADCR &= ~(0xFF << 8);//limpio el clockdiv
    LPC_ADC->ADCR |= (0x4A << 8);//DIVIDO EL CLOCKDIV POR 74 PARA OBTENER 200KHZ EN EL ADC
    LPC_ADC->ADCR &= ~(1 << 16);//MODO BURST DESACTIVADO
    LPC_ADC->ADCR &= ~(7 << 24); //PONGO TODOS EN 000
    LPC_ADC->ADCR |=(1 << 25); //PONGO UN 1 EN EL BIT 25, INICIA LA CONVERSION CUANDO SE PRESIONE EL PULSADOR
    LPC_ADC->AD0INTEN |= (1 << 0);//TERMINA DE CONVERTIR E INTERRUMPE

    NVIC->ISER[0] |=(1<<ADC);//HABILITO LA INTERRUPCION DEL ADC EN EL NVIC
}

void config_interrup(){
    LPC_SC->EXTPOLAR &= ~(1<<0);//BAJADA
    LPC_SC->EXTMODE |= (1<<0); //FLANCO
    LPC_SC->EXTINT |= (1 << 0);//LIMPIO LA FLAG

}

void ADC_IRQHandler(void){

    if ((LPC_ADC->ADGDR >> 31) & 1){// si el bit done es 1

        volatile uint32_t resultado = (LPC_ADC->ADGDR >> 4) & 0xFFF;

        if(resultado > 621){
            LPC_ADC->ADCR |=  (1 << 16);     // burst activado
        }
        else{
            LPC_ADC->ADCR &= ~(1 << 16);     // burst desactivado
            LPC_ADC->ADCR &= ~(7 << 24);     // limpio START
            LPC_ADC->ADCR |=  (1 << 25);     // start por EINT0
        }
    }
}