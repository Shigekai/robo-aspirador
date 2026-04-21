#include "predator_prey.h"

#include <math.h>
#include <stdlib.h>

//Limiar para evitar divisao por zero no vetor unitario de fuga.
#define EPSILON 1e-12

//Sorteio uniforme em [0, 1] — componente aleatorio dos vetores c1 e c2.
static double rand01(void) {
    return (double)rand() / (double)RAND_MAX;
}

//Modelo predador-presa: um predador persegue o gbest do enxame,
//e as presas fogem do predador enquanto otimizam via PSO.
//A cada iteracao: predador move primeiro, depois as presas reagem.
Point predatorPrey(PpVariant variant, ConfigPSO cfg) {
    Particle *swarm = (Particle *)malloc(sizeof(Particle) * cfg.particles);

    //Melhor global do enxame — refinado apos inicializacao.
    Point gbest;
    gbest.value = 0.0;

    //Inicializacao das presas: mesma logica do PSO base.
    for (int i = 0; i < cfg.particles; i++) {
        swarm[i].x  = randomInRange(cfg.domainMin, cfg.domainMax);
        swarm[i].y  = randomInRange(cfg.domainMin, cfg.domainMax);
        swarm[i].vx = 0.0;
        swarm[i].vy = 0.0;
        swarm[i].bx = swarm[i].x;
        swarm[i].by = swarm[i].y;
        swarm[i].bv = evaluateObjective(swarm[i].x, swarm[i].y);

        if (i == 0 || swarm[i].bv < gbest.value) {
            gbest.x     = swarm[i].bx;
            gbest.y     = swarm[i].by;
            gbest.value = swarm[i].bv;
        }
    }

    //Predador: posicao aleatoria no dominio, velocidade zero.
    Predator pred;
    pred.x  = randomInRange(cfg.domainMin, cfg.domainMax);
    pred.y  = randomInRange(cfg.domainMin, cfg.domainMax);
    pred.vx = 0.0;
    pred.vy = 0.0;

    //Loop principal.
    for (int t = 0; t < cfg.tMax; t++) {
        //Peso de inercia: decrescente apenas na variante PP_INERTIA.
        double W = (variant == PP_INERTIA)
                 ? cfg.wMax - ((cfg.wMax - cfg.wMin) / (double)cfg.tMax) * (double)t
                 : 1.0;

        //Passo 1: predador persegue o gbest com velocidade proporcional a alpha.
        pred.vx = cfg.alpha * (gbest.x - pred.x);
        pred.vy = cfg.alpha * (gbest.y - pred.y);
        pred.x  = clampToDomain(pred.x + pred.vx, cfg.domainMin, cfg.domainMax);
        pred.y  = clampToDomain(pred.y + pred.vy, cfg.domainMin, cfg.domainMax);

        //Passo 2: presas atualizam velocidade (PSO + fuga) e posicao.
        for (int i = 0; i < cfg.particles; i++) {
            //Componentes aleatorios por dimensao.
            double r1x = rand01(), r1y = rand01();
            double r2x = rand01(), r2y = rand01();

            //Velocidade PSO padrao.
            swarm[i].vx = W * swarm[i].vx
                        + cfg.c1 * r1x * (swarm[i].bx - swarm[i].x)
                        + cfg.c2 * r2x * (gbest.x     - swarm[i].x);
            swarm[i].vy = W * swarm[i].vy
                        + cfg.c1 * r1y * (swarm[i].by - swarm[i].y)
                        + cfg.c2 * r2y * (gbest.y     - swarm[i].y);

            //Termo de fuga: direcao oposta ao predador, amplitude decai com distancia.
            //v_fuga = A * exp(-lambda * ||d||) * (d / ||d||), onde d = presa - predador.
            double dx   = swarm[i].x - pred.x;
            double dy   = swarm[i].y - pred.y;
            double dist = sqrt(dx * dx + dy * dy);

            if (dist > EPSILON) {
                double flee = cfg.fleeAmplitude * exp(-cfg.fleeLambda * dist);
                swarm[i].vx += flee * (dx / dist);
                swarm[i].vy += flee * (dy / dist);
            }

            //Controle de velocidade por vMax (clamp).
            double norm = sqrt(swarm[i].vx * swarm[i].vx + swarm[i].vy * swarm[i].vy);
            if (norm > cfg.vMax) {
                swarm[i].vx = cfg.vMax * swarm[i].vx / norm;
                swarm[i].vy = cfg.vMax * swarm[i].vy / norm;
            }

            //Atualizacao da posicao com clamp ao dominio.
            swarm[i].x = clampToDomain(swarm[i].x + swarm[i].vx, cfg.domainMin, cfg.domainMax);
            swarm[i].y = clampToDomain(swarm[i].y + swarm[i].vy, cfg.domainMin, cfg.domainMax);

            //Avalia nova posicao e atualiza bests individual e global.
            double fval = evaluateObjective(swarm[i].x, swarm[i].y);
            if (fval < swarm[i].bv) {
                swarm[i].bv = fval;
                swarm[i].bx = swarm[i].x;
                swarm[i].by = swarm[i].y;
                if (fval < gbest.value) {
                    gbest.x     = swarm[i].x;
                    gbest.y     = swarm[i].y;
                    gbest.value = fval;
                }
            }
        }
    }

    free(swarm);
    return gbest;
}
