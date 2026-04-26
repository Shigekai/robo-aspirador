#ifndef CHARGED_PARTICLE_H
#define CHARGED_PARTICLE_H

#include "pso.h"

//Variantes de controle de velocidade aplicadas a componente PSO.
typedef enum {
    CP_VMAX,     //Componente PSO usa clamp por velocidade máxima
    CP_INERTIA   //Componente PSO usa peso de inércia W decrescente + vMax
} CpVariant;

//Executa o modelo charged particle (PSO + repulsão de Coulomb par-a-par)
//e retorna o melhor ponto encontrado.
Point chargedParticle(CpVariant variant, ConfigPSO cfg);

#endif
