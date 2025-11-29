#include<stdio.h>
#include<stdint.h>

typedef enum{
     LIGADO_SAFE = 1,
     DESLIGADO_SAFE = 0
} Operacao_sensor_safe;

typedef struct{
     int16_t pressao;
     int16_t temperatura_externa;
     int16_t radiacao;
     Operacao_sensor_safe operacao;
} Sensor_Externo_safe;

void ler_sensores_externos_safe(Sensor_Externo_safe *sensor){
     sensor->pressao = 0;
     sensor->temperatura_externa = 0;
     sensor->radiacao = 0;
     sensor->operacao = DESLIGADO_SAFE;
}

void modo_safe(){
          Sensor_Externo_safe s;

     s.operacao = DESLIGADO_SAFE;
     s.pressao = 0;
     s.radiacao = 0;
     s.temperatura_externa = 0;
     //if(id >= 2){
     ler_sensores_externos_safe(&s);
          //printf("[Sensores Externos] Pressao: %d | Temp_Ext: %d | Rad: %d | Oper: %d\n",s.pressao, s.temperatura_externa, s.radiacao, s.operacao );
          //printf("=====================================================================================\n");
     //}
}