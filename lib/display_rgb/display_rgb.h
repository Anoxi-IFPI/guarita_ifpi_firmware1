// #ifndef DISPLAY_RGB_H
// #define DISPLAY_RGB_H

// #include <Arduino.h>
// #include <TFT_eSPI.h> // Primeiro incluímos a biblioteca...

// // ...depois dizemos que o tft existe em outro lugar (extern)
// extern TFT_eSPI tft; 
// extern unsigned long timerAlerta;
// extern bool displayingAlert;

// // Protótipos das funções
// void desenharNavbar();
// void limparCentro();   
// void alertaSucesso(String nome, String setor);
// void feedbackErro(bool tempoEsgotado);
// void feedbackInicioGravacao(int segundos);

// #endif

#ifndef DISPLAY_RGB_H
#define DISPLAY_RGB_H

#include <Arduino.h>
#include <TFT_eSPI.h> 

extern TFT_eSPI tft; 
extern unsigned long timerAlerta;
extern bool displayingAlert;

// Protótipos das funções de Inicialização e Controle
void init_Display();
void gerenciarTimerTela();

// Protótipos das funções de Desenho
void desenharNavbar();
void limparCentro();   
void avisoCadastreWeb();
void telaTecladoDinamico(String tituloOQueDigitar, String digitosAtuais);

// Protótipos das funções de Feedback
void alertaSucesso(String nome, String setor);
void alertarPermissaoSucesso(String nome, String setor);
void alertarPermissaoErro(String nome, String motivo);
void feedbackErro(bool tempoEsgotado);
void feedbackInicioGravacao(int segundos);

#endif