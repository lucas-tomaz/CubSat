#include<stdio.h>
#include<stdint.h>
#include<time.h>

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
    Sensor_Externo_ciencia dados_ciencia;
    Sensor_Externo_safe dados_safe;
    Sensor_Externo_transmissao dados_transmissao;
} Sensores_externos;

typedef struct
{
  uint16_t sinc_code;
  Modo_Operacao modo_inicial;
  Sensores_internos payload;
  Sensores_externos ofload;
  uint8_t id_atual;

} Pacote_Dados;

void delay(uint32_t t){
    uint32_t time_ms = 1000*t;
    clock_t start_time = clock();
    while (clock() < start_time + time_ms);
}

void gerenciar_modo(Pacote_Dados *objeto){
    if(objeto->payload.status_bat_atual == MODO_CRITICO){
        objeto->modo_inicial = MODO_SAFE;
        modo_safe(&objeto->ofload.dados_safe);
    }
    else if (objeto->modo_inicial == MODO_TRANSMISSAO)
    {
        objeto->modo_inicial = MODO_TRANSMISSAO;
        modo_transmissao(&objeto->ofload.dados_transmissao);
    }
    else{
        objeto->modo_inicial = MODO_CIENCIA;
        modo_ciencia(&objeto->ofload.dados_ciencia);
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
    printf("[Sensores Internos] Volt: %d | Temp: %d | BatStatus: %d\n", pacote.payload.voltagem_bat, 
                                                                        pacote.payload.tempertura, 
                                                                        pacote.payload.status_bat_atual);
    if(pacote.modo_inicial == MODO_CIENCIA){
        printf("[Sensores Externos] Pressao: %d | Temp_Ext: %d | Rad: %d | Oper: %d\n", pacote.ofload.dados_ciencia.pressao, 
                                                                                        pacote.ofload.dados_ciencia.temperatura_externa, 
                                                                                        pacote.ofload.dados_ciencia.radiacao, 
                                                                                        pacote.ofload.dados_ciencia.operacao);
    }
    else if(pacote.modo_inicial == MODO_TRANSMISSAO){
        printf("[Sensores Externos] Pressao: %d | Temp_Ext: %d | Rad: %d | Oper: %d\n", pacote.ofload.dados_transmissao.pressao, 
                                                                                        pacote.ofload.dados_transmissao.temperatura_externa, 
                                                                                        pacote.ofload.dados_transmissao.radiacao, 
                                                                                        pacote.ofload.dados_transmissao.operacao);
    }
    else{
        printf("[Sensores Externos] Pressao: %d | Temp_Ext: %d | Rad: %d | Oper: %d\n", pacote.ofload.dados_safe.pressao, 
                                                                                        pacote.ofload.dados_safe.temperatura_externa, 
                                                                                        pacote.ofload.dados_safe.radiacao, 
                                                                                        pacote.ofload.dados_safe.operacao);
    }
    printf("=====================================================================================\n");
}

void executar_comando(Pacote_Dados *p, uint16_t comando){
    if(comando == 1){
        p->modo_inicial = MODO_CIENCIA;
        modo_ciencia(&p->ofload.dados_ciencia);
        printf("[NOTA] Entrando em modo de exploracao!\n");
    }
    else if (comando == 2)
    {
        p->modo_inicial = MODO_TRANSMISSAO;
        modo_transmissao(&p->ofload.dados_transmissao);
        printf("[NOTA] Entrando em modo de transmissao!\n");
    }
}

int main(){
    Pacote_Dados Satelite;


    //INICIALIZAÇÃO:
    Satelite.sinc_code = 0x12;
    Satelite.modo_inicial = MODO_SAFE;
    Satelite.id_atual = 0;

    uint32_t tensao = 6000;

    //Teste descarregando a bateria:
    while (tensao > 500)
    {
        Satelite.id_atual++;
        ler_sensores(&Satelite.payload, -3, tensao);
        gerenciar_modo(&Satelite);
        imprimir_pacote(Satelite);
        tensao -= 200;
    }

    //Teste carregando a bateria:
    while (tensao < 6000)
    {
        Satelite.id_atual++;
        ler_sensores(&Satelite.payload, -7, tensao);
        gerenciar_modo(&Satelite);
        imprimir_pacote(Satelite);
        tensao += 500;
    }

    executar_comando(&Satelite, 2);

    for(size_t i = 1; i<=5; i++){
        Satelite.id_atual++;
        ler_sensores(&Satelite.payload, -5, tensao);
        gerenciar_modo(&Satelite);
        imprimir_pacote(Satelite);
        tensao += 500;
    }

    executar_comando(&Satelite, 1);

    for(size_t i = 1; i<=5; i++){
        Satelite.id_atual++;
        ler_sensores(&Satelite.payload, -1, tensao);
        gerenciar_modo(&Satelite);
        imprimir_pacote(Satelite);
        tensao += 500;
    }
}