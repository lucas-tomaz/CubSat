#include "ciencia.h"

void ler_sensores_externos(Sensor_Externo_ciencia *sensor, 
                              uint16_t pressao, 
                              uint16_t temperatura, 
                              uint16_t rad){
     sensor->pressao = pressao;
     sensor->temperatura_externa = temperatura;
     sensor->radiacao = rad;
     sensor->operacao = LIGADO;
}
void modo_ciencia(Sensor_Externo_ciencia *m){
     int16_t pressao = 14;
     int16_t temp_ext = -5;
     int16_t rad = 3;
     m->operacao = DESLIGADO;
     m->pressao = 0;
     m->radiacao = 0;
     m->temperatura_externa = 0;
     ler_sensores_externos(m, pressao, temp_ext, rad);
}