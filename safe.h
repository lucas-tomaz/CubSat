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

void ler_sensores_externos_safe(Sensor_Externo_safe *sensor);
void modo_safe(Sensor_Externo_safe *s);