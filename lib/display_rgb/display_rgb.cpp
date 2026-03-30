// #include "display_rgb.h"
// #include "buzzer_leds.h"
// // Controle de Interface


// // --- CRIAÇÃO FÍSICA DO OBJETO ---
// // É aqui que o compilador reserva o espaço na memória para a tela
// TFT_eSPI tft = TFT_eSPI();

// unsigned long timerAlerta = 0;
// bool displayingAlert = false;

// // --- FUNÇÕES DE INTERFACE ---

// void desenharNavbar() {
//   tft.fillRect(0, 0, 480, 50, tft.color565(40, 40, 40)); 
//   tft.setTextColor(TFT_WHITE, tft.color565(40, 40, 40));
//   tft.setTextSize(2);
//   tft.drawCentreString("SISTEMA DE CHAVES", 240, 15, 2);
//   tft.drawLine(0, 50, 480, 50, TFT_DARKGREY);
// }

// void limparCentro() {
//   tft.fillRect(0, 51, 480, 269, TFT_BLACK);
//   tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
//   tft.setTextSize(2);
//   tft.drawCentreString("APROXIME O CARTAO", 240, 160, 1);
//   displayingAlert = false;
// }

// void alertaSucesso(String nome, String setor) {
//   // --- VISUAL (TFT) ---
//   tft.fillRect(0, 51, 480, 269, TFT_BLACK);
//   tft.fillRoundRect(40, 80, 400, 140, 10, TFT_GREEN);
//   tft.setTextColor(TFT_BLACK, TFT_GREEN);
//   tft.drawCentreString("SUCESSO!", 240, 100, 1);
//   tft.setTextSize(2);
//   tft.drawCentreString("Setor: " + setor, 240, 145, 1);

//   // --- FEEDBACK (LED/BUZZER) ---
//   sinalizarSucesso(); // Acende verde e bipa rápido
  
//   timerAlerta = millis();
//   displayingAlert = true;
// }

// void feedbackErro(bool tempoEsgotado) {
//   // --- VISUAL (TFT) ---
//   tft.fillRect(0, 51, 480, 269, TFT_BLACK);
//   tft.fillRoundRect(40, 100, 400, 100, 10, TFT_RED);
//   tft.setTextColor(TFT_WHITE, TFT_RED);
  
//   if(tempoEsgotado) {
//     tft.drawCentreString("TEMPO ESGOTADO", 240, 125, 1);
//     tft.drawCentreString("Repita o processo", 240, 165, 1);
//   } else {
//     tft.drawCentreString("ERRO DE LEITURA", 240, 135, 1);
//   }
  
//   // --- FEEDBACK (LED/BUZZER) ---
//   sinalizarErro(); // Acende vermelho e bipa longo
  
//   timerAlerta = millis();
//   displayingAlert = true;
// }

// void feedbackInicioGravacao(int segundos) {
//   // --- VISUAL (TFT) ---
//   tft.fillRect(0, 51, 480, 269, TFT_BLACK);
//   tft.fillRoundRect(40, 90, 400, 120, 10, TFT_ORANGE);
//   tft.setTextColor(TFT_WHITE, TFT_ORANGE);
//   tft.drawCentreString("MODO GRAVACAO", 240, 105, 1);
//   tft.drawCentreString("Aproxime em: " + String(segundos) + "s", 240, 155, 1);
  
//   // --- FEEDBACK (LED/BUZZER) ---
//   // Apenas garante que o LED amarelo está aceso durante a espera
  
//   // No início da gravação, podemos dar um bip de atenção
//   if(segundos == 10) { 
//      sinalizarAtencao(); // Acende amarelo e bipa curto 
//   }
// }

#include "display_rgb.h"
#include "buzzer_leds.h" // Integra os bipes com as telas!

TFT_eSPI tft = TFT_eSPI();
unsigned long timerAlerta = 0;
bool displayingAlert = false;

// ==========================================
// FUNÇÕES BASE E DE CONTROLE
// ==========================================
void init_Display() {
    tft.init(); 
    tft.setRotation(1); 
    tft.fillScreen(TFT_BLACK); 
    desenharNavbar(); 
    limparCentro();
}

void gerenciarTimerTela() {
    if (displayingAlert && (millis() - timerAlerta > 4000)) {
        limparCentro(); 
    }
}

void desenharNavbar() {
    uint16_t corFundo = tft.color565(40, 40, 40);
    tft.fillRect(0, 0, 480, 50, corFundo); 
    tft.setTextColor(TFT_WHITE, corFundo);          
    tft.setTextSize(3); 
    tft.drawCentreString("GUARITA IFPI", 240, 15, 1); 
    tft.drawLine(0, 50, 480, 50, TFT_DARKGREY); 
}

void limparCentro() {
    tft.fillRect(0, 51, 480, 269, TFT_BLACK);
    
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.drawCentreString("APROXIME A TAG", 240, 110, 1);
    
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawCentreString("0 - Verificar/Cadastrar Chave", 240, 240, 1);
    
    displayingAlert = false;
}

