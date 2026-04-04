#include "beam.h"
#include "anneling.h"

#include <stdlib.h>

//5 feixes ativos por iteracao.
static const int BEAM_WIDTH = 5;
//Cada feixe gera 5 vizinhos por ciclo.
static const int NEIGHBORS_PER_STATE = 5;
static const int ITERATIONS = 1500;
//Pool filtrado para manter os melhores candidatos.
//10 de 25 (5 feixes x 5 vizinhos): reduz escolhas muito distantes,
//mas preserva exploração estocástica com "caos controlado".
static const int TOP_CANDIDATES = 10;
static const double ETA = 0.50;
static const double EPSILON = 1e-9;

typedef struct {
    Point point;
    int active;
} WeightedCandidate;

static int comparePointValue(const void *a, const void *b) {
    const Point *pa = (const Point *)a;
    const Point *pb = (const Point *)b;

    if (pa->value < pb->value) return -1;
    if (pa->value > pb->value) return 1;
    return 0;
}

static Point neighborOf(Point base) {
    Point next;
    next.x = base.x + ETA * (2.0 * (double)rand() / (double)RAND_MAX - 1.0);
    next.y = base.y + ETA * (2.0 * (double)rand() / (double)RAND_MAX - 1.0);
    next.x = clampToDomain(next.x, SEARCH_DOMAIN_MIN, SEARCH_DOMAIN_MAX);
    next.y = clampToDomain(next.y, SEARCH_DOMAIN_MIN, SEARCH_DOMAIN_MAX);
    next.value = evaluateObjective(next.x, next.y);
    return next;
}

static Point stochasticPick(WeightedCandidate candidates[], int count) {
    double totalWeight = 0.0;
    for (int i = 0; i < count; i++) {
        if (!candidates[i].active) continue;
        //Peso inverso do valor objetivo: menor f(x,y), maior chance.
        totalWeight += 1.0 / (candidates[i].point.value + EPSILON);
    }

    if (totalWeight <= 0.0) {
        for (int i = 0; i < count; i++) {
            if (candidates[i].active) {
                candidates[i].active = 0;
                return candidates[i].point;
            }
        }
    }

    double threshold = randomInRange(0.0, totalWeight);
    double cumulative = 0.0;

    for (int i = 0; i < count; i++) {
        if (!candidates[i].active) continue;
        cumulative += 1.0 / (candidates[i].point.value + EPSILON);
        if (cumulative >= threshold) {
            candidates[i].active = 0;
            return candidates[i].point;
        }
    }

    for (int i = count - 1; i >= 0; i--) {
        if (candidates[i].active) {
            candidates[i].active = 0;
            return candidates[i].point;
        }
    }

    return candidates[0].point;
}

Point stochasticLocalBeamSearch(void) {
    Point beams[BEAM_WIDTH];
    Point pool[BEAM_WIDTH * NEIGHBORS_PER_STATE];

    //Estados iniciais aleatorios dentro do dominio.
    for (int i = 0; i < BEAM_WIDTH; i++) {
        beams[i] = randomPoint(SEARCH_DOMAIN_MIN, SEARCH_DOMAIN_MAX);
    }

    Point best = beams[0];

    for (int iter = 0; iter < ITERATIONS; iter++) {
        int index = 0;
        for (int i = 0; i < BEAM_WIDTH; i++) {
            if (beams[i].value < best.value) {
                best = beams[i];
            }
            for (int j = 0; j < NEIGHBORS_PER_STATE; j++) {
                pool[index++] = neighborOf(beams[i]);
            }
        }

        qsort(pool, BEAM_WIDTH * NEIGHBORS_PER_STATE, sizeof(Point), comparePointValue);

        int candidateCount = TOP_CANDIDATES;
        int maxPool = BEAM_WIDTH * NEIGHBORS_PER_STATE;
        if (candidateCount > maxPool) {
            candidateCount = maxPool;
        }

        //Escolha estocastica entre os 10 melhores do pool (de 25 no total).
        //A ideia e limitar aleatoriedade extrema sem perder exploração.
        WeightedCandidate candidates[TOP_CANDIDATES];
        for (int i = 0; i < candidateCount; i++) {
            candidates[i].point = pool[i];
            candidates[i].active = 1;
        }

        for (int i = 0; i < BEAM_WIDTH; i++) {
            beams[i] = stochasticPick(candidates, candidateCount);
            if (beams[i].value < best.value) {
                best = beams[i];
            }
        }
    }

    return best;
}
