/*Realizar un programa para la LPC1769 que utilice el Timer0 en modo match para generar una interrupción periódica cada 250 ms.

Datos dados
Frecuencia de CPU: CCLK = 100 MHz
Frecuencia del reloj periférico del Timer0: PCLK_TIMER0 = CCLK/4
Se debe utilizar el canal MR0 del Timer0
Requerimientos
Configurar un pin GPIO como salida para conectar un LED.
Configurar el Timer0 para que:
genere una interrupción cuando ocurra el match con MR0
reinicie el contador en cada match
continúe funcionando de manera periódica
Habilitar la interrupción del Timer0 en el NVIC usando registros.
Implementar la rutina de interrupción correspondiente.
Cada vez que ocurra la interrupción, se debe conmutar el estado del LED.
Se pide
Configurar correctamente los registros necesarios del GPIO, Timer0 y NVIC.
Calcular el valor del prescaler y el valor de MR0 para obtener una interrupción cada 250 ms.
Explicar los cálculos realizados.
Comportamiento esperado
El LED debe cambiar de estado cada 250 ms.
Por lo tanto, el ciclo completo encendido/apagado será de 500 ms.*/


#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#define TIMER0 1
void config_timer(void);
void config_ports(void);

int main(void){

    config_ports();
    config_timer();

    while(1);

    return 0;

}

void config_ports(void){

    LPC_PINCON -> PINSEL3 &= ~(3 << 20); // pin 1.26 como gpio
    LPC_GPIO1 -> FIODIR |= (1 << 26); // SALIDA
    LPC_GPIO1 -> FIOMASK = 0xFFFFFFFF; // ENMASCARO TODO EL PUERTO
    LPC_GPIO1 -> FIOMASK &= ~(1 << 26); //DESENMASCARO SOLO EL PIN QUE VOY A USAR

}


//interrupcion cada 250ms, frecuencia del cpu 100Mhz,
//frecuencia que le llega al timer por defecto 25Mhz
//pr=24.999 para que el TC aumente cada 1ms y cargo el match en 249
void config_timer(void){

    LPC_TIM0 -> PR = 24999;
    LPC_TIM0 -> MR0 = 249;
    LPC_TIM0 -> MCR |= (3); //HABILITO LA INTERRUPCION Y EL RESET EN EL REGISTRO DEL TIMER
    LPC_TIM0 -> TCR |= (1); //HABILITO EL COUNTER
    NVIC->ISER[0] |= (1 << TIMER0);//HABILITO INTERRUP EN NVIC
}

void TIMER0_IRQHandler(void){

    LPC_TIM0 -> IR = (1 <<0); //LIMPIO LA FLAG
    if ((LPC_GPIO1 -> FIOPIN & (1 << 26))){
        LPC_GPIO1 -> FIOCLR = (1 << 26);
    }
    else{
        LPC_GPIO1 -> FIOSET = (1 << 26);
    }

}
