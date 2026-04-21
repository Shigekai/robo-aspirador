# Tarefa 04 — Otimização por Enxame de Partículas (PSO)

Implementação de duas variantes do PSO aplicadas à função Rastrigin 2D:

**f(x, y) = 20 + x² - 10·cos(2πx) + y² - 10·cos(2πy)**

Mínimo global: f(0, 0) = 0

## Variantes

- **PSO com vmax:** atualização clássica `v_i(t+1) = v_i(t) + c1⊙(x*_i - x_i) + c2⊙(x_opt - x_i)`. Após calcular a velocidade, se `||v_i|| > vmax` ela é reescalada para `vmax · v_i / ||v_i||`, evitando passos explosivos.
- **PSO com inércia W:** mesma lógica (incluindo o clamp de `vmax`), mas a atualização da velocidade passa a ser `v_i(t+1) = W·v_i(t) + c1⊙(x*_i - x_i) + c2⊙(x_opt - x_i)`. O peso `W` decresce linearmente de `wmax = 0.9` até `wmin = 0.4` ao longo de `tmax` iterações, balanceando exploração e explotação.

O núcleo das duas variantes está em `pso.c`, parametrizado por um enum `PsoVariant`. Parâmetros (`n`, `tmax`, `c1`, `c2`, `vmax`, `wmin`, `wmax`) são configuráveis via `PsoConfig` em `main.c`.

O programa imprime `f(x_opt)` a cada iteração para cada variante e cada tamanho de enxame testado (`N = 5, 10, 25, 50`), permitindo comparar as curvas de convergência.

## Como compilar e rodar

**Com Make** (pré-requisito: GCC):

```bash
cd tarefa-04
make
./main
```

**Sem Make** (compilação direta):

```bash
cd tarefa-04
cc main.c common.c pso.c objectiveFunction.c -o main -lm
./main
```

Para salvar a saída e plotar as curvas depois:

```bash
./main > convergencia.txt
```
