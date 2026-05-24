# 🧊 Sensor de Temperatura para Freezer

Este projeto consiste em um sistema automatizado de controle e monitoramento de temperatura para freezers utilizando a plataforma Arduino. O sistema monitora a temperatura em tempo real, gerencia o acionamento do compressor baseado em uma meta programável (com histerese) e salva as configurações na memória não-volátil (EEPROM) para evitar perda de dados em quedas de energia.

## 🚀 Funcionalidades
*   **Monitoramento em Tempo Real:** Exibição da temperatura atual e da meta configurada em um display LCD 16x2.
*   **Controle de Refrigeração Automático:** Acionamento inteligente do compressor baseado no cálculo de histerese.
*   **Ajuste Manual de Setpoint:** Botões dedicados para aumentar ou diminuir a temperatura desejada.
*   **Modo Eco / Standby:** Botões para ligar e desligar o sistema de refrigeração via software.
*   **Persistência de Dados:** Salvamento automático das configurações na memória EEPROM do Arduino.

## 🛠️ Componentes Utilizados
*   1x Arduino UNO
*   1x Display LCD 16x2
*   1x Sensor de Temperatura (TMP36)
*   4x Botões (Push-buttons)
*   Resistores e Protoboards
*   Fiação de Conexão

## 🎛️ Interface e Controle
*   **Botão Up:** Aumenta o setpoint da temperatura.
*   **Botão Down:** Diminui o setpoint da temperatura.
*   **Botão Power On:** Ativa o sistema de refrigeração e o monitoramento.
*   **Botão Power Off:** Desativa o compressor e coloca o freezer em modo Standby.

---

## 💻 Estrutura do Código

O código foi desenvolvido na IDE do Arduino e utiliza as seguintes bibliotecas nativas:
*   `LiquidCrystal.h`: Controle do display LCD.
*   `EEPROM.h`: Gravação de dados permanentes.

### Lógica de Histerese Implementada
Para evitar o desgaste do motor por acionamentos repetitivos em variações curtas de temperatura, o sistema opera sob a seguinte regra:
*   Se $Temp \ge Setpoint + Histerese$ ➡️ **Liga o Compressor `[REF]`**
*   Se $Temp \le Setpoint$ ➡️ **Desliga o Compressor `[OK]`**

---

## 🔧 Como Executar o Projeto

1. Monte o circuito seguindo o esquemático elétrico do projeto.
2. Abra o arquivo do código na **Arduino IDE**.
3. Conecte o Arduino Uno ao computador via USB.
4. Selecione a placa `Arduino Uno` e a porta COM correspondente em *Ferramentas*.
5. Clique em **Carregar (Upload)**.

## 👥 Integrantes do Grupo
*   Thaynna
*   Davi
*   Geilson

---
🎨 *Projeto desenvolvido e simulado utilizando a plataforma Tinkercad.*

<p align="center">
  <img src="Circuito/ZeroGrau.png" alt="Circuito do Projeto" width="600">
</p>