#ifndef OBJECTIVEFUNCTION_H
#define OBJECTIVEFUNCTION_H

#define RASTRIGIN_DOMAIN_MIN  (-5.12)
#define RASTRIGIN_DOMAIN_MAX  (5.12)

#define ROSENBROCK_DOMAIN_MIN (-5.0)
#define ROSENBROCK_DOMAIN_MAX (10.0)

double rastrigin(double x, double y);
double rosenbrock(double x, double y);

#endif
