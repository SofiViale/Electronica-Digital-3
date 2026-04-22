/* 
 * Configurar el Timer 0 para que su Match genere un evento cada 10 ms. Este 
evento debe disparar automáticamente una conversión en el canal 
 * AD0.0 (P0.23). No se debe usar el Handler del Timer para iniciar la 
conversión. Si el valor convertido es mayor a 2.5 V, se debe encender un 
 * LED en P2.0. Si es menor, se debe apagar. 
 * Suponer CCLK = 100 MHz.
 */ 
 
#ifdef __USE_CMSIS 
#include "LPC17xx.h" 
#endif 
 
#include <cr_section_macros.h> 
 
 
#define LED 0 
#define ENTRADA 23 
#define VALOR_10MS 250000 
 
void configPtos(void); 
void configInt(void); 
void configTimer0(void); 
void configADC(void); 
void ADC_IRQHandler(void); 
 
int main(void) { 
 
    configPtos(); 
    configInt(); 
    configTimer0(); 
    configADC(); 
 
    LPC_GPIO2 -> FIOCLR = (1 << LED);
 
    while(1){ 
 
     __WFI();
    } 
 
    return 0 ; 
} 
 
void configPtos(void){ 
 
    LPC_PINCON -> PINSEL4 &= ~(3 << LED); //esta mal configurado, estaba para una funcion reservada lo pongo en gpio
    LPC_PINCON -> PINSEL1 |= (1 << (ENTRADA - 16)*2);//pin del adc
    LPC_PINCON -> PINMODE4 |= (2 << LED);//ni pull up ni pull down para el led
    LPC_PINCON -> PINMODE1 |= (2 << (ENTRADA - 16)*2); //lo mismo para el pin del adc
    LPC_GPIO2 -> FIODIR |= (1 << LED); //pin como salida
} 
 
void configInt(void){ 
     NVIC -> ISER[0] |= (1 << 22); //prende la interrup del nvic en el adc
} 
 
void configTimer0(void){ 
 
    LPC_SC -> PCONP |= (1 << 1); //se prende el timer en el pconp
    LPC_SC -> PCLKSEL0 &= ~(3 << 2); //se divide por 4 la entrada de la frec al timer, le entran 25M
    
    LPC_TIM0 -> TCR |= ((1 << 0) | (1 << 1));//esta linea deberia estar al final, primero se lo configura y por ultimo se lo prende
    LPC_TIM0 -> TCR &= ~(1 << 1); 
    LPC_TIM0 -> CTCR &= ~(3 << 0);//se pone el timer0 en timer mode 
    LPC_TIM0 -> PR = 0;  
    LPC_TIM0 -> MR1 = VALOR_10MS - 1;//matchea cada 10ms
    LPC_TIM0 -> MCR &= ~(1 << 3); //interrumpe el match0.1
    LPC_TIM0 -> MCR |= (1 << 4); //resetea
    LPC_TIM0 -> MCR &= ~(1 << 5); //no frena
} 
 
void configADC(void){ 
 
    LPC_SC -> PCONP |= (1 << 12); //prendo el adc
    LPC_ADC -> ADCR |= (1 << 21); //pdn en 1
    LPC_SC -> PCLKSEL0 &= ~(3 << 24); //se divide la frec por 4, ingresa 25Mhz
    LPC_ADC -> ADCR |= (1 << 0); //conecta el canal 0
    LPC_ADC -> ADCR |= (1 << 8); // se puede dividir por 3, pero es mejor hacerlo por 2
    LPC_ADC -> ADCR &= ~(1 << 16);//modo burst desactivado
    LPC_ADC -> ADCR |= (4 << 24); //start en 100
    LPC_ADC -> ADCR &= ~(1 << 27); //flanco de subida
    LPC_ADC -> ADINTEN |= (1 << 0); //activa interrup en adc
} 
void ADC_IRQHandler(void){ 

    if((LPC_ADC -> ADGR >> 31) & 1){ //SI EL BIT DONE ES 1

        volatile uint32_t resultado = (LPC_ADC -> ADGR >> 4) & 0xFFF;
        if(resultado > 3103){
            LPC_GPIO2 -> FIOSET = (1 << LED);//PRENDE EL LED
        }
        else{
            LPC_GPIO2 -> FIOCLR = (1 << LED);//APAGO EL LED
        }
    }
    
}