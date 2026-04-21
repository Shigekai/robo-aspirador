#include <stdio.h>

#include "common.h"
#include "objectiveFunction.h"
#include "pso.h"

//Executa as duas variantes do PSO para cada tamanho de enxame e imprime
//a convergência (f(x_opt) por iteração) seguida do resultado final.
static void runVariant(const char *label, PsoVariant variant, PsoConfig cfg) {
    printf("=== %s (N=%d) ===\n", label, cfg.n);
    Point r = pso(variant, cfg);
    printf("Final: x=%.4f, y=%.4f, f(x,y)=%.6f\n\n", r.x, r.y, r.value);
}

int main(void) {
    seedRandom();
    ObjectiveFunction = rastrigin;

    //Parâmetros base comuns às três variantes.
    PsoConfig cfg = {
        .n     = 0,
        .tmax  = 100,
        .c1    = 2.0,
        .c2    = 2.0,
        .vmax  = 2.0,
        .wmin  = 0.4,
        .wmax  = 0.9
    };

    int sizes[] = {5, 10, 25, 50};
    int nSizes  = sizeof(sizes) / sizeof(sizes[0]);

    printf("=== PSO — Rastrigin 2D ===\n");
    printf("Mínimo global: f(0, 0) = 0\n\n");

    //Varredura: cada tamanho de enxame testa as duas variantes
    //para permitir comparação direta da convergência.
    for (int i = 0; i < nSizes; i++) {
        cfg.n = sizes[i];
        runVariant("PSO vmax",    PSO_VMAX,    cfg);
        runVariant("PSO inercia", PSO_INERTIA, cfg);
    }

    return 0;
}
