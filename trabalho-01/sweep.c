#include <math.h>
#include <stdio.h>
#include <string.h>

#include "charged_particle.h"
#include "common.h"
#include "objectiveFunction.h"
#include "predator_prey.h"
#include "pso.h"

//Identifica o algoritmo na hora de escrever a linha do CSV.
typedef enum { ALGO_PSO, ALGO_PP, ALGO_CP } AlgoKind;

//Funcao de teste com nome, dominio e ponteiro para a funcao de custo.
typedef struct {
    const char *name;
    double (*func)(double, double);
    double dMin, dMax;
} TestFunction;

//Escreve uma linha do CSV bruto. Campos que nao se aplicam ao algoritmo
//ficam vazios (convencao cientifica de CSV).
static void writeRow(FILE *f, AlgoKind algo, const char *metodo,
                     const char *funcao, int seed, ConfigPSO cfg, Point r) {
    int usesInertia = strcmp(metodo, "inertia") == 0;

    const char *algoStr = (algo == ALGO_PSO) ? "PSO"
                        : (algo == ALGO_PP)  ? "PP"
                                             : "CP";

    fprintf(f, "%s,%s,%s,%d,%d,%d,",
            algoStr, metodo, funcao, seed, cfg.particles, cfg.tMax);

    //wMax, wMin: apenas variantes inertia.
    if (usesInertia) fprintf(f, "%.6f,%.6f,", cfg.wMax, cfg.wMin);
    else             fprintf(f, ",,");

    //c1, c2: usados por todos os algoritmos.
    fprintf(f, "%.6f,%.6f,", cfg.c1, cfg.c2);

    //vMax: aplicado em todas as variantes (clamp tanto na inercia quanto no
    //metodo clamp puro).
    fprintf(f, "%.6f,", cfg.vMax);

    //alpha, A, lambda: apenas predator-prey.
    if (algo == ALGO_PP) {
        fprintf(f, "%.6f,%.6f,%.6f,",
                cfg.alpha, cfg.fleeAmplitude, cfg.fleeLambda);
    } else {
        fprintf(f, ",,,");
    }

    //Q, dNuc, dRep: apenas charged particle.
    if (algo == ALGO_CP) {
        fprintf(f, "%.6f,%.6f,%.6f,", cfg.Q, cfg.dNuc, cfg.dRep);
    } else {
        fprintf(f, ",,,");
    }

    //Resultado: f e coordenadas finais.
    fprintf(f, "%.6f,%.6f,%.6f\n", r.value, r.x, r.y);
}

//Imprime progresso resumido de uma execucao em stderr.
static void logProgress(int counter, int total, const char *algoMetodo,
                        const char *paramSummary, const char *funcao,
                        int seed, double f, int dropped) {
    if (dropped) {
        fprintf(stderr, "[%03d/%d] %s %s %s seed=%d DROPPED\n",
                counter, total, algoMetodo, paramSummary, funcao, seed);
    } else {
        fprintf(stderr, "[%03d/%d] %s %s %s seed=%d -> f=%.6f\n",
                counter, total, algoMetodo, paramSummary, funcao, seed, f);
    }
}

