#include "buzzer_leds.h"

void init_Componentes() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_VERDE_PIN, OUTPUT);
  pinMode(LED_VERMELHO_PIN, OUTPUT);
  pinMode(LED_AMARELO_PIN, OUTPUT);

  // Garantir que tudo comece desligado
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_VERDE_PIN, LOW);
  digitalWrite(LED_VERMELHO_PIN, LOW);
  digitalWrite(LED_AMARELO_PIN, LOW);
}

void sinalizarSucesso() {
  digitalWrite(LED_VERDE_PIN, HIGH); // Liga LED verde
  tone(BUZZER_PIN, 2000); // Som Agudo com onda de 2000 Hz
  delay(200); // Duração do som
  noTone(BUZZER_PIN); // Para o som
  digitalWrite(LED_VERDE_PIN, LOW); // Desliga LED verde
}

void sinalizarErro() {
  digitalWrite(LED_VERMELHO_PIN, HIGH);
  tone(BUZZER_PIN, 400); // Som Grave com onda de 400 Hz
  delay(800); // Duração do som
  noTone(BUZZER_PIN);  // Para o som
  digitalWrite(LED_VERMELHO_PIN, LOW); // Desliga LED vermelho
}

void sinalizarAtencao() {
  // Exemplo de bips duplos com LED amarelo
  for(int i=0; i<3; i++) {
    digitalWrite(LED_AMARELO_PIN, HIGH);
    tone(BUZZER_PIN, 1200); 
    delay(250);
    noTone(BUZZER_PIN);
    digitalWrite(LED_AMARELO_PIN, LOW);
    delay(100);
  }
}