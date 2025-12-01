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

typedef struct {
    float x;
    float y;
    float z;
} Posicao_SST;

typedef struct{
    float roll; //eixo longitudinal
    float pitch; //eixo lateral
    float yaw; //eixo vertical
    uint16_t disc_x;
    uint16_t disc_y;
    uint16_t disc_z;

} Atitude_SST;

typedef struct{
    float vx;
    float vy;
    float vz;
} Velocidade_SST;

typedef struct
{
  uint16_t sinc_code;
  Modo_Operacao modo_inicial;
  Sensores_internos payload;
  Sensores_externos ofload;
  uint8_t id_atual;
  Posicao_SST posicao_atual;
  Atitude_SST atitude_atual;
  Velocidade_SST velocidade_atual;

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

void simular_orbita(Pacote_Dados *pacote, float dt){
    // X_novo = X_antigo + (V_x * dt)
    pacote->posicao_atual.x += pacote->velocidade_atual.vx * dt;
    
    // Y_novo = Y_antigo + (V_y * dt)
    pacote->posicao_atual.y += pacote->velocidade_atual.vy * dt;
    
    // Z_novo = Z_antigo + (V_z * dt)
    pacote->posicao_atual.z += pacote->velocidade_atual.vz * dt;

}

#define SENSITIVITY_FACTOR 0.00005f // Valor de ajuste para simulação (ex: radianos por ciclo)

void simular_atitude(Pacote_Dados *pacote, float dt){
    
    // Rotação no eixo X (Roll)
    pacote->atitude_atual.roll += (float)pacote->atitude_atual.disc_x * SENSITIVITY_FACTOR * dt;

    // Rotação no eixo Y (Pitch)
    pacote->atitude_atual.pitch += (float)pacote->atitude_atual.disc_y * SENSITIVITY_FACTOR * dt;

    // Rotação no eixo Z (Yaw)
    pacote->atitude_atual.yaw += (float)pacote->atitude_atual.disc_z * SENSITIVITY_FACTOR * dt;

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

   
    printf("[POSIÇÃO] X: %.2f | Y: %.2f | Z: %.2f\n", pacote.posicao_atual.x, 
                                                     pacote.posicao_atual.y, 
                                                     pacote.posicao_atual.z);
    
    printf("[ATITUDE] R: %.2f | P: %.2f | Y: %.2f | D_X: %u | D_Y: %u | D_Z: %u\n", 
                                                    pacote.atitude_atual.roll,
                                                    pacote.atitude_atual.pitch,
                                                    pacote.atitude_atual.yaw,
                                                    pacote.atitude_atual.disc_x,
                                                    pacote.atitude_atual.disc_y,
                                                    pacote.atitude_atual.disc_z);

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
    
    // 💡 Definimos o passo de tempo da simulação (em segundos)
    float dt = 0.5f; 

    //INICIALIZAÇÃO:
    Satelite.sinc_code = 0x12;
    Satelite.modo_inicial = MODO_SAFE;
    Satelite.id_atual = 0;

    // ✅ CORREÇÃO: Inicialização de Posição (X, Y, Z)
    Satelite.posicao_atual.x = 100000.0f; 
    Satelite.posicao_atual.y = 100000.0f;
    Satelite.posicao_atual.z = 0.0f;

    // ✅ Inicialização de Velocidade (Vx, Vy, Vz)
    Satelite.velocidade_atual.vx = 50.0f; 
    Satelite.velocidade_atual.vy = 25.0f;
    Satelite.velocidade_atual.vz = 0.0f;

    // ✅ Inicialização de Atitude e Discos (Comando)
    Satelite.atitude_atual.roll = 0.0f;
    Satelite.atitude_atual.pitch = 0.0f;
    Satelite.atitude_atual.yaw = 0.0f;
    // Comandos de disco iniciais: apenas X em rotação
    Satelite.atitude_atual.disc_x = 1000; 
    Satelite.atitude_atual.disc_y = 0;
    Satelite.atitude_atual.disc_z = 0;

    uint32_t tensao = 6000;

    // Teste 1: Descarregando a bateria (com Posição/Atitude)
    while (tensao > 500)
    {
        Satelite.id_atual++;
        ler_sensores(&Satelite.payload, -3, tensao);
        gerenciar_modo(&Satelite);
        
        // 💡 INTEGRAÇÃO: Simulação de Movimento e Rotação
        simular_orbita(&Satelite, dt);
        simular_atitude(&Satelite, dt);
        
        imprimir_pacote(Satelite);
        tensao -= 200;
    }

    // Teste 2: Carregando a bateria
    while (tensao < 6000)
    {
        Satelite.id_atual++;
        ler_sensores(&Satelite.payload, -7, tensao);
        gerenciar_modo(&Satelite);

        // 💡 INTEGRAÇÃO: Simulação de Movimento e Rotação
        simular_orbita(&Satelite, dt);
        simular_atitude(&Satelite, dt);
        
        imprimir_pacote(Satelite);
        tensao += 500;
    }

    executar_comando(&Satelite, 2); // Entra em modo TRANSMISSAO

    // Teste 3: Comandos de Disco (eixo X)
    for(size_t i = 1; i<=5; i++){
        Satelite.id_atual++;
        ler_sensores(&Satelite.payload, -5, tensao);
        gerenciar_modo(&Satelite);
        
        // 💡 INTEGRAÇÃO: Simulação de Movimento e Rotação
        simular_orbita(&Satelite, dt);
        simular_atitude(&Satelite, dt);
        
        imprimir_pacote(Satelite);
        tensao += 500;
    }
    
    // Parando o disco X e ligando o disco Y para mudar a rotação!
    Satelite.atitude_atual.disc_x = 0;
    Satelite.atitude_atual.disc_y = 2000;
    printf("[NOTA] Mudando o comando de disco: X=0, Y=2000!\n");

    executar_comando(&Satelite, 1); // Entra em modo CIENCIA

    // Teste 4: Comandos de Disco (eixo Y)
    for(size_t i = 1; i<=5; i++){
        Satelite.id_atual++;
        ler_sensores(&Satelite.payload, -1, tensao);
        gerenciar_modo(&Satelite);
        
        // 💡 INTEGRAÇÃO: Simulação de Movimento e Rotação
        simular_orbita(&Satelite, dt);
        simular_atitude(&Satelite, dt);
        
        imprimir_pacote(Satelite);
        tensao += 500;
    }
}