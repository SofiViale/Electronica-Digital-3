/* 
 * Utilizando únicamente el Timer 3, configure el sistema para cumplir con los 
siguientes requisitos simultáneos: 
 * "Hardware (MAT3.0): El pin P0.10 (MAT3.0) debe generar una señal cuadrada de 
1 kHz de forma automática (Toggle por hardware). 
 * Software (Interrupción): Cada vez que ocurra el Match de esa señal de 1 kHz, 
el micro debe entrar a un Handler para incrementar 
 * un contador global llamado pulsos. 
 * Segundo Evento (Match 1): Cuando el contador pulsos llegue a 1000 (lo que 
indicaría que pasó 1 segundo), se debe prender el LED rojo (P0.22) 
 * usando GPIO y detener el Timer 3 por completo. 
 * Datos técnicos: 
 * CCLK = 100 MHz 
 * PCLK = CCLK/4 (25 MHz) 
 */ 

 #ifdef __USE_CMSIS
#include "LPC17xx.h"
#include <stdbool.h>
#endif
#define TM3 4

volatile uint32_t pulsos = 0;
volatile bool contar = false;

void config_ports();
void config_timer();

int main(void){

    config_ports();
    config_timer();

    while(1);

    return 0;
}

void config_ports(){

    LPC_PINCON->PINSEL0 &= ~(3 << 20);
    LPC_PINCON->PINSEL0 |=  (3 << 20);

    LPC_PINCON -> PINSEL1 &= ~(3 << 12);//LED ROJO GPIO
    LPC_GPIO0 -> FIODIR |=(1 << 22);//PONGO AL PIN COMO SALIDA
    LPC_GPIO0->FIOCLR  = (1 << 22);

}


void config_timer(){

    LPC_SC -> PCONP |= (1 << 23); //ALIMENTO AL TIMER 3
    LPC_SC -> PCLKSEL1 &= ~(3 << 14);//LE ENTRA 25Mhz AL TIMER3

    LPC_TIM3 -> PR = 12499;
    LPC_TIM3 -> MR0 = 0;
    LPC_TIM3 -> MCR |= (3 << 0);
    LPC_TIM3 -> EMR |= (3 << 4);//TOGGLE

    NVIC -> ISER[0] |=(1 << TM3);

    LPC_TIM3->TCR = 2;   // reset
    LPC_TIM3 -> TCR = (1);    //START

}

void TIMER3_IRQHandler(void){

    if(contar){
        pulsos++;
        contar = false;
        if(pulsos == 1000){
            LPC_GPIO0 -> FIOSET |= (1 << 22);//PRENDO EL LED
            LPC_TIM3->TCR = 0;
        }
    }else{
        contar = true;
    }
    LPC_TIM3 -> IR = (1 << 0); 

}