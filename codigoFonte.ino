#include <DHT.h> // Biblioteca para o sensor DHT22
#include <LiquidCrystal_I2C.h> // Biblioteca para o LCD I2C
#include <RTClib.h> // Biblioteca para o RTC DS3231

RTC_DS3231 rtc; // Criação de um objeto RTC para manipular o DS3231

#define I2C_ADDR    0x27 // Endereço I2C do display LCD
#define LCD_COLUMNS 16 // Número de colunas do LCD
#define LCD_LINES   2 // Número de linhas do LCD

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES); // Criação do objeto LCD

DHT dht(2, DHT22); // Criação do objeto DHT, conectando ao pino 2 e usando o sensor DHT22

float temp; // Variável para armazenar a temperatura lida
int horaAtual = -1; // Inicialização da hora atual, usada para verificar mudanças de hora
int dataIndex = 0; // Índice para rastrear a posição no array de dados

// Estrutura para armazenar a temperatura e o horário
struct DataEntry {
  float temperature;
  String time;
};

DataEntry data[10]; // Array para armazenar até 10 entradas de dados

// Função para obter a data e hora formatada como uma string
String getData() {
  DateTime now = rtc.now(); // Obtém a hora atual do RTC
  char buffer[17]; // Buffer para armazenar a string formatada
  sprintf(buffer, "%02d/%02d - %02d:%02d", now.day(), now.month(), now.hour(), now.minute());
  return String(buffer); // Converte o buffer para String e retorna
}

// Função para armazenar a temperatura e o horário no array de dados
void sendData(float temp) {
  DateTime now = rtc.now(); // Obtém a hora atual do RTC
  if (horaAtual != now.hour() && dataIndex < 10) { // Verifica se a hora mudou e se ainda há espaço no array
    data[dataIndex].temperature = temp; // Armazena a temperatura no array
    data[dataIndex].time = getData(); // Armazena a hora formatada no array
    horaAtual = now.hour(); // Atualiza a hora atual
    dataIndex++; // Incrementa o índice do array
  }
}

// Função para imprimir todos os dados armazenados no array no Serial Monitor
void printAllData() {
  Serial.println("All recorded data:"); // Mensagem de cabeçalho
  for (int i = 0; i < dataIndex; i++) { // Itera sobre todas as entradas armazenadas
    Serial.print("Index ");
    Serial.print(i);
    Serial.print(": Temp = ");
    Serial.print(data[i].temperature);
    Serial.print(", Time = ");
    Serial.println(data[i].time);
  }
}

// Configuração inicial
void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial
  dht.begin(); // Inicializa o sensor DHT22
  lcd.init(); // Inicializa o LCD
  lcd.backlight(); // Liga a luz de fundo do LCD
  if (!rtc.begin()) { // Verifica se o RTC está presente
    Serial.println("Couldn't find RTC"); // Mensagem de erro se o RTC não for encontrado
    while (1); // Entra em loop infinito
  }
  if (rtc.lostPower()) { // Verifica se o RTC perdeu a energia e precisa ser ajustado
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Ajusta o RTC para a data e hora de compilação
  }
}

// Loop principal
void loop() {
  lcd.setCursor(0, 0); // Define o cursor do LCD na linha 0, coluna 0
  lcd.print(getData()); // Exibe a data e hora atual no LCD

  temp = dht.readTemperature(); // Lê a temperatura do sensor DHT22

  lcd.setCursor(0, 1); // Define o cursor do LCD na linha 1, coluna 0
  lcd.print("Temp: ");
  lcd.print(temp); // Exibe a temperatura no LCD
  lcd.print(" C  ");

  sendData(temp); // Armazena a temperatura e o horário no array de dados

  printAllData(); // Imprime todos os dados armazenados no Serial Monitor

  delay(2000); // Aguarda 2 segundos antes de repetir o loop
}
