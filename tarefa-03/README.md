# Tarefa 03 — Simulated Annealing e Busca em Feixe Local Estocástica

Implementação de dois algoritmos de otimização aplicados à função Rastrigin 2D:

**f(x, y) = 20 + x² - 10·cos(2πx) + y² - 10·cos(2πy)**

Mínimo global: f(0, 0) = 0

## Algoritmos

- **Simulated Annealing:** busca local com aceitação probabilística de soluções piores, controlada por temperatura decrescente (schedule geométrico).
- **Stochastic Beam Search:** mantém k estados em paralelo, gera vizinhos para cada um e seleciona os próximos k estados estocasticamente a partir dos melhores candidatos.

## Como compilar e rodar

**Com Make** (pré-requisito: GCC):

```bash
cd tarefa-03
make
./main
```

**Sem Make** (compilação direta):

```bash
cd tarefa-03
cc main.c common.c anneling.c beam.c objectiveFunction.c -o main -lm
./main
```

