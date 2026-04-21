# Trabalho 01 — PSO + Predador-Presa

Implementação do PSO e do modelo Predador-Presa aplicados às funções Rastrigin 2D e Rosenbrock 2D.

## Algoritmos

- **PSO com vmax:** controle de velocidade por clamp.
- **PSO com inércia W:** peso `W` decresce linearmente de `wmax = 0.9` até `wmin = 0.4` ao longo de `tmax` iterações.
- **Predador-Presa com vmax:** presas usam PSO + termo de fuga, velocidade limitada por clamp.
- **Predador-Presa com inércia W:** presas usam PSO + termo de fuga, com peso de inércia decrescente.

## Como compilar e rodar

**Com Make** (pré-requisito: GCC):

```bash
cd trabalho-01
make
./main
```

**Sem Make** (compilação direta):

```bash
cd trabalho-01
cc main.c common.c pso.c predator_prey.c objectiveFunction.c -o main -lm
./main
```
