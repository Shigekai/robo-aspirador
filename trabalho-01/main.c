#include <stdio.h>

#include "common.h"
#include "objectiveFunction.h"
#include "pso.h"
#include "predator_prey.h"
#include "charged_particle.h"

//Agrupa funcao de custo com seu dominio e ponto otimo conhecido.
typedef struct {
    const char *name;
    double (*func)(double, double);
    double domainMin, domainMax;
    double optX, optY;
} TestFunction;

int main(void) {
    seedRandom();

    //Funcoes de teste: Rastrigin e Rosenbrock 2D.
    TestFunction functions[] = {
        {"Rastrigin 2D",  rastrigin,  RASTRIGIN_DOMAIN_MIN,  RASTRIGIN_DOMAIN_MAX,  0.0, 0.0},
        {"Rosenbrock 2D", rosenbrock, ROSENBROCK_DOMAIN_MIN, ROSENBROCK_DOMAIN_MAX, 1.0, 1.0}
    };
    int nFunctions = sizeof(functions) / sizeof(functions[0]);

    ConfigPSO cfg = configDefault();

    //Executa os 4 algoritmos para cada funcao de custo.
    for (int f = 0; f < nFunctions; f++) {
        ObjectiveFunction = functions[f].func;
        cfg.domainMin     = functions[f].domainMin;
        cfg.domainMax     = functions[f].domainMax;
        cfg.dRep          = 0.25 * (cfg.domainMax - cfg.domainMin);

        printf("=== %s ===\n", functions[f].name);
        printf("Minimo global: f(%.1f, %.1f) = 0\n\n", functions[f].optX, functions[f].optY);

        Point r;

        r = pso(PSO_VMAX, cfg);
        printf("[PSO vMax]          x=%.6f, y=%.6f, f=%.6f\n", r.x, r.y, r.value);

        r = pso(PSO_INERTIA, cfg);
        printf("[PSO Inertia]       x=%.6f, y=%.6f, f=%.6f\n", r.x, r.y, r.value);

        r = predatorPrey(PP_VMAX, cfg);
        printf("[Pred-Prey vMax]    x=%.6f, y=%.6f, f=%.6f\n", r.x, r.y, r.value);

        r = predatorPrey(PP_INERTIA, cfg);
        printf("[Pred-Prey Inertia] x=%.6f, y=%.6f, f=%.6f\n", r.x, r.y, r.value);

        r = chargedParticle(CP_VMAX, cfg);
        printf("[CP vMax]           x=%.6f, y=%.6f, f=%.6f\n", r.x, r.y, r.value);

        r = chargedParticle(CP_INERTIA, cfg);
        printf("[CP Inertia]        x=%.6f, y=%.6f, f=%.6f\n", r.x, r.y, r.value);

        printf("\n");
    }

    return 0;
}
