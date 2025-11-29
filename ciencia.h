#include<stdio.h>
#include<stdint.h>

typedef enum{
     LIGADO = 1,
     DESLIGADO = 0
} Operacao_sensor_ciencia;

typedef struct{
     int16_t pressao;
     int16_t temperatura_externa;
     int16_t radiacao;
     Operacao_sensor_ciencia operacao;
} Sensor_Externo_ciencia;

void ler_sensores_externos(Sensor_Externo_ciencia *sensor, uint16_t pressao, uint16_t temperatura, uint16_t rad){
     sensor->pressao = pressao;
     sensor->temperatura_externa = temperatura;
     sensor->radiacao = rad;
     sensor->operacao = LIGADO;
}
void modo_ciencia(){
     Sensor_Externo_ciencia m;

     int16_t pressao = 14;
     int16_t temp_ext = -5;
     int16_t rad = 3;

     m.operacao = DESLIGADO;
     m.pressao = 0;
     m.radiacao = 0;
     m.temperatura_externa = 0;
     //if(id >= 2){
          ler_sensores_externos(&m, pressao, temp_ext, rad);
          //printf("[Sensores Externos] Pressao: %d | Temp_Ext: %d | Rad: %d | Oper: %d\n",m.pressao, m.temperatura_externa, m.radiacao, m.operacao );
         // printf("=====================================================================================\n");
    // }
}