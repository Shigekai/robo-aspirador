# Trabalho 01 — PSO, Predador-Presa e Charged Particle

Implementação dos três algoritmos de otimização por enxame aplicados às
funções **Rastrigin 2D** e **Rosenbrock 2D**, com varredura de
hiperparâmetros e coleta de resultados em CSV.

## Algoritmos

- **PSO base**: enxame canônico com duas variantes de controle de
  velocidade — `clamp` (vMax) e `inertia` (peso `W` decrescente
  linearmente de `wMax = 0.9` para `wMin = 0.4`).
- **Predador-Presa**: presas otimizam via PSO + termo exponencial de fuga
  do predador, que persegue o `gbest`. Mesmas duas variantes.
- **Charged Particle Swarm Optimization (CPSO)**: PSO + repulsão de
  Coulomb par-a-par com gating por raio nuclear (`dNuc`) e raio de
  repulsão (`dRep`). Duas variantes; a componente PSO recebe o
  controle de velocidade (clamp/inércia), enquanto a repulsão tem um
  limitador defensivo `repMax` (default = largura do domínio).

## Estrutura

```
trabalho-01/
├── main.c                # protocolo final (480 runs)
├── sweep.c               # varredura de hiperparametros (288 runs)
├── pso.{h,c}             # PSO base + ConfigPSO compartilhado
├── predator_prey.{h,c}   # modelo predador-presa
├── charged_particle.{h,c}# CPSO com repulsao de Coulomb
├── objectiveFunction.{h,c}# Rastrigin e Rosenbrock 2D
├── common.{h,c}          # utilitarios compartilhados (RNG, clamp)
├── Makefile
├── SPEC-trabalho-01.md   # especificacao versionada
├── data/                 # CSVs gerados por main e sweep
└── docs/
    └── varredura-hiperparametros.md  # escolhas justificadas
```

## Como compilar e rodar

Pré-requisito: GCC ou clang com suporte a C11.

```bash
cd trabalho-01

# Protocolo final (480 runs, hiperparametros ja escolhidos):
make
./main

# Varredura exploratória (288 runs, opcional):
make sweep
./sweep

# Limpa os binários:
make clean
```

`./main` escreve em `data/resultados_brutos.csv` (uma linha por execução)
e `data/resultados_agregados.csv` (uma linha por configuração com média
e desvio padrão sobre as 10 sementes). `./sweep` escreve em
`data/varredura_brutos.csv`. Os três CSVs são versionáveis. Progresso é
impresso em `stderr`.

## Saída

### `resultados_brutos.csv`

Colunas: `algoritmo, metodo, funcao, seed, n, t_max, w_max, w_min, c1,
c2, v_max, alpha, A, lambda, Q, d_nuc, d_rep, f_final, x_final,
y_final`. Hiperparâmetros não usados pelo algoritmo da linha ficam em
branco.

### `resultados_agregados.csv`

Colunas: `algoritmo, metodo, funcao, n, n_runs_kept, mean_f, std_f`.
`n_runs_kept` é o número de sementes (de 10) que produziram resultado
finito. Linhas com `n_runs_kept = 0` ficam com `mean_f` e `std_f`
vazios; com `n_runs_kept = 1` ficam com `mean_f` preenchido e `std_f`
vazio (denominador `N − 1 = 0`). `std_f` é desvio padrão amostral.

### `varredura_brutos.csv`

Mesmo formato de `resultados_brutos.csv`. Cobre 288 execuções do sweep
com `N = 30`, `tMax = 200`, sementes `{1, 2, 3}`.

## Reprodutibilidade

Cada execução é precedida por `seedRandomWith(seed)` chamado
imediatamente antes da chamada do algoritmo, dentro do laço mais interno.
Logo, qualquer tupla `(algoritmo, metodo, funcao, hiperparams, n, seed)`
é reprodutível bit-a-bit em re-execuções no mesmo compilador.

## Documentação adicional

- [`docs/varredura-hiperparametros.md`](docs/varredura-hiperparametros.md):
  resultados da varredura, critério de escolha (média de `f_final` →
  desvio padrão se gap `< 5%` → ordem natural da tupla) e justificativa
  por algoritmo × função.
- [`SPEC-trabalho-01.md`](SPEC-trabalho-01.md): especificação completa
  do trabalho (Stage 1 PSO+PP, Stage 2 CPSO, Stage 3 sweep + coleta).
