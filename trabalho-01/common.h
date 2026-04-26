#ifndef COMMON_H
#define COMMON_H

typedef struct {
    double x;
    double y;
    double value;
} Point;

extern double (*ObjectiveFunction)(double x, double y);

void seedRandom(void);
void seedRandomWith(unsigned int seed);
double randomInRange(double min, double max);
double clampToDomain(double value, double min, double max);
double evaluateObjective(double x, double y);
Point randomPoint(double min, double max);

#endif