int main(void) {
    FILE *csv = fopen("data/varredura_brutos.csv", "w");
    if (!csv) {
        perror("Nao foi possivel abrir data/varredura_brutos.csv");
        return 1;
    }

    //Cabecalho.
    fprintf(csv,
            "algoritmo,metodo,funcao,seed,n,t_max,"
            "w_max,w_min,c1,c2,v_max,"
            "alpha,A,lambda,Q,d_nuc,d_rep,"
            "f_final,x_final,y_final\n");

    const int seeds[] = {1, 2, 3};
    const int nSeeds  = 3;
    const int total   = 288;

    TestFunction functions[] = {
        {"Rastrigin",  rastrigin,  RASTRIGIN_DOMAIN_MIN,  RASTRIGIN_DOMAIN_MAX},
        {"Rosenbrock", rosenbrock, ROSENBROCK_DOMAIN_MIN, ROSENBROCK_DOMAIN_MAX}
    };
    const int nFuncs = sizeof(functions) / sizeof(functions[0]);

    int counter = 0;
    int written = 0;

    for (int f = 0; f < nFuncs; f++) {
        ObjectiveFunction = functions[f].func;

        ConfigPSO cfg = configDefault();
        cfg.domainMin = functions[f].dMin;
        cfg.domainMax = functions[f].dMax;
        cfg.repMax    = cfg.domainMax - cfg.domainMin;
        cfg.particles = 30;
        cfg.tMax      = 200;

        //----------- Bloco PSO base (variante clamp) -----------
        //Varre vMax, mantendo c1=c2=2.0 (literatura).
        const double vMaxes[] = {2.0, 5.0, 10.0};
        for (int v = 0; v < 3; v++) {
            cfg.vMax = vMaxes[v];
            for (int s = 0; s < nSeeds; s++) {
                int seed = seeds[s];
                seedRandomWith((unsigned int)seed);
                Point r = pso(PSO_VMAX, cfg);
                counter++;

                int dropped = !isfinite(r.value);
                if (!dropped) {
                    writeRow(csv, ALGO_PSO, "clamp",
                             functions[f].name, seed, cfg, r);
                    written++;
                }

                char ps[64];
                snprintf(ps, sizeof(ps), "vMax=%.2f", cfg.vMax);
                logProgress(counter, total, "PSO clamp", ps,
                            functions[f].name, seed, r.value, dropped);
            }
        }

        //Restaura vMax para o default da literatura. PP e CP nao varrem este
        //parametro nesta etapa; sao varridos os hiperparametros novos deles.
        cfg.vMax = 5.0;

        //----------- Bloco Predator-Prey (variante inertia) -----------
        const double alphas[]  = {0.1, 0.5, 1.0};
        const double amps[]    = {1.0, 3.0, 5.0};
        const double lambdas[] = {0.5, 1.0, 2.0};
        for (int a = 0; a < 3; a++) {
            for (int b = 0; b < 3; b++) {
                for (int l = 0; l < 3; l++) {
                    cfg.alpha         = alphas[a];
                    cfg.fleeAmplitude = amps[b];
                    cfg.fleeLambda    = lambdas[l];

                    for (int s = 0; s < nSeeds; s++) {
                        int seed = seeds[s];
                        seedRandomWith((unsigned int)seed);
                        Point r = predatorPrey(PP_INERTIA, cfg);
                        counter++;

                        int dropped = !isfinite(r.value);
                        if (!dropped) {
                            writeRow(csv, ALGO_PP, "inertia",
                                     functions[f].name, seed, cfg, r);
                            written++;
                        }

                        char ps[96];
                        snprintf(ps, sizeof(ps),
                                 "alpha=%.2f A=%.2f lambda=%.2f",
                                 cfg.alpha, cfg.fleeAmplitude, cfg.fleeLambda);
                        logProgress(counter, total, "PP inertia", ps,
                                    functions[f].name, seed, r.value, dropped);
                    }
                }
            }
        }

        //----------- Bloco Charged Particle (variante inertia) -----------
        const double Qs[]    = {1.0, 3.0, 5.0};
        const double dNucs[] = {0.01, 0.1};
        const double dReps[] = {2.0, 5.0, 10.0};
        for (int q = 0; q < 3; q++) {
            for (int dn = 0; dn < 2; dn++) {
                for (int dr = 0; dr < 3; dr++) {
                    cfg.Q    = Qs[q];
                    cfg.dNuc = dNucs[dn];
                    cfg.dRep = dReps[dr];

                    for (int s = 0; s < nSeeds; s++) {
                        int seed = seeds[s];
                        seedRandomWith((unsigned int)seed);
                        Point r = chargedParticle(CP_INERTIA, cfg);
                        counter++;

                        int dropped = !isfinite(r.value);
                        if (!dropped) {
                            writeRow(csv, ALGO_CP, "inertia",
                                     functions[f].name, seed, cfg, r);
                            written++;
                        }

                        char ps[96];
                        snprintf(ps, sizeof(ps),
                                 "Q=%.2f dNuc=%.2f dRep=%.2f",
                                 cfg.Q, cfg.dNuc, cfg.dRep);
                        logProgress(counter, total, "CP inertia", ps,
                                    functions[f].name, seed, r.value, dropped);
                    }
                }
            }
        }
    }

    fclose(csv);
    fprintf(stderr, "[sweep] completo: %d/%d linhas escritas\n",
            written, total);
    return 0;
}
