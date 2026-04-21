#include <math.h>

// Escolhemo utilizar rastrigin como função objetivo
// E geradora de terreno
double rastrigin(double x, double y) {
    double termX = pow(x, 2) - 10 * cos(2.0 * M_PI * x);
    double termY = pow(y, 2) - 10 * cos(2.0 * M_PI * y);

    return 20 + termX + termY;
}