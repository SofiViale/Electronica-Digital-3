/*
Tengo 3 señales analógicas que quiero analizar, para eso necesito guardarlas en 3 vectores que se llaman señal_1, señal_2 y señal_3. 
Las frecuencias de las señales son 5kHz, 8kHz y 10kHz respectivamente. Utilizando el adc tomar muestras y guardarlas en los vectores.
La frecuencia del core clock es de 60Mhz.
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#define ADC 22

volatile uint32_t señal_1[1024];
volatile uint32_t señal_2[1024];
volatile uint32_t señal_3[1024];
volatile uint32_t cont = 0;

void config_ports();
void config_adc();

int main(void){

    config_ports();
    config_adc();


    while(1);

    return 0;
}

void config_ports(){

    //HABILITO ADC0
    LPC_PINCON -> PINSEL1 &= ~(3 << 14);
    LPC_PINCON -> PINSEL1 |= (1 << 14);

    //HABILITO ADC1
    LPC_PINCON -> PINSEL1 &= ~(3 << 16);
    LPC_PINCON -> PINSEL1 |= (1 << 16);
    
    //HABILITO ADC2
    LPC_PINCON -> PINSEL1 &= ~(3 << 18);
    LPC_PINCON -> PINSEL1 |= (1 << 18);
}


void config_adc(void){

    LPC_SC -> PCONP |= (1 << 12);
    LPC_SC -> PCLKSEL0 &= ~(3 << 24);

    LPC_ADC -> ADCR |= (7 << 0);
    LPC_ADC -> ADCR &= ~(0xFF << 8);
    LPC_ADC -> ADCR |= (0xF9 << 8);
    LPC_ADC -> ADCR |= (1 << 16);
    LPC_ADC -> ADINTEN |= (7 << 0); //HABILITO INTERRUPCIONES EN CANAL 0, 1 Y 2
    LPC_ADC -> ADINTEN &= ~ (1 << 8); // HABILITO PARA QUE CADA CANAL INTERRUMPA 

    NVIC -> ISER[0] |= (1 << ADC);//HABILITO INTERRUPCION EN EL NVIC

}

void ADC_IRQHandler(){

    if ((LPC_ADC -> AD0STAT >> 0) & 1)
    {
        señal_1[cont] = (LPC_ADC ->AD0DR0 >> 4) & 0xFFF;
    }
    if ((LPC_ADC -> AD0STAT >> 1) & 1)
    {
        señal_2[cont] = (LPC_ADC ->AD0DR1 >> 4) & 0xFFF;
    }
    if ((LPC_ADC -> AD0STAT >> 2) & 1)
    {
        señal_3[cont] = (LPC_ADC ->AD0DR2 >> 4) & 0xFFF;

        cont++;
        if (cont > 1023)
        {
            cont = 0;
        }
    }
}