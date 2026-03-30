#include <Arduino.h>
#include <LittleFS.h>

// 1. Inclusão dos Módulos
#include "rfid_ifpi.h"
#include "buzzer_leds.h"
#include "wifi_ifpi.h"
#include "mqtt_ifpi.h"
#include "display_rgb.h"
#include "teclado_matricial.h"

// Objetos de Rede
WifiIFPI redeWifi;
MqttIFPI redeMqtt;

// ==========================================
// VARIÁVEIS DA MÁQUINA DE ESTADOS
// ==========================================
enum EstadoSistema {
  ESPERANDO_ACAO,
  DIGITANDO_SENHA,
  VERIFICANDO_CADASTRO // O NOVO ESTADO!
};

EstadoSistema estadoAtual = ESPERANDO_ACAO;

String senhaDigitada = "";
const String senhaCorreta = "12345678"; 
String setorTagLida = ""; 

// ==========================================
// SETUP
// ==========================================
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    init_Componentes(); 
    init_Teclado(); 
    init_Display(); 
    delay(500); 

    if(!LittleFS.begin(true)){ 
        Serial.println("Erro crítico ao montar LittleFS");
    }
    redeWifi.iniciar(); 
    redeMqtt.iniciar(redeWifi.getMqttServerIp()); 
    delay(500); 

    init_RFID(); 
    Serial.println("=== SISTEMA GUARITA IFPI INICIADO ===");
}

// ==========================================
// LOOP PRINCIPAL
// ==========================================
void loop() {
    // 1. GERENCIAMENTO DE SISTEMAS DE FUNDO
    if (displayingAlert && (millis() - timerAlerta > 5000)) {
        limparCentro();
        digitalWrite(LED_VERDE_PIN, LOW);
        digitalWrite(LED_VERMELHO_PIN, LOW);
        digitalWrite(LED_AMARELO_PIN, LOW);
    }
    redeMqtt.manterConexao();

    // 2. LEITURA DOS SENSORES FÍSICOS
    char key = lerTecla(); 
    bool tagPresente = verificarTagPresente(); 

    // 3. O FLUXO DA MÁQUINA DE ESTADOS
    switch (estadoAtual) {
        
        // ---------------------------------------------------
        case ESPERANDO_ACAO:
            if (tagPresente) {
                setorTagLida = lerNomeDoSetor(2); 

                if (setorTagLida == "" || setorTagLida == "Vazio" || setorTagLida == "ERRO") {
                    avisoCadastreWeb(); 
                } else {
                    estadoAtual = DIGITANDO_SENHA;
                    senhaDigitada = ""; 
                    telaTecladoDinamico("Setor: " + setorTagLida, senhaDigitada);
                    sinalizarAtencao(); 
                }
            }
            // Se apertar o '0', inicia a verificação
            else if (key == '0') {
                estadoAtual = VERIFICANDO_CADASTRO;
                
                tft.fillRect(0, 51, 480, 269, TFT_BLACK);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.setTextSize(3);
                tft.drawCentreString("VERIFICAR CHAVE", 240, 110, 1);
                tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
                tft.setTextSize(2);
                tft.drawCentreString("Aproxime a tag para ler", 240, 160, 1);
                tft.drawCentreString("[ * para Cancelar ]", 240, 240, 1);
                sinalizarAtencao(); 
            }
            break;

        // ---------------------------------------------------
        case DIGITANDO_SENHA:
            if (key) {
                if (key >= '0' && key <= '9' && senhaDigitada.length() < 8) {
                    senhaDigitada += key;
                    telaTecladoDinamico("Setor: " + setorTagLida, senhaDigitada);
                } 
                else if (key == '*') { 
                    if (senhaDigitada.length() > 0) {
                        senhaDigitada.remove(senhaDigitada.length() - 1);
                        telaTecladoDinamico("Setor: " + setorTagLida, senhaDigitada);
                    }
                } 
                else if (key == '#') { 
                    if (senhaDigitada == senhaCorreta) {
                        alertarPermissaoSucesso("Usuario", setorTagLida);
                        redeMqtt.publicarTag("Acesso Liberado no Setor: " + setorTagLida); 
                    } else {
                        alertarPermissaoErro("Desconhecido", "Senha Incorreta");
                    }
                    estadoAtual = ESPERANDO_ACAO; 
                }
            }
            break;

        // ---------------------------------------------------
        case VERIFICANDO_CADASTRO:
            
            // O usuário pode apertar * para cancelar a verificação
            if (key == '*') {
                estadoAtual = ESPERANDO_ACAO;
                limparCentro();
            }
            
            // O usuário encostou uma Tag:
            else if (tagPresente) {
                setorTagLida = lerNomeDoSetor(2);

                if (setorTagLida == "" || setorTagLida == "Vazio" || setorTagLida == "ERRO") {
                    // TAG VIRGEM! 
                    tft.fillRect(0, 51, 480, 269, TFT_BLACK);
                    tft.fillRoundRect(40, 100, 400, 100, 10, TFT_ORANGE);
                    tft.setTextColor(TFT_WHITE, TFT_ORANGE);
                    tft.setTextSize(3);
                    tft.drawCentreString("TAG VIRGEM", 240, 130, 1);
                    sinalizarErro(); 
                    
                    delay(2000); 

                    // CHAMA A GRAVAÇÃO PELO SERIAL
                    gravarSetor(); 

                    estadoAtual = ESPERANDO_ACAO;
                    limparCentro();
                } 
                else {
                    // TAG JÁ CADASTRADA!
                    alertaSucesso("Chave ja Cadastrada", setorTagLida);
                    estadoAtual = ESPERANDO_ACAO;
                }
            }
            break;
    }
}