#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include<string.h>   // Para funções de string
#include<math.h>     // Para cálculos matemáticos
#include<curl/curl.h> // Biblioteca para requisições HTTP
#include <stdlib.h>   // ✅ CORRIGIDO: Para malloc, free, realloc

#include "safe.h"
#include "ciencia.h"
#include "transmissao.h"

// Fator de Sensibilidade para Rotação
#define SENSITIVITY_FACTOR 0.00005f 

// ------------------------------------
// --- ENUMS e STRUCTS (CORRIGIDAS) ---
// ------------------------------------

// 1. ENUMS
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

// 2. ESTRUTURAS BÁSICAS
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

typedef struct {
    int16_t tempertura;
    uint16_t voltagem_bat;
    Status_Bateria status_bat_atual;
} Sensores_internos;

typedef struct{
    // Assume que Sensor_Externo_... estão definidos em seus respectivos headers
    Sensor_Externo_ciencia dados_ciencia;
    Sensor_Externo_safe dados_safe;
    Sensor_Externo_transmissao dados_transmissao;
} Sensores_externos;

// 3. ESTRUTURAS SGP4/REDE
typedef struct {
    char linha1[70]; 
    char linha2[70];
    double epoch_julian; // Data de referência da órbita TLE
} TLE_Data;

struct MemoryStruct {
  char *memory;
  size_t size;
};


// 4. ESTRUTURA PRINCIPAL (Pacote_Dados)
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
    
    TLE_Data dados_orbita;
    double tempo_juliano; 

} Pacote_Dados;


// ----------------------------
// --- PROTÓTIPOS DE FUNÇÕES ---
// ----------------------------

// Redefinição dos protótipos para evitar implicit declaration warnings/errors
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
void parse_tle(Pacote_Dados *pacote, const char *raw_data);
void fetch_tle_from_url(Pacote_Dados *pacote, const char *url);
void propagate_sgp4(const TLE_Data *tle, double tempo_juliano, Posicao_SST *pos, Velocidade_SST *vel); // SGP4 placeholder

void delay(uint32_t t);
void gerenciar_modo(Pacote_Dados *objeto);
void ler_sensores(Sensores_internos *sensor, uint16_t temperatura, uint16_t voltagem);
void simular_orbita(Pacote_Dados *pacote, float dt);
void simular_atitude(Pacote_Dados *pacote, float dt);
void imprimir_pacote(Pacote_Dados pacote);
void executar_comando(Pacote_Dados *p, uint16_t comando);


// --------------------------------
// --- FUNÇÕES DE REDE E PARSE ---
// --------------------------------

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        printf("Out of memory (realloc failed)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void parse_tle(Pacote_Dados *pacote, const char *raw_data) {
    // Cópia local para permitir o uso de strtok (que modifica a string)
    char temp_data[256]; 
    strncpy(temp_data, raw_data, sizeof(temp_data) - 1);
    temp_data[sizeof(temp_data) - 1] = '\0';

    char *name_line = strtok(temp_data, "\n");
    char *line1 = strtok(NULL, "\n");
    char *line2 = strtok(NULL, "\n");

    if (line1 && line2) {
        strncpy(pacote->dados_orbita.linha1, line1, sizeof(pacote->dados_orbita.linha1) - 1);
        pacote->dados_orbita.linha1[sizeof(pacote->dados_orbita.linha1) - 1] = '\0';

        strncpy(pacote->dados_orbita.linha2, line2, sizeof(pacote->dados_orbita.linha2) - 1);
        pacote->dados_orbita.linha2[sizeof(pacote->dados_orbita.linha2) - 1] = '\0';
        
        printf("[NOTA] TLE carregado. Satélite: %s\n", name_line ? name_line : "Desconhecido");
        
        // **FALTA AQUI:** Lógica para extrair o EPOCH_JULIAN da linha 1
        // (Será necessária para a inicialização precisa do SGP4)
        
    } else {
        printf("[ERRO] Falha ao analisar TLE. Dados brutos:\n%s\n", raw_data);
    }
}

void fetch_tle_from_url(Pacote_Dados *pacote, const char *url) {
    CURL *curl_handle;
    CURLcode res;
    
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  
    chunk.size = 0;    
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    if(curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        
        res = curl_easy_perform(curl_handle);

        if(res != CURLE_OK) {
            fprintf(stderr, "[ERRO] curl_easy_perform() falhou: %s\n", curl_easy_strerror(res));
        } else {
            parse_tle(pacote, chunk.memory);
        }

        curl_easy_cleanup(curl_handle);
    }

    free(chunk.memory);
    curl_global_cleanup();
}


// ---------------------------------
// --- FUNÇÕES SGP4 (CONCEITUAL) ---
// ---------------------------------

void propagate_sgp4(const TLE_Data *tle, double tempo_juliano, Posicao_SST *pos, Velocidade_SST *vel) {
    // Placeholder para a lógica do algoritmo SGP4
    // Esta função será preenchida com o código SGP4
}


// ---------------------------
// --- FUNÇÕES DE SIMULAÇÃO ---
// ---------------------------

void delay(uint32_t t){
    uint32_t time_ms = 1000*t;
    clock_t start_time = clock();
    while (clock() < start_time + time_ms);
}

