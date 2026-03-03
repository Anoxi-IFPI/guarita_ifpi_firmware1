#include <Arduino.h>
#include "rfid_ifpi.h"
#include "buzzer_leds.h"

void setup() {
    Serial.begin(9600);
    init_Componentes(); 
    init_RFID(); 
    exibirMenu();
}

void loop() {
    // 1. Escuta comandos do teclado
    if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '0') {
            gravarSetor(); // Os sons já acontecem lá dentro (Amarelo, Verde ou Vermelho)
        }
    }

    // 2. Leitura automática
    if (verificarTagPresente()) {
        lerNomeDoSetor(2);
        sinalizarSucesso(); // Bip verde curto para indicar leitura OK
        delay(1500);
        exibirMenu();
    }
}