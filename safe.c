#include "safe.h"

void ler_sensores_externos_safe(Sensor_Externo_safe *sensor){
     sensor->pressao = 0;
     sensor->temperatura_externa = 0;
     sensor->radiacao = 0;
     sensor->operacao = DESLIGADO_SAFE;
}

void modo_safe(Sensor_Externo_safe *s){
     s->operacao = DESLIGADO_SAFE;
     s->pressao = 0;
     s->radiacao = 0;
     s->temperatura_externa = 0;
     ler_sensores_externos_safe(s);
}