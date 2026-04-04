#include "common.h"
#include "anneling.h"

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
//Perturbação de estado a cada iteração
static const double ETA = 0.50;
//Limite de temperatura:
static const double THRESHOLD = 0.0001;

//Função responsável pela redução da temperatura a cada iteração
static double schedule(double temp) {
    double alpha = 0.9995;
    return temp * alpha;
}

//Função que define se um estado de menor potencial
//Será aceito, considerando a temperatura e a diferença de potencial.
static bool acceptance(double temp, double delta) {
    double p = exp(-delta / temp);
    double r = (double)rand() / (double)RAND_MAX;
    return r <= p;
}

//Função de recozimento simulado
Point simulatedAnneling(void) {
    //Estado inicial com base nos macros definidos
    Point current = randomPoint(SEARCH_DOMAIN_MIN, SEARCH_DOMAIN_MAX);
    double temp = INITIAL_TEMP;
    //Continua até atingir o limite de temperatura definido
    while (temp > THRESHOLD) {
        Point next;
        next.x = current.x + ETA * (2.0 * (double)rand() / (double)RAND_MAX - 1.0);
        next.y = current.y + ETA * (2.0 * (double)rand() / (double)RAND_MAX - 1.0);
        next.x = clampToDomain(next.x, SEARCH_DOMAIN_MIN, SEARCH_DOMAIN_MAX);
        next.y = clampToDomain(next.y, SEARCH_DOMAIN_MIN, SEARCH_DOMAIN_MAX);
        next.value = evaluateObjective(next.x, next.y);
        //Diferença de "potencial" de cada estado
        double delta = next.value - current.value;
        if (delta < 0.0 || acceptance(temp, delta)) {
            current = next;
        }

        temp = schedule(temp);
    }

    return current;
}
