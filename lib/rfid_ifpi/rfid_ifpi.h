#ifndef RFID_IFPI_H
#define RFID_IFPI_H

#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

// Definições de pinos (Padrão para quem usar a biblioteca)
#define SS_PIN 5
#define RST_PIN 22

extern SPIClass hspi;

extern MFRC522 mfrc522;
extern MFRC522::MIFARE_Key key; 

// Protótipos das funções (O que o módulo oferece)
void exibirMenu();
void init_RFID(); // Função para substituir o setup interno do módulo
String lerNomeDoSetor(byte bloco);
void gravarSetor();
bool verificarTagPresente(); // Função auxiliar para o loop principal

#endif