#include "pso.h"

#include <math.h>
#include <stdlib.h>

//Sorteio uniforme em [0, 1] — componente aleatorio dos vetores c1 e c2.
static double rand01(void) {
    return (double)rand() / (double)RAND_MAX;
}

//Retorna configuracao com valores padrao.
//O chamador deve definir domainMin e domainMax antes de usar.
ConfigPSO configDefault(void) {
    ConfigPSO cfg = {
        .particles      = 30,
        .tMax            = 200,
        .c1              = 2.0,
        .c2              = 2.0,
        .vMax            = 5.0,
        .wMin            = 0.4,
        .wMax            = 0.9,
        .alpha           = 4.0,
        .fleeAmplitude   = 3.0,
        .fleeLambda      = 1.0,
        .Q               = 0.1,
        .dNuc            = 0.1,
        .dRep            = 0.0,
        .domainMin       = 0.0,
        .domainMax       = 0.0
    };
    return cfg;
}

//Nucleo do PSO. As duas variantes compartilham o clamp de vMax;
//apenas PSO_INERTIA ativa o peso de inercia W decrescente.
Point pso(PsoVariant variant, ConfigPSO cfg) {
    Particle *swarm = (Particle *)malloc(sizeof(Particle) * cfg.particles);

    //Melhor global do enxame — refinado apos inicializacao.
    Point gbest;
    gbest.value = 0.0;

    //Inicializacao: posicao aleatoria no dominio, velocidade zero,
    //melhor individual igual a posicao inicial.
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

    //Loop principal: atualiza velocidade, posicao e bests a cada iteracao.
    for (int t = 0; t < cfg.tMax; t++) {
        //Peso de inercia: decrescente linearmente apenas na variante PSO_INERTIA.
        double W = (variant == PSO_INERTIA)
                 ? cfg.wMax - ((cfg.wMax - cfg.wMin) / (double)cfg.tMax) * (double)t
                 : 1.0;

        for (int i = 0; i < cfg.particles; i++) {
            //Componentes aleatorios por dimensao.
            double r1x = rand01(), r1y = rand01();
            double r2x = rand01(), r2y = rand01();

            //Atualizacao da velocidade conforme o modelo canonico de PSO.
            swarm[i].vx = W * swarm[i].vx
                        + cfg.c1 * r1x * (swarm[i].bx - swarm[i].x)
                        + cfg.c2 * r2x * (gbest.x     - swarm[i].x);
            swarm[i].vy = W * swarm[i].vy
                        + cfg.c1 * r1y * (swarm[i].by - swarm[i].y)
                        + cfg.c2 * r2y * (gbest.y     - swarm[i].y);

            //Controle de velocidade por vMax (clamp): aplicado nas duas variantes.
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
