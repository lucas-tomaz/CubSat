#include<stdio.h>
#include<stdint.h>

#include "safe.h"
#include "ciencia.h"
#include "transmissao.h"

typedef enum {
    MODO_CRITICO = 0,
    MODO_BAIXO = 1,
    MODO_ATIVO = 2
} Status_Bateria;

typedef enum {
    MODO_CIENCIA = 1,
    MODO_TRANSMISSAO = 2,
    MODO_SAFE = 0
} Modo_Operacao;

typedef struct {
    int16_t tempertura;
    uint16_t voltagem_bat;
    Status_Bateria status_bat_atual;
} Sensores_internos;

typedef struct{
    int16_t pressao;
    int16_t tempertura_externa;
    int16_t radiacao;
    Operacao_sensor_ciencia opciencia;
    Operacao_sensor_safe opsafe;
    Operacao_sensor_transmissao optransm;
} Sensores_externos;


typedef struct
{
  uint16_t sinc_code;
  Modo_Operacao modo_inicial;
  Sensores_internos payload;
  Sensores_externos ofload;
  uint8_t id_atual;

} Pacote_Dados;

void gerenciar_modo(Pacote_Dados *objeto){
    if(objeto->payload.status_bat_atual == MODO_CRITICO){
        objeto->modo_inicial = MODO_SAFE;
        modo_safe();
    }
    else if (objeto->modo_inicial == MODO_TRANSMISSAO)
    {
        objeto->modo_inicial = MODO_TRANSMISSAO;
        modo_transmissao();
    }
    else{
        objeto->modo_inicial = MODO_CIENCIA;
        modo_ciencia();
    }
}

void ler_sensores(Sensores_internos *sensor, uint16_t temperatura, uint16_t voltagem){
    sensor->tempertura = temperatura;
    sensor->voltagem_bat = voltagem;
    if(voltagem <= 1000){
        sensor->status_bat_atual = MODO_CRITICO;
    }
    else if (voltagem <= 5000)
    {
        sensor->status_bat_atual = MODO_BAIXO;
    }
    else{
        sensor->status_bat_atual = MODO_ATIVO;
    }
}

void imprimir_pacote(Pacote_Dados pacote){
    printf("=====================================================================================\n");
    printf("PCT ID:%u | Sync: %d | Modo: %d \n", pacote.id_atual, pacote.sinc_code, pacote.modo_inicial);
    printf("[Sensores Internos] Volt: %d | Temp: %d | BatStatus: %d\n", pacote.payload.voltagem_bat, pacote.payload.tempertura, pacote.payload.status_bat_atual);
    if(pacote.modo_inicial == MODO_CIENCIA){
        printf("[Sensores Externos] Pressao: %d | Temp_Ext: %d | Rad: %d | Oper: %d\n", pacote.ofload.pressao, pacote.ofload.tempertura_externa, pacote.ofload.radiacao, pacote.ofload.opciencia);
    }
    else if(pacote.modo_inicial == MODO_TRANSMISSAO){
        printf("[Sensores Externos] Pressao: %d | Temp_Ext: %d | Rad: %d | Oper: %d\n", pacote.ofload.pressao, pacote.ofload.tempertura_externa, pacote.ofload.radiacao, pacote.ofload.optransm);
    }
    else{
        printf("[Sensores Externos] Pressao: %d | Temp_Ext: %d | Rad: %d | Oper: %d\n", pacote.ofload.pressao, pacote.ofload.tempertura_externa, pacote.ofload.radiacao, pacote.ofload.opsafe);
    }
    printf("=====================================================================================\n");
}

int main(){
    Pacote_Dados Satelite;


    //INICIALIZAÇÃO:
    Satelite.sinc_code = 2;
    Satelite.modo_inicial = MODO_SAFE;
    Satelite.id_atual = 0;

    uint32_t tensao = 6000;

    while (tensao > 500)
    {
        Satelite.id_atual++;
        ler_sensores(&Satelite.payload, -3, tensao);
        gerenciar_modo(&Satelite);
        imprimir_pacote(Satelite);
        tensao -= 200;
    }
    
}