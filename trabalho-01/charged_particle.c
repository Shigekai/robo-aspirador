#include "charged_particle.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//Sorteio uniforme em [0, 1]
//Componente aleatório dos vetores c1 e c2.
static double rand01(void) {
    return (double)rand() / (double)RAND_MAX;
}

//Modelo charged particle: PSO + repulsão de Coulomb
Point chargedParticle(CpVariant variant, ConfigPSO cfg) {
    //Validação: dRep precisa ser estritamente maior que dNuc, caso contrário
    //a repulsão é silenciosamente desligada
    if (cfg.dRep <= cfg.dNuc) {
        fprintf(stderr,
                "chargedParticle: cfg.dRep (%.6f) deve ser maior que cfg.dNuc (%.6f)\n",
                cfg.dRep, cfg.dNuc);
        exit(EXIT_FAILURE);
    }

    Particle *swarm = (Particle *)malloc(sizeof(Particle) * cfg.particles);

    //Q^2 calculado uma vez (carga uniforme entre partículas).
    const double Q2 = cfg.Q * cfg.Q;

    //Melhor global do enxame — refinado após inicialização.
    Point gbest;
    gbest.value = 0.0;

    //Inicialização: posição aleatória, velocidade (componente PSO) zero,
    //melhor individual igual a posição inicial.
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

    //Loop principal.
    for (int t = 0; t < cfg.tMax; t++) {
        //Peso de inércia: decrescente apenas na variante CP_INERTIA.
        double W = (variant == CP_INERTIA)
                 ? cfg.wMax - ((cfg.wMax - cfg.wMin) / (double)cfg.tMax) * (double)t
                 : 1.0;

        for (int i = 0; i < cfg.particles; i++) {
            //Componentes aleatórios por dimensão.
            double r1x = rand01(), r1y = rand01();
            double r2x = rand01(), r2y = rand01();

            //Componente PSO: usa Particle.vx/vy como histórico (memória de inércia).
            double vpsoX = W * swarm[i].vx
                         + cfg.c1 * r1x * (swarm[i].bx - swarm[i].x)
                         + cfg.c2 * r2x * (gbest.x     - swarm[i].x);
            double vpsoY = W * swarm[i].vy
                         + cfg.c1 * r1y * (swarm[i].by - swarm[i].y)
                         + cfg.c2 * r2y * (gbest.y     - swarm[i].y);

            //Clamp vMax aplicado a componente PSO (ambas as variantes).
            double norm = sqrt(vpsoX * vpsoX + vpsoY * vpsoY);
            if (norm > cfg.vMax) {
                vpsoX = cfg.vMax * vpsoX / norm;
                vpsoY = cfg.vMax * vpsoY / norm;
            }

            //Persiste a componente PSO clampada para uso na próxima iteração.
            //A repulsão não entra aqui — ela não se acumula via inércia.
            swarm[i].vx = vpsoX;
            swarm[i].vy = vpsoY;

            //Termo de repulsão: soma vetorial de Coulomb sobre j != i,
            //ativo apenas na faixa dNuc < r < dRep.
            double repX = 0.0;
            double repY = 0.0;
            for (int j = 0; j < cfg.particles; j++) {
                if (j == i) continue;

                double dx = swarm[i].x - swarm[j].x;
                double dy = swarm[i].y - swarm[j].y;
                double r2 = dx * dx + dy * dy;
                double r  = sqrt(r2);

                if (r > cfg.dNuc && r < cfg.dRep) {
                    double inv_r3 = 1.0 / (r2 * r);
                    repX += Q2 * dx * inv_r3;
                    repY += Q2 * dy * inv_r3;
                }
            }

            //Atualização da posição com (componente PSO + repulsão), clampada ao domínio.
            swarm[i].x = clampToDomain(swarm[i].x + vpsoX + repX, cfg.domainMin, cfg.domainMax);
            swarm[i].y = clampToDomain(swarm[i].y + vpsoY + repY, cfg.domainMin, cfg.domainMax);

            //Avalia nova posição e atualiza bests individual e global.
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
