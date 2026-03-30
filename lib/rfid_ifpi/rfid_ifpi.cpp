#include "rfid_ifpi.h"
#include "buzzer_leds.h"
#include "display_rgb.h"

MFRC522 mfrc522(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key;

void init_RFID() {
    // 1. A INICIALIZAÇÃO SIMPLES (A que funciona!)
    SPI.begin(); 
    mfrc522.PCD_Init(); 

    // 2. Prepara a chave de autenticação padrão (0xFF)
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

    // 3. Verificação de Firmware
    byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
    Serial.print("RFID Firmware: 0x");
    Serial.println(v, HEX);
    
    if (v == 0x00 || v == 0xFF) {
        Serial.println("ERRO: Sensor não respondeu!");
    } else {
        Serial.println("RFID ONLINE (Sensor OK! Aguardando tag...)");
    }
}

// exibe o menu inicial de opções para o usuário
void exibirMenu() {
    Serial.println("\n[ AGUARDANDO TAG... ]");
    Serial.println("Comando: Digite '0' para gravar novo setor");
}

// verifica se uma nova tag foi aproximada e se a leitura do UID foi bem sucedida
bool verificarTagPresente() {
    return (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial());
}

// função para ler o nome do setor
// void lerNomeDoSetor(byte bloco) {
//     Serial.print("ID da Tag:");
//     for (byte i = 0; i < mfrc522.uid.size; i++) {
//         Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//         Serial.print(mfrc522.uid.uidByte[i], HEX);
//     }
//     Serial.println(); 

//     MFRC522::StatusCode status; 
//     byte buffer[18];
//     byte tamanho = 18;

//     status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &key, &(mfrc522.uid));
    
//     if (status != MFRC522::STATUS_OK) {
//         feedbackErro(false);
//         return;
//     }

//     status = mfrc522.MIFARE_Read(bloco, buffer, &tamanho);
//     if (status == MFRC522::STATUS_OK) {
//         if (buffer[0] == 0 || buffer[0] == ' ') {
//             Serial.println("AVISO: Tag sem cadastro!");
//             feedbackErro(false);
//         } else {
//             String nomeLido = "";
//             for (byte i = 0; i < 16; i++) {
//                 if (buffer[i] != 0 && buffer[i] != ' ') nomeLido += (char)buffer[i];
//             }
//             Serial.println("SETOR: " + nomeLido);
//             alertaSucesso("CONCLUIDO", nomeLido); 
//         }
//     } else {
//         feedbackErro(false);
//     }
//     mfrc522.PICC_HaltA();
//     mfrc522.PCD_StopCrypto1();
// }

String lerNomeDoSetor(byte bloco) {
    Serial.print("ID da Tag:");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println(); 

    MFRC522::StatusCode status; 
    byte buffer[18];
    byte tamanho = 18;

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &key, &(mfrc522.uid));
    
    if (status != MFRC522::STATUS_OK) {
        Serial.println("Erro de Autenticação na Tag.");
        return "ERRO"; // Devolve a palavra "ERRO" para o main
    }

    String resultado = ""; // Variável para armazenar o que vamos devolver

    status = mfrc522.MIFARE_Read(bloco, buffer, &tamanho);
    if (status == MFRC522::STATUS_OK) {
        if (buffer[0] == 0 || buffer[0] == ' ') {
            Serial.println("AVISO: Tag sem cadastro!");
            resultado = "Vazio"; // Indica que a tag está limpa
        } else {
            for (byte i = 0; i < 16; i++) {
                if (buffer[i] != 0 && buffer[i] != ' ') {
                    resultado += (char)buffer[i];
                }
            }
            Serial.println("SETOR LIDO: " + resultado);
        }
    } else {
        resultado = "ERRO";
    }

    // Encerra a comunicação com a Tag de forma segura
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    // Entrega o texto para a Máquina de Estados usar na tela!
    return resultado; 
}

// void gravarSetor() {
//     while(Serial.available() > 0) Serial.read(); 

//     Serial.println("\n[ MODO GRAVACAO ]");
//     feedbackInicioGravacao(10); 
    
//     Serial.println("Digite o nome do setor:");
//     while (Serial.available() == 0) { } 
    
//     delay(200); 
//     String nomeSetor = Serial.readStringUntil('\n');
//     nomeSetor.trim();

//     if (nomeSetor.length() == 0) {
//         feedbackErro(false); 
//         exibirMenu();
//         return;
//     }

//     Serial.println("Nome aceito: " + nomeSetor);
//     Serial.println("APROXIME A TAG AGORA (10s)...");

//     unsigned long inicio = millis();
//     int ultimoSegundo = 10;

//     while (millis() - inicio < 10000) {
//         int segundoAtual = 10 - (millis() - inicio) / 1000;
//         if (segundoAtual != ultimoSegundo) {
//             ultimoSegundo = segundoAtual;
//             feedbackInicioGravacao(ultimoSegundo);
//         }

//         if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
//             byte buffer[16];
//             for (byte i = 0; i < 16; i++) {
//                 buffer[i] = (i < nomeSetor.length()) ? nomeSetor[i] : ' ';
//             }

//             if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) { 
//                 if (mfrc522.MIFARE_Write(2, buffer, 16) == MFRC522::STATUS_OK) {
//                     Serial.println(">>> SUCESSO!");
//                     alertaSucesso("GRAVADO!", nomeSetor);
//                 } else {
//                     feedbackErro(false);
//                 }
//             }
//             mfrc522.PICC_HaltA();
//             mfrc522.PCD_StopCrypto1();
//             exibirMenu();
//             return;
//         }
//     }
    
//     Serial.println(">>> TEMPO ESGOTADO!");
//     feedbackErro(true);
//     exibirMenu();
// }

void gravarSetor() {
    while(Serial.available() > 0) Serial.read(); 

    Serial.println("\n[ MODO GRAVACAO ]");
    feedbackInicioGravacao(10); 
    
    Serial.println("Digite o nome do setor:");
    while (Serial.available() == 0) { } 
    
    delay(200); 
    String nomeSetor = Serial.readStringUntil('\n');
    nomeSetor.trim();

    if (nomeSetor.length() == 0) {
        feedbackErro(false); 
        exibirMenu();
        return;
    }

    Serial.println("Nome aceito: " + nomeSetor);
    Serial.println("APROXIME A TAG AGORA (10s)...");

    unsigned long inicio = millis();
    int ultimoSegundo = 10;

    while (millis() - inicio < 10000) {
        int segundoAtual = 10 - (millis() - inicio) / 1000;
        if (segundoAtual != ultimoSegundo) {
            ultimoSegundo = segundoAtual;
            feedbackInicioGravacao(ultimoSegundo);
        }

        if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
            byte buffer[16];
            for (byte i = 0; i < 16; i++) {
                buffer[i] = (i < nomeSetor.length()) ? nomeSetor[i] : ' ';
            }

            // A autenticação é obrigatória antes de gravar em cartões MIFARE Classic!
            if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) { 
                if (mfrc522.MIFARE_Write(2, buffer, 16) == MFRC522::STATUS_OK) {
                    Serial.println(">>> SUCESSO!");
                    // Usando o texto longo que combina com a tela de sucesso
                    alertaSucesso("GRAVADO COM SUCESSO", nomeSetor); 
                } else {
                    feedbackErro(false);
                }
            } else {
                 feedbackErro(false); // Caso a autenticação falhe
            }
            
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            exibirMenu();
            return;
        }
    }
    
    Serial.println(">>> TEMPO ESGOTADO!");
    feedbackErro(true);
    exibirMenu();
}