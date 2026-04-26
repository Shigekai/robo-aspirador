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

//Hiperparametros escolhidos na varredura (Stage 3a) — uma instancia por
//funcao de custo. Ver docs/varredura-hiperparametros.md para os criterios.
typedef struct {
    double vMaxClamp;            //Aplicado em PSO_VMAX e herdado por PP/CP em variante clamp
    double ppAlpha, ppA, ppLambda;
    double cpQ, cpDNuc, cpDRep;
} ChosenParams;

static const ChosenParams RASTRIGIN_PARAMS = {
    .vMaxClamp = 2.0,
    .ppAlpha   = 1.0, .ppA = 1.0, .ppLambda = 2.0,
    .cpQ       = 1.0, .cpDNuc = 0.01, .cpDRep = 5.0
};

static const ChosenParams ROSENBROCK_PARAMS = {
    .vMaxClamp = 2.0,
    .ppAlpha   = 0.1, .ppA = 1.0, .ppLambda = 2.0,
    .cpQ       = 3.0, .cpDNuc = 0.10, .cpDRep = 2.0
};

//Funcao de teste com nome, dominio e ponteiro para os hiperparams escolhidos.
typedef struct {
    const char *name;
    double (*func)(double, double);
    double dMin, dMax;
    const ChosenParams *params;
} TestFunction;

//Acumulador para agregacao em memoria (uma entrada por configuracao
//distinta de algoritmo x metodo x funcao x n).
typedef struct {
    char algoritmo[8];
    char metodo[8];
    char funcao[16];
    int  n;
    int  n_runs_kept;
    double sum_f, sum_f2;
} AggBucket;

//Escreve uma linha do CSV bruto. Mesma logica de campos vazios usada em
//sweep.c (campos nao usados pelo algoritmo ficam em branco).
static void writeRow(FILE *f, AlgoKind algo, const char *metodo,
                     const char *funcao, int seed, ConfigPSO cfg, Point r) {
    int usesInertia = strcmp(metodo, "inertia") == 0;

    const char *algoStr = (algo == ALGO_PSO) ? "PSO"
                        : (algo == ALGO_PP)  ? "PP"
                                             : "CP";

    fprintf(f, "%s,%s,%s,%d,%d,%d,",
            algoStr, metodo, funcao, seed, cfg.particles, cfg.tMax);

    if (usesInertia) fprintf(f, "%.6f,%.6f,", cfg.wMax, cfg.wMin);
    else             fprintf(f, ",,");

    fprintf(f, "%.6f,%.6f,", cfg.c1, cfg.c2);
    fprintf(f, "%.6f,", cfg.vMax);

    if (algo == ALGO_PP) {
        fprintf(f, "%.6f,%.6f,%.6f,",
                cfg.alpha, cfg.fleeAmplitude, cfg.fleeLambda);
    } else {
        fprintf(f, ",,,");
    }

    if (algo == ALGO_CP) {
        fprintf(f, "%.6f,%.6f,%.6f,", cfg.Q, cfg.dNuc, cfg.dRep);
    } else {
        fprintf(f, ",,,");
    }

    fprintf(f, "%.6f,%.6f,%.6f\n", r.value, r.x, r.y);
}

//Despacha a chamada para o algoritmo correto com base na variante.
static Point runAlgo(AlgoKind kind, const char *metodo, ConfigPSO cfg) {
    int clamp = strcmp(metodo, "clamp") == 0;
    switch (kind) {
        case ALGO_PSO: return pso(clamp ? PSO_VMAX : PSO_INERTIA, cfg);
        case ALGO_PP:  return predatorPrey(clamp ? PP_VMAX : PP_INERTIA, cfg);
        case ALGO_CP:  return chargedParticle(clamp ? CP_VMAX : CP_INERTIA, cfg);
    }
    //Inalcancavel (compilador exige um retorno).
    Point p = {0.0, 0.0, 0.0};
    return p;
}

