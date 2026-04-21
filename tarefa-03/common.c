#include "common.h"

#include <stdlib.h>
#include <time.h>

//Arquivo para métodos comuns para anneling e beam
double (*ObjectiveFunction)(double x, double y) = 0;

void seedRandom(void) {
    srand((unsigned int)time(NULL));
}

double randomInRange(double min, double max) {
    return min + (max - min) * ((double)rand() / (double)RAND_MAX);
}

double clampToDomain(double value, double min, double max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

double evaluateObjective(double x, double y) {
    return ObjectiveFunction ? ObjectiveFunction(x, y) : 0.0;
}

Point randomPoint(double min, double max) {
    Point p;
    p.x = randomInRange(min, max);
    p.y = randomInRange(min, max);
    p.value = evaluateObjective(p.x, p.y);
    return p;
}

