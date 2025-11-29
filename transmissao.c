#include "transmissao.h"

void ler_sensores_externos_transmissao(Sensor_Externo_transmissao *sensor){
     sensor->pressao = 0;
     sensor->temperatura_externa = 0;
     sensor->radiacao = 0;
     sensor->operacao = DESLIGADO_TRANSMISSAO;
}

void modo_transmissao(Sensor_Externo_transmissao *p){
     p->operacao = DESLIGADO_TRANSMISSAO;
     p->pressao = 0;
     p->radiacao = 0;
     p->temperatura_externa = 0;
     ler_sensores_externos_transmissao(p);
}