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

void ler_sensores_externos(Sensor_Externo_ciencia *sensor, 
                         uint16_t pressao, uint16_t temperatura, uint16_t rad);
void modo_ciencia(Sensor_Externo_ciencia *m);