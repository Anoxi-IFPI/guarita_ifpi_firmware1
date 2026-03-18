#include "display_rgb.h"
#include "buzzer_leds.h"
// Controle de Interface


// --- CRIAÇÃO FÍSICA DO OBJETO ---
// É aqui que o compilador reserva o espaço na memória para a tela
TFT_eSPI tft = TFT_eSPI();

unsigned long timerAlerta = 0;
bool displayingAlert = false;

// --- FUNÇÕES DE INTERFACE ---

void desenharNavbar() {
  tft.fillRect(0, 0, 480, 50, tft.color565(40, 40, 40)); 
  tft.setTextColor(TFT_WHITE, tft.color565(40, 40, 40));
  tft.setTextSize(2);
  tft.drawCentreString("SISTEMA DE CHAVES", 240, 15, 2);
  tft.drawLine(0, 50, 480, 50, TFT_DARKGREY);
}

void limparCentro() {
  tft.fillRect(0, 51, 480, 269, TFT_BLACK);
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawCentreString("APROXIME O CARTAO", 240, 160, 1);
  displayingAlert = false;
}

void alertaSucesso(String nome, String setor) {
  // --- VISUAL (TFT) ---
  tft.fillRect(0, 51, 480, 269, TFT_BLACK);
  tft.fillRoundRect(40, 80, 400, 140, 10, TFT_GREEN);
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.drawCentreString("SUCESSO!", 240, 100, 1);
  tft.setTextSize(2);
  tft.drawCentreString("Chave: " + setor, 240, 145, 1);
  tft.drawCentreString("User: " + nome, 240, 175, 1);

  // --- FEEDBACK (LED/BUZZER) ---
  sinalizarSucesso(); // Acende verde e bipa rápido
  
  timerAlerta = millis();
  displayingAlert = true;
}

void feedbackErro(bool tempoEsgotado) {
  // --- VISUAL (TFT) ---
  tft.fillRect(0, 51, 480, 269, TFT_BLACK);
  tft.fillRoundRect(40, 100, 400, 100, 10, TFT_RED);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  
  if(tempoEsgotado) {
    tft.drawCentreString("TEMPO ESGOTADO", 240, 125, 1);
    tft.drawCentreString("Repita o processo", 240, 165, 1);
  } else {
    tft.drawCentreString("ERRO DE LEITURA", 240, 135, 1);
  }
  
  // --- FEEDBACK (LED/BUZZER) ---
  sinalizarErro(); // Acende vermelho e bipa longo
  
  timerAlerta = millis();
  displayingAlert = true;
}

void feedbackInicioGravacao(int segundos) {
  // --- VISUAL (TFT) ---
  tft.fillRect(0, 51, 480, 269, TFT_BLACK);
  tft.fillRoundRect(40, 90, 400, 120, 10, TFT_ORANGE);
  tft.setTextColor(TFT_WHITE, TFT_ORANGE);
  tft.drawCentreString("MODO GRAVACAO", 240, 105, 1);
  tft.drawCentreString("Aproxime em: " + String(segundos) + "s", 240, 155, 1);
  
  // --- FEEDBACK (LED/BUZZER) ---
  // Apenas garantimos que o LED amarelo está aceso durante a espera
  
  // No início da gravação, podemos dar um bip de atenção
  if(segundos == 10) { 
     sinalizarAtencao(); // Acende amarelo e bipa curto 
  }
}