// ==========================================
// TELAS DE INTERAÇÃO (TECLADO E AVISOS)
// ==========================================
void avisoCadastreWeb() {
    tft.fillRect(0, 51, 480, 269, TFT_BLACK);
    tft.fillRoundRect(40, 100, 400, 100, 10, TFT_ORANGE);
    
    tft.setTextColor(TFT_WHITE, TFT_ORANGE);
    tft.setTextSize(3);
    tft.drawCentreString("CHAVE NAO VINCULADA", 240, 120, 1);
    
    tft.setTextSize(2);
    tft.drawCentreString("Cadastre a chave no sistema Web", 240, 160, 1);
    
    sinalizarErro(); // Toca bip de erro!
    timerAlerta = millis();
    displayingAlert = true; 
}

void telaTecladoDinamico(String tituloOQueDigitar, String digitosAtuais) {
    tft.fillRect(0, 51, 480, 269, TFT_BLACK);
    
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawCentreString(tituloOQueDigitar, 240, 90, 1);
    
    // Caixa de Input
    tft.fillRect(100, 130, 280, 50, TFT_DARKGREY); 
    tft.setTextColor(TFT_YELLOW, TFT_DARKGREY); 
    tft.setTextSize(3);
    
    if(digitosAtuais == "") {
        tft.drawCentreString("----", 240, 145, 1);
    } else {
        tft.drawCentreString(digitosAtuais, 240, 145, 1);
    }

    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawCentreString("[Pressione ENTER no teclado]", 240, 220, 1);
    
    displayingAlert = false; 
}

// ==========================================
// TELAS DE FEEDBACK E GRAVAÇÃO
// ==========================================
void alertaSucesso(String mensagem, String setor) {
    tft.fillRect(0, 51, 480, 269, TFT_BLACK);
    tft.fillRoundRect(40, 80, 400, 140, 10, TFT_DARKGREEN);
    
    tft.setTextColor(TFT_WHITE, TFT_DARKGREEN);
    tft.setTextSize(3);
    
    // AQUI ESTÁ A MÁGICA: O título gigante agora é dinâmico!
    tft.drawCentreString(mensagem, 240, 100, 1); 
    
    tft.setTextSize(2);
    tft.drawCentreString("Setor: " + setor, 240, 145, 1);
    
    sinalizarSucesso(); // Toca bip de sucesso!
    timerAlerta = millis();
    displayingAlert = true;
}

void alertarPermissaoSucesso(String nome, String setor) {
    tft.fillRect(0, 51, 480, 269, TFT_BLACK);
    tft.fillRoundRect(40, 80, 400, 140, 10, TFT_DARKGREEN);
    
    tft.setTextColor(TFT_WHITE, TFT_DARKGREEN);
    tft.setTextSize(3);
    tft.drawCentreString("ACESSO LIBERADO", 240, 100, 1);
    
    tft.setTextSize(2);
    tft.drawCentreString("Setor: " + setor, 240, 145, 1);
    tft.drawCentreString("Usuario: " + nome, 240, 175, 1);

    sinalizarSucesso(); // Toca bip de sucesso!
    timerAlerta = millis();
    displayingAlert = true;
}

void alertarPermissaoErro(String nome, String motivo) {
    tft.fillRect(0, 51, 480, 269, TFT_BLACK);
    tft.fillRoundRect(40, 80, 400, 140, 10, TFT_RED);
    
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.setTextSize(3);
    tft.drawCentreString("ACESSO NEGADO!", 240, 100, 1);
    
    tft.setTextSize(2);
    tft.drawCentreString("Usuario: " + nome, 240, 145, 1);
    tft.drawCentreString("Motivo: " + motivo, 240, 175, 1);
    
    sinalizarErro(); // Toca bip de erro!
    timerAlerta = millis();
    displayingAlert = true;
}

void feedbackErro(bool tempoEsgotado) {
    tft.fillRect(0, 51, 480, 269, TFT_BLACK);
    tft.fillRoundRect(40, 100, 400, 100, 10, TFT_RED);
    
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.setTextSize(3);
    
    if(tempoEsgotado) {
        tft.drawCentreString("TEMPO ESGOTADO", 240, 120, 1);
        tft.setTextSize(2);
        tft.drawCentreString("Repita o processo", 240, 160, 1);
    } else {
        tft.drawCentreString("ERRO DE LEITURA", 240, 135, 1);
    }
    
    sinalizarErro(); // Toca bip de erro!
    timerAlerta = millis();
    displayingAlert = true;
}

void feedbackInicioGravacao(int segundos) {
    tft.fillRect(0, 51, 480, 269, TFT_ORANGE);
    
    tft.setTextColor(TFT_WHITE, TFT_ORANGE);
    tft.setTextSize(3);
    tft.drawCentreString("APROXIME PARA GRAVAR", 240, 70, 1);
    
    tft.setTextSize(8); 
    tft.drawCentreString(String(segundos), 240, 140, 1);
    
    tft.setTextSize(2);
    tft.drawCentreString("Segundos restantes", 240, 260, 1);
    
    if(segundos == 10) { 
        sinalizarAtencao(); // Toca bip de alerta apenas no segundo 10
    }
    displayingAlert = false; 
}
