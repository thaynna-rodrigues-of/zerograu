#include <EEPROM.h>         // Biblioteca para ler e gravar dados que não somem ao desligar o Arduino
#include <LiquidCrystal.h>  // Biblioteca para controle e comunicação com o display LCD

// Inicializa o display LCD definindo os pinos de controle e dados (RS, Enable, D4, D5, D6, D7)
// Nota: Certifique-se de que a ordem declarada aqui condiz exatamente com a pinagem física do circuito
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2); 

// --- Definição dos Pinos dos Botões ---
int bup = 7;        // Botão para aumentar a temperatura alvo (Setpoint)
int bdown = 6;      // Botão para diminuir a temperatura alvo (Setpoint)
int buph = 8;       // Botão para LIGAR o sistema (Power On)
int bdownh = 9;     // Botão para DESLIGAR o sistema / Standby (Power Off)

// --- Definição dos Pinos de Atuador e Sensor ---
int compressor = 13;   // Pino que simula o motor do freezer (controla o LED integrado da placa)
int temp_sensor = A0;  // Entrada analógica onde o sensor de temperatura está conectado

// --- Variáveis de Controle de Temperatura ---
int8_t setpoint = -5;  // Temperatura desejada no freezer (armazenado em 8 bits para suportar valores negativos)
int histerese = 3;     // Margem de tolerância para evitar que o motor ligue e desligue sem parar
float temp = 0;        // Variável que guarda a temperatura atual medida pelo sensor

bool sistemaLigado = true; // Flag que controla se o freezer está ativo ou em modo Standby

// --- Variáveis para Debounce dos Botões ---
// Guardam o estado anterior de cada botão para detectar apenas o momento exato do clique (borda de descida)
int last_bup = HIGH;
int last_bdown = HIGH;
int last_buph = HIGH;
int last_bdownh = HIGH;

void setup(){
  // Configura os pinos dos botões como entrada digital
  pinMode(bup, INPUT);
  pinMode(bdown, INPUT);
  pinMode(buph, INPUT);
  pinMode(bdownh, INPUT);
  
  pinMode(compressor, OUTPUT); // Configura o pino do compressor como saída digital
  
  digitalWrite(compressor, HIGH); // Inicia o compressor ligado por segurança
  lcd.begin(16, 2);               // Inicializa o LCD configurando o tamanho (16 colunas x 2 linhas)
  lcd.display();                  // Liga a exibição de texto do display
  
  // Recupera as configurações salvas na memória EEPROM para manter o histórico pós-queda de energia
  setpoint = (int8_t)EEPROM.read(0); // Lê o setpoint no endereço 0
  histerese = EEPROM.read(1);        // Lê a histerese no endereço 1
  
  // Filtro de segurança: Se a memória estiver vazia ou resetada (valores padrão 255 ou -1), define os valores iniciais estáveis
  if(setpoint == -1 || setpoint == 127 || setpoint == 255) setpoint = -5;
  if(histerese == -1 || histerese == 255) histerese = 3;
}

void loop(){
  // Realiza a leitura digital atual do estado de todos os botões
  int reading_up     = digitalRead(bup);
  int reading_down   = digitalRead(bdown);
  int reading_uph    = digitalRead(buph);
  int reading_downh  = digitalRead(bdownh);

  // --- Lógica do Botão de Desligar (Standby) ---
  // Se o botão foi pressionado agora (LOW) e antes estava solto (HIGH)
  if (reading_downh == LOW && last_bdownh == HIGH) {
    sistemaLigado = false;         // Atualiza o estado do sistema para desligado
    digitalWrite(compressor, LOW); // Garante o desligamento imediato do compressor/motor
    lcd.clear();                   // Limpa a tela para a nova mensagem
    delay(50);                     // Pequena pausa para debounce (evitar leituras falsas)
  }
  
  // --- Lógica do Botão de Ligar ---
  if (reading_uph == LOW && last_buph == HIGH) {
    sistemaLigado = true;          // Ativa o sistema novamente
    lcd.clear();
    delay(50);
  }

  // --- Modo Standby (Freezer Desligado) ---
  if (!sistemaLigado) {
    lcd.setCursor(0, 0);
    lcd.print("    FREEZER    ");
    lcd.setCursor(0, 1);
    lcd.print("  STATUS: OFF   ");
    
    // Atualiza o histórico dos botões para o próximo ciclo do loop
    last_bup = reading_up;
    last_bdown = reading_down;
    last_buph = reading_uph;
    last_bdownh = reading_downh;
    
    delay(150);
    return; // Interrompe a execução do loop aqui, ignorando a leitura do sensor e controle do motor
  }

  // --- Leitura e Conversão do Sensor de Temperatura ---
  float raw = analogRead(temp_sensor); // Lê o valor bruto do sensor (0 a 1023)
  float volt = raw * (5.0 / 1023.0);   // Converte o valor bruto para a tensão correspondente (0V a 5V)
  temp = (volt - 0.5) * 100.0;         // Converte a tensão em graus Celsius (Fórmula específica do sensor TMP36)

  // --- Ajuste Manual do Setpoint (Aumentar) ---
  if(reading_up == LOW && last_bup == HIGH){
    setpoint++; 
    EEPROM.write(0, setpoint); // Grava permanentemente o novo valor na posição 0 da EEPROM
    delay(50);
  }
  
  // --- Ajuste Manual do Setpoint (Diminuir) ---
  if(reading_down == LOW && last_bdown == HIGH){
    setpoint--; 
    EEPROM.write(0, setpoint); // Grava permanentemente o novo valor na posição 0 da EEPROM
    delay(50);
  }

  // Atualiza o histórico do estado dos botões para a próxima verificação
  last_bup = reading_up;
  last_bdown = reading_down;
  last_buph = reading_uph;
  last_bdownh = reading_downh;

  // --- Lógica de Controle com Histerese (Termostato) ---
  // Se a temperatura subir e atingir o limite máximo tolerado (Meta + Histerese), liga a refrigeração
  if (temp >= (setpoint + histerese)) {
    digitalWrite(compressor, HIGH); 
  } 
  // Se a refrigeração atingir a temperatura exata da Meta, desliga o compressor para poupar energia
  else if (temp <= setpoint) {
    digitalWrite(compressor, LOW); 
  }

  // --- Atualização das Informações no Display LCD ---
  // Linha 1: Exibe a temperatura atual em tempo real
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temp, 1);   // Mostra a temperatura com apenas 1 casa decimal
  lcd.print((char)176); // Imprime o símbolo de grau (°)
  lcd.print("C     ");  // Espaços em branco no fim limpam resquícios de caracteres antigos na tela

  // Linha 2: Exibe a temperatura alvo definida pelo usuário
  lcd.setCursor(0, 1);
  lcd.print("Meta:");
  lcd.print(setpoint);
  lcd.print((char)176);
  lcd.print("C ");

  // Canto Inferior Direito: Exibe o status atual de funcionamento do motor
  lcd.setCursor(11, 1);
  if (digitalRead(compressor) == HIGH) {
    lcd.print("[REF]");  // Indica que o compressor está ligado refrigerando
  } else {
    lcd.print("[OK] ");  // Indica que a temperatura está controlada e o motor descansando
  }

  delay(150); // Taxa de atualização do ciclo de monitoramento
}