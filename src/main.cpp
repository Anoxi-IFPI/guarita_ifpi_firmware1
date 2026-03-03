#include <Arduino.h>

// 1. Inclusão dos Módulos de Hardware
#include "rfid_ifpi.h"
#include "buzzer_leds.h"

// 2. Inclusão dos Módulos de Rede
#include "wifi_ifpi.h"
#include "mqtt_ifpi.h"

// Criação dos objetos de Rede
WifiIFPI redeWifi;
MqttIFPI redeMqtt;

void setup() {

    WiFi.mode(WIFI_STA); // Garante que está em modo estação antes de limpar
    WiFi.disconnect(true, true);
    delay(1000);
    
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- RESET DE FÁBRICA INICIADO ---");

    // LIMPEZA TOTAL (MUITO IMPORTANTE AGORA)
    WiFi.disconnect(true, true); 
    delay(2000); // Dá tempo para o chip processar a limpeza

    // Formata o LittleFS do zero para corrigir o erro de permissão
    if(!LittleFS.begin(true)){ 
        Serial.println("Erro crítico ao montar LittleFS");
    } else {
        Serial.println("LittleFS Formatado e Montado!");
    }

    init_Componentes(); 
    init_RFID(); 

    redeWifi.iniciar(); 
    redeMqtt.iniciar(redeWifi.getMqttServerIp()); 

    exibirMenu();
}

void loop() {
    // 1. Escuta comandos do teclado (Para gravar setor)
    if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '0') {
            gravarSetor(); // (Buzzer e LEDs já funcionam lá dentro)
        }
    }

    // 2. Mantém a conexão com o PC (Mosquitto) viva
    redeMqtt.manterConexao();

    // 3. Leitura automática do Cartão RFID
    if (verificarTagPresente()) {
        
        lerNomeDoSetor(2); // Lê os dados salvos no cartão
        sinalizarSucesso(); // Dá o Bip verde de sucesso
        
        // NOVIDADE: Envia um aviso para o Mosquitto no PC de que um cartão foi lido!
        // (Depois podemos alterar para enviar o UID ou o Setor exato)
        redeMqtt.publicarTag("Cartao Detectado na Guarita!"); 

        delay(1500);
        exibirMenu();
    }
}