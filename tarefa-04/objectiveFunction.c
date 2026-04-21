#include <math.h>

//Função de Rastrigin 2D: mínimo global em (0, 0) com f = 0.
double rastrigin(double x, double y) {
    double termX = pow(x, 2) - 10 * cos(2.0 * M_PI * x);
    double termY = pow(y, 2) - 10 * cos(2.0 * M_PI * y);

    return 20 + termX + termY;
}
