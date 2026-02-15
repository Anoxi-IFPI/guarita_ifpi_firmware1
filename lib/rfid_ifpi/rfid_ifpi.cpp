#include "rfid_ifpi.h"
#include "buzzer_leds.h"

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void init_RFID() {
    SPI.begin();
    mfrc522.PCD_Init();
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}

void exibirMenu() {
    Serial.println("\n[ AGUARDANDO TAG... ]");
    Serial.println("Comando: Digite '0' para gravar novo setor");
}

bool verificarTagPresente() {
    return (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial());
}

void lerNomeDoSetor(byte bloco) {
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
    
    // VERMELHO: Erro na leitura/autenticação
    if (status != MFRC522::STATUS_OK) {
        sinalizarErro(); 
        return;
    }

    status = mfrc522.MIFARE_Read(bloco, buffer, &tamanho);
    if (status == MFRC522::STATUS_OK) {
        if (buffer[0] == 0 || buffer[0] == ' ') {
            Serial.println("AVISO: Tag sem cadastro!");
            sinalizarErro(); // VERMELHO: Tag vazia é erro de acesso
        } else {
            Serial.print("SETOR: ");
            for (byte i = 0; i < 16; i++) {
                if (buffer[i] != 0) Serial.write(buffer[i]);
            }
            Serial.println();
            sinalizarSucesso(); // VERDE: Leitura com sucesso
        }
    } else {
        sinalizarErro(); // VERMELHO: Falha física de leitura
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

void gravarSetor() {
    while(Serial.available() > 0) Serial.read(); 

    Serial.println("\n[ MODO GRAVACAO ]");
    
    // AMARELO: Liga por breve segundo e apaga ao iniciar o processo
    sinalizarAtencao(); 
    
    Serial.println("Digite o nome do setor e aperte Enter:");
    
    while (Serial.available() == 0) { } 
    
    delay(200); 
    String nomeSetor = Serial.readStringUntil('\n');
    nomeSetor.trim();

    if (nomeSetor.length() == 0) {
        sinalizarErro(); // VERMELHO: Se não digitar nada
        exibirMenu();
        return;
    }

    Serial.println("Nome aceito: " + nomeSetor);
    Serial.println("APROXIME A TAG AGORA (10s)...");

    unsigned long inicio = millis();
    while (millis() - inicio < 10000) {
        if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
            delay(500); 
            byte buffer[16];
            for (byte i = 0; i < 16; i++) {
                buffer[i] = (i < nomeSetor.length()) ? nomeSetor[i] : ' ';
            }

            if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {
                if (mfrc522.MIFARE_Write(2, buffer, 16) == MFRC522::STATUS_OK) {
                    Serial.println(">>> SUCESSO!");
                    sinalizarSucesso(); // VERDE: Gravado com sucesso
                } else {
                    sinalizarErro();    // VERMELHO: Erro na escrita
                }
            }
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
            exibirMenu();
            return;
        }
    }
    
    // VERMELHO: Se os 10 segundos passarem sem aproximar a tag
    Serial.println(">>> TEMPO ESGOTADO!");
    sinalizarErro(); 
    exibirMenu();
}