int main(void) {
    FILE *raw = fopen("data/resultados_brutos.csv", "w");
    if (!raw) {
        perror("Nao foi possivel abrir data/resultados_brutos.csv");
        return 1;
    }

    fprintf(raw,
            "algoritmo,metodo,funcao,seed,n,t_max,"
            "w_max,w_min,c1,c2,v_max,"
            "alpha,A,lambda,Q,d_nuc,d_rep,"
            "f_final,x_final,y_final\n");

    const struct { AlgoKind kind; const char *name; } algos[] = {
        {ALGO_PSO, "PSO"}, {ALGO_PP, "PP"}, {ALGO_CP, "CP"}
    };
    const int nAlgos = sizeof(algos) / sizeof(algos[0]);

    const char *metodos[] = {"clamp", "inertia"};
    const int   nMetodos  = 2;

    TestFunction functions[] = {
        {"Rastrigin",  rastrigin,  RASTRIGIN_DOMAIN_MIN,  RASTRIGIN_DOMAIN_MAX,  &RASTRIGIN_PARAMS},
        {"Rosenbrock", rosenbrock, ROSENBROCK_DOMAIN_MIN, ROSENBROCK_DOMAIN_MAX, &ROSENBROCK_PARAMS}
    };
    const int nFuncs = sizeof(functions) / sizeof(functions[0]);

    const int ns[] = {5, 10, 25, 50};
    const int nNs  = sizeof(ns) / sizeof(ns[0]);

    const int firstSeed = 1;
    const int lastSeed  = 10;
    const int total     = nAlgos * nMetodos * nFuncs * nNs * (lastSeed - firstSeed + 1);

    AggBucket buckets[48]; //3 algos x 2 metodos x 2 funcoes x 4 valores de n.
    int nb = 0;

    int counter = 0;

    for (int a = 0; a < nAlgos; a++) {
        AlgoKind kind     = algos[a].kind;
        const char *aName = algos[a].name;

        for (int m = 0; m < nMetodos; m++) {
            const char *metodo = metodos[m];

            for (int f = 0; f < nFuncs; f++) {
                ObjectiveFunction = functions[f].func;
                const ChosenParams *p = functions[f].params;

                ConfigPSO cfg = configDefault();
                cfg.domainMin = functions[f].dMin;
                cfg.domainMax = functions[f].dMax;
                cfg.repMax    = cfg.domainMax - cfg.domainMin;
                cfg.tMax      = 200;

                //Variante clamp herda o vMax escolhido pelo PSO base na
                //varredura. Variante inertia mantem o vMax default da
                //literatura (5.0), que e o valor com que PP e CP foram
                //varridos para os hiperparametros especificos deles.
                if (strcmp(metodo, "clamp") == 0) {
                    cfg.vMax = p->vMaxClamp;
                } else {
                    cfg.vMax = 5.0;
                }

                if (kind == ALGO_PP) {
                    cfg.alpha         = p->ppAlpha;
                    cfg.fleeAmplitude = p->ppA;
                    cfg.fleeLambda    = p->ppLambda;
                }
                if (kind == ALGO_CP) {
                    cfg.Q    = p->cpQ;
                    cfg.dNuc = p->cpDNuc;
                    cfg.dRep = p->cpDRep;
                }

                for (int in = 0; in < nNs; in++) {
                    cfg.particles = ns[in];

                    //Cria bucket para esta configuracao (uma por (a,m,f,n)).
                    AggBucket *b = &buckets[nb++];
                    strcpy(b->algoritmo, aName);
                    strcpy(b->metodo,    metodo);
                    strcpy(b->funcao,    functions[f].name);
                    b->n           = ns[in];
                    b->n_runs_kept = 0;
                    b->sum_f       = 0.0;
                    b->sum_f2      = 0.0;

                    for (int seed = firstSeed; seed <= lastSeed; seed++) {
                        seedRandomWith((unsigned int)seed);
                        Point r = runAlgo(kind, metodo, cfg);
                        counter++;

                        int dropped = !isfinite(r.value);
                        if (!dropped) {
                            writeRow(raw, kind, metodo, functions[f].name,
                                     seed, cfg, r);
                            b->n_runs_kept++;
                            b->sum_f  += r.value;
                            b->sum_f2 += r.value * r.value;
                        }

                        if (dropped) {
                            fprintf(stderr,
                                    "[%03d/%d] %s %s %s n=%d seed=%d DROPPED\n",
                                    counter, total, aName, metodo,
                                    functions[f].name, ns[in], seed);
                        } else {
                            fprintf(stderr,
                                    "[%03d/%d] %s %s %s n=%d seed=%d -> f=%.6f\n",
                                    counter, total, aName, metodo,
                                    functions[f].name, ns[in], seed, r.value);
                        }
                    }
                }
            }
        }
    }

    fclose(raw);

    FILE *agg = fopen("data/resultados_agregados.csv", "w");
    if (!agg) {
        perror("Nao foi possivel abrir data/resultados_agregados.csv");
        return 1;
    }
    fprintf(agg, "algoritmo,metodo,funcao,n,n_runs_kept,mean_f,std_f\n");

    for (int i = 0; i < nb; i++) {
        AggBucket *b = &buckets[i];
        fprintf(agg, "%s,%s,%s,%d,%d,",
                b->algoritmo, b->metodo, b->funcao, b->n, b->n_runs_kept);
        if (b->n_runs_kept == 0) {
            //Configuracao toda descartada: linha sem mean/std.
            fprintf(agg, ",\n");
        } else if (b->n_runs_kept == 1) {
            //Sem desvio padrao definido (denominador N-1 = 0).
            fprintf(agg, "%.6f,\n", b->sum_f);
        } else {
            double mean = b->sum_f / (double)b->n_runs_kept;
            double var  = (b->sum_f2 - (double)b->n_runs_kept * mean * mean)
                        / (double)(b->n_runs_kept - 1);
            //Salvaguarda numerica: variancia negativa por erro de ponto
            //flutuante e tratada como zero.
            double sd   = sqrt(var > 0.0 ? var : 0.0);
            fprintf(agg, "%.6f,%.6f\n", mean, sd);
        }
    }

    fclose(agg);

    fprintf(stderr,
            "[main] completo: %d runs; %d configuracoes agregadas\n",
            counter, nb);
    return 0;
}
