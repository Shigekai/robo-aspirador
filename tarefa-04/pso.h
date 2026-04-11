#ifndef PSO_H
#define PSO_H

#include "common.h"

#define SEARCH_DOMAIN_MIN (-5.12)
#define SEARCH_DOMAIN_MAX (5.12)

//Duas variantes do PSO compartilham o mesmo núcleo.
typedef enum {
    PSO_VMAX,     //Variante 1: controle de velocidade por vmax
    PSO_INERTIA   //Variante 2: vmax + peso de inércia W decrescente
} PsoVariant;

//Partícula do enxame: posição, velocidade e melhor local.
typedef struct {
    double x,  y;   //Posição atual
    double vx, vy;  //Velocidade atual
    double bx, by;  //Melhor posição individual (x*_i)
    double bv;      //f(x*_i)
} Particle;

//Parâmetros configuráveis do PSO.
typedef struct {
    int    n;        //Tamanho do enxame
    int    tmax;     //Número de iterações
    double c1, c2;   //Coeficientes cognitivo e social
    double vmax;     //Limite de velocidade (variante PSO_VMAX)
    double wmin;     //Peso de inércia mínimo (variante PSO_INERTIA)
    double wmax;     //Peso de inércia máximo (variante PSO_INERTIA)
} PsoConfig;

Point pso(PsoVariant variant, PsoConfig cfg);

#endif
