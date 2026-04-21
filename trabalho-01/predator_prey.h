#ifndef PREDATOR_PREY_H
#define PREDATOR_PREY_H

#include "pso.h"

//Variantes de controle de velocidade das presas.
typedef enum {
    PP_VMAX,     //Presas usam clamp por velocidade maxima
    PP_INERTIA   //Presas usam peso de inercia W decrescente + vMax
} PpVariant;

//Predador: posicao e velocidade no espaco 2D.
typedef struct {
    double x, y;    //Posicao atual
    double vx, vy;  //Velocidade atual
} Predator;

//Executa o modelo predador-presa e retorna o melhor ponto encontrado.
Point predatorPrey(PpVariant variant, ConfigPSO cfg);

#endif
