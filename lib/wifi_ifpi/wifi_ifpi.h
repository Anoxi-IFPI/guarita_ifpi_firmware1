// Estas duas linhas (ifndef e define) evitam que o compilador inclua este arquivo duas vezes, 
// o que causaria um erro de "redefinição". É uma boa prática padrão em C++.
#ifndef WIFI_IFPI_H
#define WIFI_IFPI_H

#include <Arduino.h>
#include <WiFiManager.h>      // Biblioteca que cria o portal para digitar a senha do Wi-Fi
#include <LittleFS.h>         // Biblioteca que lê os arquivos da memória interna do ESP32 (onde está o site)
#include <ESPAsyncWebServer.h> // Biblioteca do servidor web que vai rodar o site no ESP32

class WifiIFPI {
  // Tudo que é 'private' só pode ser acessado e modificado por dentro desta própria classe.
  // Protege os dados para que o arquivo main.cpp não bagunce nada sem querer.
  private:
    AsyncWebServer* server;       // Ponteiro para o servidor web
    char mqtt_server_ip[40];      // Variável que vai guardar o IP do seu computador (Mosquitto)

  // Tudo que é 'public' são as funções que o nosso main.cpp vai poder "chamar".
  public:
    WifiIFPI();                   // Construtor: função que roda automaticamente quando a classe é criada
    void iniciar();               // Função principal que vai ligar o Wi-Fi e o site
    char* getMqttServerIp();      // Função "entregadora" que devolve o IP para passarmos pro MQTT
};

#endif