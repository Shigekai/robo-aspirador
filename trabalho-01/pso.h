#ifndef PSO_H
#define PSO_H

#include "common.h"

//Duas variantes de controle de velocidade do PSO.
typedef enum {
    PSO_VMAX,     //Variante 1: limitacao por velocidade maxima (clamp)
    PSO_INERTIA   //Variante 2: peso de inercia W decrescente + vMax
} PsoVariant;

//Particula do enxame: posicao, velocidade e melhor individual.
typedef struct {
    double x,  y;   //Posicao atual
    double vx, vy;  //Velocidade atual
    double bx, by;  //Melhor posicao individual (pbest)
    double bv;      //Valor de f(pbest)
} Particle;

//Configuração unificada para PSO, predador-presa e charged particle.
//Campos alpha, fleeAmplitude e fleeLambda são usados apenas pelo modelo
//predador-presa. Campos Q, dNuc e dRep são usados apenas pelo charged particle.
typedef struct {
    int    particles;       //Numero de particulas no enxame
    int    tMax;            //Numero maximo de iteracoes
    double c1, c2;          //Coeficientes cognitivo e social
    double vMax;            //Velocidade maxima (clamp)
    double wMin, wMax;      //Limites do peso de inercia
    double alpha;           //Velocidade de perseguicao do predador
    double fleeAmplitude;   //Amplitude de fuga da presa (A)
    double fleeLambda;      //Decaimento exponencial da fuga (lambda)
    double Q;               //Carga uniforme das partículas (charged particle)
    double dNuc;            //Raio nuclear
    double dRep;            //Raio de repulsão
    double repMax;          //Salvaguarda numerica: limite na norma do vetor
                            //de repulsao do CPSO. 0.0 desativa o limite.
    double domainMin;       //Limite inferior do dominio de busca
    double domainMax;       //Limite superior do dominio de busca
} ConfigPSO;

//Retorna configuracao com valores padrao.
ConfigPSO configDefault(void);

//Executa PSO com a variante especificada e retorna o melhor ponto.
Point pso(PsoVariant variant, ConfigPSO cfg);

#endif
