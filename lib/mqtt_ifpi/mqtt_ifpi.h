#ifndef MQTT_IFPI_H
#define MQTT_IFPI_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h> // Biblioteca responsável pela comunicação MQTT

class MqttIFPI {
  private:
    WiFiClient espClient; // Objeto que gerencia a conexão de rede crua do ESP32
    PubSubClient client;  // Objeto MQTT que usa a conexão crua acima
    const char* broker_ip; // Variável para guardar o IP do computador que o Wi-Fi vai nos passar

  public:
    MqttIFPI(); // Construtor
    void iniciar(const char* ip); // Prepara o MQTT recebendo o IP
    void manterConexao();         // Fica verificando se a conexão caiu e tenta religar
    void publicarTag(String tag); // Envia o ID do cartão para o Mosquitto
};

#endif