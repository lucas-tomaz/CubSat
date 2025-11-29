#include<stdio.h>
#include<stdint.h>

typedef enum{
     LIGADO_TRANSMISSAO = 1,
     DESLIGADO_TRANSMISSAO = 0
} Operacao_sensor_transmissao;

typedef struct{
     int16_t pressao;
     int16_t temperatura_externa;
     int16_t radiacao;
     Operacao_sensor_transmissao operacao;
} Sensor_Externo_transmissao;

void ler_sensores_externos_transmissao(Sensor_Externo_transmissao *sensor){
     sensor->pressao = 0;
     sensor->temperatura_externa = 0;
     sensor->radiacao = 0;
     sensor->operacao = DESLIGADO_TRANSMISSAO;
}

void modo_transmissao(){
    Sensor_Externo_transmissao p;
     p.operacao = DESLIGADO_TRANSMISSAO;
     p.pressao = 0;
     p.radiacao = 0;
     p.temperatura_externa = 0;
     //if(id >= 2){
        ler_sensores_externos_transmissao(&p);
          //printf("[Sensores Externos] Pressao: %d | Temp_Ext: %d | Rad: %d | Oper: %d\n",p.pressao, p.temperatura_externa, p.radiacao, p.operacao );
          //printf("=====================================================================================\n");
     //}
}