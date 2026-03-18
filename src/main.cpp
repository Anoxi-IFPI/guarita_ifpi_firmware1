#include <Arduino.h>
#include <LittleFS.h>

// 1. Inclusão dos Módulos
#include "rfid_ifpi.h"
#include "buzzer_leds.h"
#include "wifi_ifpi.h"
#include "mqtt_ifpi.h"
#include "display_rgb.h"

// Objetos de Rede
WifiIFPI redeWifi;
MqttIFPI redeMqtt;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // --- 1. HARDWARE BÁSICO ---
    init_Componentes(); // Buzzer e LEDs
    
    // --- 2. INICIALIZAÇÃO DO DISPLAY (LIGA PRIMEIRO) ---
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    desenharNavbar();
    limparCentro();
    
    delay(500); // Dá um tempo para a tela estabilizar a energia

    // --- 3. REDE E SISTEMA ---
    if(!LittleFS.begin(true)){ 
        Serial.println("Erro crítico ao montar LittleFS");
    }
    redeWifi.iniciar(); 
    redeMqtt.iniciar(redeWifi.getMqttServerIp()); 

    delay(500); // Dá um tempo pro Wi-Fi parar de puxar pico de corrente

    // --- 4. INICIALIZAÇÃO DO RFID (LIGA POR ÚLTIMO) ---
    init_RFID(); // Agora sim, com a energia e os barramentos calmos, o RFID acorda!

    exibirMenu();
}

void loop() {
    // A. GERENCIADOR DE TEMPO DO DISPLAY (Substitui os delays bloqueantes)
    // Se um alerta estiver na tela e passar 5 segundos, ele limpa sozinho
    if (displayingAlert && (millis() - timerAlerta > 5000)) {
        limparCentro();
        // Garante que LEDs de status apaguem quando a tela volta ao normal
        digitalWrite(LED_VERDE_PIN, LOW);
        digitalWrite(LED_VERMELHO_PIN, LOW);
        digitalWrite(LED_AMARELO_PIN, LOW);
    }

    // B. MANTER CONEXÃO MQTT (Mosquitto)
    redeMqtt.manterConexao();

    // C. COMANDOS SERIAL (Digite '0' para gravar)
    if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '0') {
            gravarSetor(); // A lógica de contagem e gravação roda aqui
        }
    }

    // D. LEITURA AUTOMÁTICA RFID
    if (verificarTagPresente()) {
        Serial.println("\n\n!!! TAG FÍSICA DETECTADA !!!\n");
        
        // Lê o bloco 2 e já chama a função de sucesso do display internamente
        lerNomeDoSetor(2); 
        
        // Publica o aviso no servidor Mosquitto
        redeMqtt.publicarTag("Acesso Registrado na Guarita!"); 
    }
}