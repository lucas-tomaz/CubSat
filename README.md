# CubSat Project
Repositório destinado a documentação do desenvolvimento de um código para simulação de um modelo de satélite do tipo CubSat.

### Descrição: 
Um CubeSat é um tipo de pequeno satélite (nanossatélite) com formato cúbico, padronizado em unidades de 10 cm (10 x 10 x 10) cm. Eles são usados para diversas finalidades, como observação da Terra, testes de tecnologia e experimentos científicos, sendo uma forma mais acessível e econômica de pesquisa espacial.

### Objetivo do projeto:
Este software tem como objetivo simular a arquitetura de dados de um CubeSat. O foco principal é o gerenciamento dos modos de operação (Safe, Exploração e Transmissão) e a simulação do fluxo completo de recebimento e processamento de pacotes de dados vindos dos sensores.

#### 🛰️ Modos de Operação do Sistema

O software implementa uma **Máquina de Estados** que alterna automaticamente o comportamento do CubeSat baseada na telemetria interna (nível de bateria) e no comando atual.

##### 1. 🧪 MODO_CIENCIA (Science Mode)
É o estado nominal de operação, onde o satélite cumpre sua missão principal.
- **Comportamento:** Os sensores externos são ativados (`LIGADO`). O sistema simula a coleta de dados ambientais (Pressão, Temperatura Externa e Radiação) e os armazena no pacote de dados.
- **Condição de Ativação:** Operação padrão quando a bateria está estável e não há requisição de transmissão.

##### 2. 📡 MODO_TRANSMISSAO (Transmission Mode)
Estado dedicado ao *downlink* (envio de dados para a Terra).
- **Comportamento:** A coleta de novos dados científicos é pausada (sensores em `standby` ou desligados) para priorizar o consumo energético e o processamento para o rádio transmissor.
- **Detalhe Técnico:** O status de operação muda para `DESLIGADO_TRANSMISSAO` para evitar corrupção de dados durante o envio. Este modo, junto com o `MODO_CIENCIA`, são ativamos via comando da cabine de controle em terra. Com isso, o operador pode decidir o ponto de operação e ativar os sensores externos somente quando nescessário.

##### 3. 🛡️ MODO_SAFE (Safety Mode)
Estado crítico de sobrevivência do satélite.
- **Comportamento:** Desligamento forçado de todos os periféricos não essenciais. O sistema ignora a coleta de dados e foca na manutenção da telemetria vital (bateria e temperatura interna).
- **Condição de Ativação:** Acionado automaticamente (interrupção por software) quando a tensão da bateria cai para níveis críticos (**≤ 1000mV**), prevenindo o *blackout* total do sistema. Além disso, neste modo há um corte total de comandos com o centro de operação. Neste modo, o operador não pode tirar o sistema do `MODO_SAFE` através de comandos enviados na sala de controle.

<img width="399" height="261" alt="image" src="https://github.com/user-attachments/assets/52a70357-7c81-4b4e-9934-cfc492e57470" /> <img width="399" height="261" alt="image" src="https://github.com/user-attachments/assets/c8c59037-a1dc-4fe9-8746-ab01a2ec3f20" />

