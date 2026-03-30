#include "teclado_matricial.h"
#include <Wire.h> 
#include <Keypad_I2C.h> 

#define I2CADDR 0x20
const uint8_t ROWS = 4;
const uint8_t COLS = 4;

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

uint8_t rowPins[ROWS] = { 0, 1, 2, 3 };  
uint8_t colPins[COLS] = { 4, 5, 6, 7 }; 

Keypad_I2C keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR);

void init_Teclado() {
  Wire.begin(21, 32); // Inicia o barramento
  keypad.begin();     // Inicia o teclado
}

char lerTecla() {
  return keypad.getKey(); // Apenas devolve a letra/número apertado
}