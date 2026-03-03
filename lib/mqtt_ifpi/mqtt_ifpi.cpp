#include "mqtt_ifpi.h"

// ------------------------------------------------------------------
// CONSTRUTOR: Vincula o cliente MQTT à placa de rede Wi-Fi do ESP32
// ------------------------------------------------------------------
MqttIFPI::MqttIFPI() {
    client.setClient(espClient); 
}

// ------------------------------------------------------------------
// FUNÇÃO INICIAR: Define qual é o endereço do "cérebro" (Mosquitto)
// ------------------------------------------------------------------
void MqttIFPI::iniciar(const char* ip) {
    broker_ip = ip;
    // Diz ao MQTT: "O seu servidor alvo é este IP, e ele funciona na porta 1883"
    client.setServer(broker_ip, 1883); 
}

// ------------------------------------------------------------------
// MANTER CONEXÃO: Função vital. Deve ser chamada repetidamente no loop()
// ------------------------------------------------------------------
void MqttIFPI::manterConexao() {
    static unsigned long ultimaTentativa = 0;

    if (!client.connected()) {
        unsigned long agora = millis();
        
        // Tenta conectar apenas a cada 5 segundos, sem usar 'delay' ou 'while'
        if (agora - ultimaTentativa > 5000) {
            ultimaTentativa = agora;
            Serial.print("A ligar ao servidor MQTT (Mosquitto)... ");
            
            if (client.connect("ESP32_Guarita_IFPI")) {
                Serial.println("Ligado com sucesso!");
            } else {
                Serial.print("Falhou, erro: ");
                Serial.println(client.state());
            }
        }
    } else {
        client.loop(); // Só processa o loop se estiver conectado
    }
}

// ------------------------------------------------------------------
// PUBLICAR TAG: Função que usaremos quando o RFID ler uma chave
// ------------------------------------------------------------------
void MqttIFPI::publicarTag(String tag) {
    // Só tenta enviar a mensagem se a conexão com o PC estiver ativa
    if (client.connected()) {
        // Envia a 'tag' para o tópico (canal) "guarita/rfid/tag"
        // .c_str() converte a String do Arduino para um texto padrão da linguagem C que o MQTT exige
        client.publish("guarita/rfid/tag", tag.c_str());
    }
}