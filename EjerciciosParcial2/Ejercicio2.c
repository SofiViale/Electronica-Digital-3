/*Por un pin del ADC del microcontrolador LPC1769 ingresa una tensión de rango dinámico 0 a 3,3 [V] proveniente de un sensor de temperatura. 
Debido a la baja tasa de variación de la señal, se pide tomar una muestra cada 30[s]. Pasados los 2[min] se debe promediar las últimas 4 muestras y, 
en función de este valor, tomar una decisión sobre una salida digital de la placa.

Si el valor es < 1 [V], colocar la salida en 0 (0[V]).
Si el valor es >= 1[V] y <= 2[V], sacar una señal intermitente de f = ???.
Si el valor es > 2[V], colocar la salida en 1 (3,3[V]).

CCLOCK = 100 MHz*/


#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"

volatile uint32_t cont = 0;
volatile uint32_t promedio = 0;

void config_ports();
void config_adc();
void config_timer0();

int main(void){

    config_ports();
    config_adc();
    config_timer0();

    while(1){};

    return 0;
}



void config_ports(void){
    //SALIDA DIGITAL DE LA PLACA 0.16
    PINSEL_CFG_Type config = {0};

    config.Portnum = PINSEL_PORT_0;
    config.Pinnum = PINSEL_PIN_16;
    config.Funcnum = PINSEL_FUNC_0;//lo pongo como gpio
    config.OpenDrain = PINSEL_PINMODE_NORMAL;
    GPIO_SetDir(0,16,1);

    PINSEL_ConfigPin(&config);
    //CONFIGURO ENTRADA PARA ADC

    config.Portnum = PINSEL_PORT_0;
    config.Pinnum = PINSEL_PIN_23;
    config.Funcnum = PINSEL_FUNC_1;//lo pongo como adc
    config.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&config);

}

void config_adc(void){
    
    ADC_Init(LPC_ADC, 200000); //PRENDO ADC
    ADC_BurstCmd(LPC_ADC, DISABLE); //DESHABILITO MODO BURST
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, enable); //HABILITO EL CANAL 0
    ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, enable); //HABILITO INTERRUPCION EN ADC
    ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01); //CONVIERTE CUANDO MATCHEA POR 0.1

    NVIC_EnableIRQ(ADC_IRQn);//HABILITO INTERRUPCION EN NVIC

}

void config_timer0(void){

    TIM_TIMERCFG_Type config = {0};

    config.PrescaleOption = TIM_PRESCALE_USVAL;
    config.PrescaleValue = 1000;
    
    TIM_MATCHCFG_Type match= {0};
    match.MatchChannel = 1;
    match.IntOnMatch = enable;
    match.ResetOnMatch = enable;
    match.StopOnMatch = disable;
    match.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    match.MatchValue = 29999;

    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &config);
    TIM_ConfigMatch(LPC_TIM0, &match);

    NVIC_DisableIRQ(TIMER0_IRQn);
    TIM_Cmd(LPC_TIM0, ENABLE);
}

void ADC_IRQHandler(void){
    uint32_t resultado = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
    cont ++;

    if(cont <= 4){
        promedio = promedio + resultado;
    }
    if(cont == 4){
        
        cont = 0;
        promedio = promedio/4;
        if(promedio < 1241 && promedio > 0){

            NVIC_DisableIRQ(TIMER0_IRQn);
            LPC_GPIO0->FIOCLR = (1 << 16);
            
        }
        else if(promedio > 2428){

            NVIC_DisableIRQ(TIMER0_IRQn);
            LPC_GPIO0->FIOSET = (1 << 16);
        }
        else{
            NVIC_EnableIRQ(TIMER0_IRQn);
        }
        promedio = 0;
    }
}

void TIMER0_IRQHandler(void){
    TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);
    
    if(!(LPC_GPIO0->FIOPIN & (1 << 16))){ 
        LPC_GPIO0->FIOSET = (1 << 16);
    }
    else{
        LPC_GPIO0->FIOCLR = (1 << 16);
    }
}