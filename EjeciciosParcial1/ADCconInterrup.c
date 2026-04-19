#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#define ADC 22
void config_adc(void);
void config_ports(void);


int main(void){

    config_ports();
    config_adc();
    while(1);
}

void config_ports(void){

    LPC_PINCON -> PINSEL3 &= ~(3 << 20); //GPIO PIN 1.26
    LPC_GPIO1 -> FIODIR |= (1 << 26); //OUTPUT
    LPC_GPIO1 -> FIOMASK = 0xFFFFFFFF; //ENMASCARO TODO EL PUERTO
    LPC_GPIO1 -> FIOMASK &= ~(1 << 26); //DESENMASCARO EL PIN 1.26 
    LPC_PINCON -> PINSEL1 |= (1<<18);//PIN 0.25 COMO ADC0.2
    LPC_PINCON -> PINSEL1 &= ~(1<<19);//PIN 0.25 COMO ADC0.2

}

void config_adc(void){
    LPC_SC -> PCONP |= (1 << 12); //PRENDO EL ADC
    LPC_SC -> PCLKSEL0 &= ~(3 << 24); //AL ADC LE LLEGA UNA FREC DE 25MHZ
    LPC_ADC ->ADCR |=  (0x04 << 0); // PONGO EN 1 EL CANAL 2
    LPC_ADC ->ADCR &= ~(0xFF << 8); //LIMPIO LOS BITS
    LPC_ADC ->ADCR |=  (0x7C << 8); //BUSCO ADC A 200KHZ
    LPC_ADC ->ADCR |=  (1 << 16); //MODO BURST ACTIVADO
    LPC_ADC ->AD0INTEN |= (1<<2); //HABILITO LA INTERRUPCION EN EL CANAL 2 DEL ADC0
    LPC_ADC ->AD0INTEN &= ~(1<<8); //INTERRUMPE CUANDO SE LEVANTA LA FLAG DEL CANAL 2 DEL ADC0

    NVIC->ISER[0] |= (1 << ADC); //HABILITO LA INTERRUPCION EN EL NVIC 

}

void ADC_IRQHandler(void){

    volatile uint32_t valor = 0;
    if((LPC_ADC -> ADGDR >> 31) & 1){
        
        valor = (LPC_ADC -> ADGDR >> 4) & 0xFFF;
        
        if(valor > 2047){

            LPC_GPIO1 -> FIOSET |= (1 << 26);//SACO UN 1 LOGICO
        }
        else{
            LPC_GPIO1 -> FIOCLR |= (1 << 26);//SACO UN 0 LOGICO

        }
    }

}