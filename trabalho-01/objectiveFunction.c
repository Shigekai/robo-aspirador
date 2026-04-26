#include <math.h>
//Garantimos a constante para portar entre toolchains.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//Funcao de Rastrigin 2D: minimo global em (0, 0) com f = 0.
double rastrigin(double x, double y) {
    double termX = pow(x, 2) - 10 * cos(2.0 * M_PI * x);
    double termY = pow(y, 2) - 10 * cos(2.0 * M_PI * y);

    return 20 + termX + termY;
}

//Funcao de Rosenbrock 2D: minimo global em (1, 1) com f = 0.
double rosenbrock(double x, double y) {
    return (1 - x) * (1 - x) + 100 * (y - x * x) * (y - x * x);
}