void gerenciar_modo(Pacote_Dados *objeto){
    if(objeto->payload.status_bat_atual == MODO_CRITICO){
        objeto->modo_inicial = MODO_SAFE;
        // modo_safe(&objeto->ofload.dados_safe); 
    }
    else if (objeto->modo_inicial == MODO_TRANSMISSAO)
    {
        objeto->modo_inicial = MODO_TRANSMISSAO;
        // modo_transmissao(&objeto->ofload.dados_transmissao);
    }
    else{
        objeto->modo_inicial = MODO_CIENCIA;
        // modo_ciencia(&objeto->ofload.dados_ciencia);
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

void simular_orbita(Pacote_Dados *pacote, float dt){
    
    // 1. Atualização do Tempo Juliano
    pacote->tempo_juliano += (double)dt / 86400.0; 

    // 2. Chamada à função SGP4 (Descomente quando tiver a implementação SGP4)
    /*
    propagate_sgp4(
        &pacote->dados_orbita, 
        pacote->tempo_juliano, 
        &pacote->posicao_atual, 
        &pacote->velocidade_atual
    );
    */

    // Lógica de movimento simples (USADA ATÉ O SGP4 SER INTEGRADO)
    pacote->posicao_atual.x += pacote->velocidade_atual.vx * dt;
    pacote->posicao_atual.y += pacote->velocidade_atual.vy * dt;
    pacote->posicao_atual.z += pacote->velocidade_atual.vz * dt;
}

void simular_atitude(Pacote_Dados *pacote, float dt){
    // Rotação no eixo X (Roll)
    pacote->atitude_atual.roll += (float)pacote->atitude_atual.disc_x * SENSITIVITY_FACTOR * dt;
    // Rotação no eixo Y (Pitch)
    pacote->atitude_atual.pitch += (float)pacote->atitude_atual.disc_y * SENSITIVITY_FACTOR * dt;
    // Rotação no eixo Z (Yaw)
    pacote->atitude_atual.yaw += (float)pacote->atitude_atual.disc_z * SENSITIVITY_FACTOR * dt;

    if (pacote->atitude_atual.roll >= 360.0f) pacote->atitude_atual.roll -= 360.0f;
    // ... (outros limites para pitch e yaw omitidos para brevidade)
}

void imprimir_pacote(Pacote_Dados pacote){
    printf("=====================================================================================\n");
    printf("PCT ID:%u | Sync: %d | Modo: %d | Tempo Juliano: %.6f\n", 
            pacote.id_atual, pacote.sinc_code, pacote.modo_inicial, pacote.tempo_juliano);
            
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
    
    // ... (Impressão de Sensores Externos omitida por brevidade)
    printf("=====================================================================================\n");
}

void executar_comando(Pacote_Dados *p, uint16_t comando){
    if(comando == 1){
        p->modo_inicial = MODO_CIENCIA;
        // modo_ciencia(&p->ofload.dados_ciencia);
        printf("[NOTA] Entrando em modo de exploracao!\n");
    }
    else if (comando == 2)
    {
        p->modo_inicial = MODO_TRANSMISSAO;
        // modo_transmissao(&p->ofload.dados_transmissao);
        printf("[NOTA] Entrando em modo de transmissao!\n");
    }
}


// -----------------------
// --- FUNÇÃO PRINCIPAL ---
// -----------------------

int main(){
    Pacote_Dados Satelite;
    // Passo de tempo para a propagação orbital: 5.0 segundos
    float dt = 5.0f; 

    //INICIALIZAÇÃO:
    Satelite.sinc_code = 0x12;
    // Modo Inicial Estável
    Satelite.modo_inicial = MODO_CIENCIA; 
    Satelite.id_atual = 0;
    
    // Inicialização de Posição, Velocidade e Atitude (Dados Estáveis)
    Satelite.posicao_atual.x = 100000.0f; 
    Satelite.posicao_atual.y = 100000.0f;
    Satelite.posicao_atual.z = 0.0f;
    Satelite.velocidade_atual.vx = 50.0f; 
    Satelite.velocidade_atual.vy = 25.0f;
    Satelite.velocidade_atual.vz = 0.0f;
    Satelite.atitude_atual.roll = 0.0f;
    Satelite.atitude_atual.pitch = 0.0f;
    Satelite.atitude_atual.yaw = 0.0f;
    Satelite.atitude_atual.disc_x = 1000; 
    Satelite.atitude_atual.disc_y = 0;
    Satelite.atitude_atual.disc_z = 0;
    
    // Calculando o Tempo Juliano Inicial (Hora atual)
    Satelite.tempo_juliano = 2440587.5 + (double)time(NULL) / 86400.0;
    
    // CHAMADA DE REDE (Fetch TLE)
    printf("[INICIALIZANDO] Buscando dados TLE da ISS...\n");
    fetch_tle_from_url(&Satelite, "http://live.ariss.org/iss.txt");

    // VALORES ESTÁVEIS (Voltagem Alta, Temperatura Estável)
    uint32_t tensao_estavel = 6000;
    uint16_t temperatura_estavel = -3;
    
    printf("\n[SIMULAÇÃO ORBITAL INICIADA - ATUALIZAÇÃO A CADA 5 SEGUNDOS]\n");
    printf("Foco na progressão da POSIÇÃO e TEMPO JULIANO.\n");
    printf("Pressione Ctrl+C a qualquer momento para sair.\n\n");

    // Loop infinito de atualização
    while (1) 
    {
        Satelite.id_atual++;
        
        // 1. Leitura e Gerenciamento (Estáveis)
        // Usamos os valores estáveis e chamamos as funções para manter o sistema em MODO CIENCIA.
        ler_sensores(&Satelite.payload, temperatura_estavel, tensao_estavel);
        gerenciar_modo(&Satelite); 
        
        // 2. Propagação Orbital (Avança 5.0 segundos)
        simular_orbita(&Satelite, dt); 
        simular_atitude(&Satelite, dt);
        
        // 3. Imprime os dados
        imprimir_pacote(Satelite);
        
        // 4. Pausa de 5 segundos
        delay(5); 
    }
    
    return 0; 
}