#ifndef TRANSMISSAO_H
#define TRANSMISSAO_H

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

void ler_sensores_externos_transmissao(Sensor_Externo_transmissao *sensor);
void modo_transmissao(Sensor_Externo_transmissao *p);

#endif