#include <stdio.h>

#include "common.h"
#include "objectiveFunction.h"
#include "anneling.h"
#include "beam.h"

int main(void) {
    seedRandom();
    ObjectiveFunction = rastrigin;

    printf("=== Global Optimization (2D Rastrigin) ===\n");
    printf("Global minimum: f(0, 0) = 0\n\n");

    printf("[Simulated Annealing]\n");
    for (int i = 0; i < 5; i++) {
        Point result = simulatedAnneling();
        printf("Run %d: x=%.4f, y=%.4f, f(x,y)=%.4f\n", i + 1, result.x, result.y, result.value);
    }

    printf("\n[Stochastic Local Beam Search]\n");
    for (int i = 0; i < 5; i++) {
        Point result = stochasticLocalBeamSearch();
        printf("Run %d: x=%.4f, y=%.4f, f(x,y)=%.4f\n", i + 1, result.x, result.y, result.value);
    }

    return 0;
}
