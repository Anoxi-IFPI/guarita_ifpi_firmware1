#ifndef BUZZER_LEDS_H
#define BUZZER_LEDS_H

#include <Arduino.h>

// Definições de pinos para o buzzer e LEDs
#define BUZZER_PIN 13       // Feedback sonoro
#define LED_VERDE_PIN 4     // Acesso permitido
#define LED_VERMELHO_PIN 15 // Acesso negado
#define LED_AMARELO_PIN 21  // Atenção / Aguardando

// Protótipos das funções para controle do buzzer e LEDs

void init_Componentes();
void sinalizarSucesso();
void sinalizarErro();
void sinalizarAtencao();

#endif
