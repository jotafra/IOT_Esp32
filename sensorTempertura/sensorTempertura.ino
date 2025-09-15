#include <math.h>
#include "AdafruitIO_WiFi.h"
#include "NewPing.h"

//Configuração da rede WIFI
#define WIFI_SSID "maju" 
#define WIFI_PASS "maju0812"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

#define pinNTC 34 // pino do sensor de temperatura
#define pinLed 14 // pino do led

#define BUZZER_PIN 27
#define LED_ALARME 13
#define BOTAO_FISICO 26
#define TRIG_PIN 12
#define ECHO_PIN 14 

#define WIFI_SSID ""
#define WIFI_PASS ""

// Configuração do ultrassônico 
#define MAX_DISTANCE 100
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// Controle de envio de dados 
float temp_atual = 0;
float temp_anterior = -1;

//Variavel / ponteiro para referenciar o feed temperatura
AdafruitIO_Feed *temperatura = io.feed("Temperatura");
AdafruitIO_Feed *ledFeed = io.feed("botaoled");
AdafruitIO_Feed *botaoAlarme = io.feed("botaoalarme");

const float Rfixo = 10000.0; // Resistor do projeto 
const float Beta = 3950.0; 
const float R0 = 10000.0; // nominal do NTC
const float T0_kelvin = 298.15; // 25°C em Kelvin
const float Vcc = 3.3;

// Variáveis de controle 
bool alarmeAtivo = false;
unsigned int distancia = 0;
const int LIMITE_DISTANCIA = 15;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_ALARME, OUTPUT);
  pinMode(BOTAO_FISICO, INPUT);

  pinMode(pinNTC, INPUT);
  pinMode(pinLed, OUTPUT);
  Serial.begin(115200);

  while(!Serial);
  Serial.print("Conectando ao adafruit IO");
  io.connect();

  while(io.status() < AIO_CONNECTED){
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());

  // Configuração do callback, quando o feed receber(atualizar) um valor
  // temperatura -> onMessage(handleTemperatura);
  botaoAlarme -> onMessage(handleAlarme);

  Serial.println("Solicitando o estado inicial do alarme: ");
  botaoAlarme -> get();

  ledFeed -> onMessage(handleBotaoLed);
  //registra a função de callback 
  // ela será chamada sempre que o feed receber um novo dado
  delay(1000);
}

void loop() {
  io.run();
  delay(500);

  // Leitura do botão físico
  if (digitalRead(BOTAO_FISICO) == 1){
    delay(200); // debounce simples 
    alarmeAtivo = !alarmeAtivo;

    botaoAlarme -> save(String(alarmeAtivo ? "true" : "false"));
    Serial.println(alarmeAtivo ? F("Alarme ARMADO pelo botao fisico.") : F("Alarme DESARMADO pelo botao fisico."));
  }
  distancia = sonar.ping_cm();
  Serial.print("Distancia lida: ");
  Serial.println(distancia);
  Serial.print(" cm");

  // Ativação ou desativação do alarme
  if(alarmeAtivo && distancia > 0 && distancia < LIMITE_DISTANCIA){
    ativarAlerta();
  } else {
    desligarAlerta();
  }


}