#include "pso.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//Sorteio uniforme em [0, 1] — representa o componente aleatório dos
//vetores c1 e c2 (um valor por dimensão).
static double rand01(void) {
    return (double)rand() / (double)RAND_MAX;
}

//Núcleo do PSO. As duas variantes compartilham o clamp de vmax;
//apenas a variante PSO_INERTIA ativa o peso de inércia W.
Point pso(PsoVariant variant, PsoConfig cfg) {
    Particle *swarm = (Particle *)malloc(sizeof(Particle) * cfg.n);

    //Melhor global do enxame — inicializado na primeira partícula
    //e refinado após a geração de todas as posições iniciais.
    Point gbest;
    gbest.value = 0.0;

    //Inicialização: posição aleatória no domínio, velocidade zero,
    //melhor individual igual à posição inicial.
    for (int i = 0; i < cfg.n; i++) {
        swarm[i].x  = randomInRange(SEARCH_DOMAIN_MIN, SEARCH_DOMAIN_MAX);
        swarm[i].y  = randomInRange(SEARCH_DOMAIN_MIN, SEARCH_DOMAIN_MAX);
        swarm[i].vx = 0.0;
        swarm[i].vy = 0.0;
        swarm[i].bx = swarm[i].x;
        swarm[i].by = swarm[i].y;
        swarm[i].bv = evaluateObjective(swarm[i].x, swarm[i].y);

        if (i == 0 || swarm[i].bv < gbest.value) {
            gbest.x = swarm[i].bx;
            gbest.y = swarm[i].by;
            gbest.value = swarm[i].bv;
        }
    }

    //Loop principal: atualiza velocidade, posição e bests em cada iteração.
    for (int t = 0; t < cfg.tmax; t++) {
        //Peso de inércia: decrescente linearmente apenas na variante PSO_INERTIA.
        double W = (variant == PSO_INERTIA)
                 ? cfg.wmax - ((cfg.wmax - cfg.wmin) / (double)cfg.tmax) * (double)t
                 : 1.0;

        for (int i = 0; i < cfg.n; i++) {
            //Componentes aleatórios dos vetores c1⊙ e c2⊙ (um por dimensão).
            double r1x = rand01(), r1y = rand01();
            double r2x = rand01(), r2y = rand01();

            //Atualização da velocidade conforme o modelo canônico de PSO.
            swarm[i].vx = W * swarm[i].vx
                        + cfg.c1 * r1x * (swarm[i].bx - swarm[i].x)
                        + cfg.c2 * r2x * (gbest.x   - swarm[i].x);
            swarm[i].vy = W * swarm[i].vy
                        + cfg.c1 * r1y * (swarm[i].by - swarm[i].y)
                        + cfg.c2 * r2y * (gbest.y   - swarm[i].y);

            //Controle de velocidade por vmax: aplicado nas duas variantes.
            double norm = sqrt(swarm[i].vx * swarm[i].vx + swarm[i].vy * swarm[i].vy);
            if (norm > cfg.vmax) {
                swarm[i].vx = cfg.vmax * swarm[i].vx / norm;
                swarm[i].vy = cfg.vmax * swarm[i].vy / norm;
            }

            //Atualização da posição com clamp para manter no domínio de Rastrigin.
            swarm[i].x = clampToDomain(swarm[i].x + swarm[i].vx, SEARCH_DOMAIN_MIN, SEARCH_DOMAIN_MAX);
            swarm[i].y = clampToDomain(swarm[i].y + swarm[i].vy, SEARCH_DOMAIN_MIN, SEARCH_DOMAIN_MAX);

            //Avalia nova posição e atualiza bests individual e global.
            double fval = evaluateObjective(swarm[i].x, swarm[i].y);
            if (fval < swarm[i].bv) {
                swarm[i].bv = fval;
                swarm[i].bx = swarm[i].x;
                swarm[i].by = swarm[i].y;
                if (fval < gbest.value) {
                    gbest.x = swarm[i].x;
                    gbest.y = swarm[i].y;
                    gbest.value = fval;
                }
            }
        }

        //Log de convergência: f(x_opt) a cada iteração.
        printf("iter %3d: f=%.6f\n", t, gbest.value);
    }

    free(swarm);
    return gbest;
}
