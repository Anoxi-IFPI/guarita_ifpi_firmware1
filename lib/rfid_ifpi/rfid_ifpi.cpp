//biblioteca de menu das funções de leitura e gravação do RFID
#include "rfid_ifpi.h"
//biblioteca do buzzer e LEDs para sinalização de sucesso, erro e atenção
#include "buzzer_leds.h"

//instancia do rc522 para configurar os pinos de comunicação SPI e reset
MFRC522 mfrc522(SS_PIN, RST_PIN); 
//chave de autenticação padrão para acessar os setores de memoria do cartão
MFRC522::MIFARE_Key key;

//inicialização do RFID, SPI e autenticação
void init_RFID() {
    SPI.begin();
    mfrc522.PCD_Init();
    //preenche a chave de autenticação com o valor padrão (0xFF 0xFF 0xFF 0xFF 0xFF 0xFF)
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}

//exibe o menu inicial de opções para o usuário
void exibirMenu() {
    Serial.println("\n[ AGUARDANDO TAG... ]");
    Serial.println("Comando: Digite '0' para gravar novo setor");
}

//verifica se uma nova tag foi aproximada e se a leitura do UID foi bem sucedida
bool verificarTagPresente() {
    return (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial());
}

//função para ler o nome do setor gravado na tag, autenticando e lendo o bloco 2 do cartão, onde o nome do setor é armazenado
void lerNomeDoSetor(byte bloco) {
    Serial.print("ID da Tag:");
    //exibe o UID da tag lida em hexadecimal
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println(); 
    //buffer para armazenar o resultado da autenticação e leitura
    MFRC522::StatusCode status; 
    byte buffer[18];
    byte tamanho = 18;

    //status pega o resultado da autenticação do bloco 2 usando a chave padrão e o UID da tag lida
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &key, &(mfrc522.uid));
    
    // VERMELHO: Erro na leitura/autenticação
    if (status != MFRC522::STATUS_OK) {
        sinalizarErro(); 
        return;
    }
    // verifica se o bloco 2 está vazio 
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
    mfrc522.PICC_HaltA(); //coloca a tag em modo de espera
    mfrc522.PCD_StopCrypto1(); //para a criptografia após a leitura para liberar o acesso a outras tags
}


void gravarSetor() {
    // Limpa o buffer serial para evitar leituras indesejadas
    while(Serial.available() > 0) Serial.read(); 

    Serial.println("\n[ MODO GRAVACAO ]");
    
    // AMARELO: Liga por breve segundo e apaga ao iniciar o processo
    sinalizarAtencao(); 
    
    Serial.println("Digite o nome do setor e aperte Enter:");
    // Aguarda o usuário digitar o nome do setor e pressionar Enter
    while (Serial.available() == 0) { } // Espera até que haja algo digitado no monitor serial para gravar o setor
    //
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
        // Verifica se uma nova tag foi aproximada e se a leitura do UID foi bem sucedida
        if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
            delay(500); 
            byte buffer[16];
            for (byte i = 0; i < 16; i++) {
                buffer[i] = (i < nomeSetor.length()) ? nomeSetor[i] : ' ';
            }// Autentica o bloco 2 usando a chave padrão e o UID da tag lida, e grava o nome do setor no bloco 2
            //autentica o bloco 2 usando a chave padrão e o UID da tag lida
            if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 2, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) { 
                //grava o nome do setor no bloco 2
                if (mfrc522.MIFARE_Write(2, buffer, 16) == MFRC522::STATUS_OK) {
                    Serial.println(">>> SUCESSO!");
                    sinalizarSucesso(); // VERDE: Gravado com sucesso
                } else {
                    sinalizarErro();    // VERMELHO: Erro na escrita
                }
            }
            mfrc522.PICC_HaltA(); //coloca a tag em modo de espera
            mfrc522.PCD_StopCrypto1(); //para a criptografia após a escrita para liberar o acesso a outras tags
            exibirMenu();
            return;
        }
    }
    
    // VERMELHO: Se os 10 segundos passarem sem aproximar a tag
    Serial.println(">>> TEMPO ESGOTADO!");
    sinalizarErro(); 
    exibirMenu();
}