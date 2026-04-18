/*Ejercicio: “Nivel analógico en LED”

Usá una entrada ADC para leer un potenciómetro y hacé que un pin de salida digital refleje el resultado así:

si la lectura ADC es mayor que la mitad de escala, ponés el pin de salida en 1
si la lectura ADC es menor o igual a la mitad, ponés el pin de salida en 0

*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif


void config_ports(void);
void config_adc(void);
void lectura(void);

int main(){
    config_ports();
    config_adc();
    
    while (1){
        
        lectura();

    }
    
    return 0;
}


void config_ports(){

    LPC_PINCON -> PINSEL1 &= ~(3 << 20);// EL PIN 0.26 COMO GPIO POR DONDE SACARE LA SEÑAL
    LPC_GPIO0 -> FIODIR |= (1 << 26); //LO CONFIGURO COMO SALIDA
    LPC_GPIO0 -> FIOMASK = 0xFFFFFFFF; //ENMASCARO TODO EL PUERTO POR LAS DUDAS
    LPC_GPIO0 -> FIOMASK &= ~(1 << 26); //DESENMASCARO EL PIN QUE ME IMPORTA EN ESTE CASO EL P0.26
    LPC_PINCON -> PINSEL1 |= (1 << 14);// CONFIGURO EL PIN 0.23 COMO ENTRADA DE ADC0, POR ACA ENTRA LA SEÑAL DEL POTENCIOMETRO 
    LPC_PINCON -> PINSEL1 &= ~(1 << 15);// CONFIGURO EL PIN 0.23 COMO ENTRADA DE ADC0, POR ACA ENTRA LA SEÑAL DEL POTENCIOMETRO

}

void config_adc(){

    LPC_SC->PCONP |= (1<<12); //PRENDO EL ADC
    LPC_SC->PCLKSEL0 |= (1<<24);// LO PONGO EN 01 PARA QUE NO SE DIVIDA EL PERIFERICAL CLOCK
    LPC_SC->PCLKSEL0 &= ~(1<<25);// LO PONGO EN 01 PARA QUE NO SE DIVIDA EL PERIFERICAL CLOCK
    LPC_ADC -> ADCR = 0x0195;//activo el canal 0 y divido por 1 el clkdiv 30Mhz/( x+ 1) = 200.000 60Mhz 
    LPC_ADC -> ADCR &= ~(1 << 16);//desactivo modo burst
    LPC_ADC -> ADCR |= (1 << 21);//conversor operacional
    LPC_ADC -> ADCR |= (1 << 24);//start convertion now
    LPC_ADC -> ADCR &= ~(1 << 25);//start convertion now
    LPC_ADC -> ADCR &= ~(1 << 26);//start convertion now
    LPC_ADC -> ADCR &= ~(1 << 27);//desactivo conversion por flancos
}

void lectura(){

    volatile uint32_t valor = 0;
    if((LPC_ADC -> ADGDR >> 31) & 1){
        
        valor = (LPC_ADC -> ADGDR >> 4) & 0xFFF;
        
        if(valor > 2047){

            LPC_GPIO0 -> FIOSET |= (1 << 26);//SACO UN 1 LOGICO
        }
        else{
            LPC_GPIO0 -> FIOCLR |= (1 << 26);//SACO UN 0 LOGICO

        }
    }
}