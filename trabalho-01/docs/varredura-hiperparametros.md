# Varredura de Hiperparâmetros — Stage 3a

Este documento registra o resultado da varredura exploratória de
hiperparâmetros executada por `./sweep`. A varredura cobriu **288 execuções**
(3 algoritmos × grade própria × 2 funções de custo × 3 sementes), com
método de controle de velocidade fixo em **inércia** para PP e CP, e em
**clamp** para a varredura específica do PSO base. Sementes `{1, 2, 3}`,
`N = 30`, `tMax = 200`. Todos os 288 runs produziram resultados finitos —
**zero linhas descartadas** (regra: NaN/Inf seriam excluídos).

CSV bruto: [`data/varredura_brutos.csv`](../data/varredura_brutos.csv).

## Critério de escolha

1. **Menor média** de `f_final` sobre as 3 sementes (primário).
2. Se a diferença entre médias for `< 5%`, **menor desvio padrão** vence.
3. Em caso de empate residual, **ordem natural numérica** da tupla de
   hiperparâmetros.

Em todos os casos abaixo, o gap entre o vencedor e o segundo colocado foi
**> 5%**, então o tiebreak por desvio padrão não foi acionado.

## Salvaguarda numérica do CPSO

A varredura ativa o limitador `repMax = domainMax − domainMin` sobre a
norma do vetor de repulsão de Coulomb antes da soma com a componente PSO.
É um teto defensivo (uma travessia de domínio por iteração) que só
dispara em configurações patológicas — combinações como `Q=5, dNuc=0.01`
não geraram NaN/Inf e o limite raramente entra em ação. Mantido para
robustez.

## Resultados por (algoritmo × função)

### PSO — `clamp` — Rastrigin

| `v_max` | mean f | std f |
|---------|--------|-------|
| **2.0** | **0.006145** | **0.004114** |
| 5.0 | 0.053176 | 0.071408 |
| 10.0 | 0.804016 | 0.576372 |

**Escolha: `v_max = 2.0`.** O valor `2.0` ganha com folga (~9× melhor que
o segundo). Em Rastrigin, com várias bacias locais e domínio pequeno
(`[-5.12, 5.12]`), velocidades altas fazem partículas saltar entre vales
e piorar a convergência fina.

### PSO — `clamp` — Rosenbrock

| `v_max` | mean f | std f |
|---------|--------|-------|
| **2.0** | **0.001570** | **0.001824** |
| 5.0 | 0.005923 | 0.008851 |
| 10.0 | 0.045285 | 0.040123 |

**Escolha: `v_max = 2.0`.** Mesma direção: velocidades menores favorecem
convergência ao longo do vale curvo da Rosenbrock. `v_max = 2` ganha em
ambas funções, então este valor também é o `vMax` herdado pelas variantes
clamp de PP e CP no protocolo final (Stage 3b).

### Predator-Prey — `inertia` — Rastrigin

Top 3 (de 27 combinações):

| `α` | `A` | `λ` | mean f | std f |
|-----|-----|-----|--------|-------|
| **1.0** | **1.0** | **2.0** | **0.006890** | **0.005729** |
| 0.1 | 1.0 | 1.0 | 0.007674 | 0.006740 |
| 0.5 | 1.0 | 2.0 | 0.007679 | 0.008538 |

**Escolha: `α = 1.0, A = 1.0, λ = 2.0`.** Padrão claro: as 6 melhores
combinações têm `A = 1.0` (a menor amplitude de fuga); amplitude maior
empurra demais as presas e estraga a convergência. `λ = 2` (decaimento
mais agressivo da fuga com distância) torna o efeito do predador local —
útil em uma paisagem multimodal como Rastrigin. `α = 1.0` (predador rápido)
permite que ele alcance o `gbest` cedo, a fuga decai logo, e o enxame
volta a otimizar.

### Predator-Prey — `inertia` — Rosenbrock

Top 3:

| `α` | `A` | `λ` | mean f | std f |
|-----|-----|-----|--------|-------|
| **0.1** | **1.0** | **2.0** | **0.000263** | **0.000183** |
| 1.0 | 1.0 | 1.0 | 0.000426 | 0.000191 |
| 0.5 | 1.0 | 2.0 | 0.000474 | 0.000271 |

**Escolha: `α = 0.1, A = 1.0, λ = 2.0`.** Em Rosenbrock (vale longo e
curvo), um predador lento (`α = 0.1`) gera perturbações mais suaves que
não atrapalham a descida fina ao longo do vale. `A = 1.0` e `λ = 2.0`
seguem a mesma lógica de Rastrigin: fuga pequena e local.

### Charged Particle — `inertia` — Rastrigin

Top 3 (de 18 combinações):

| `Q` | `dNuc` | `dRep` | mean f | std f |
|-----|--------|--------|--------|-------|
| **1.0** | **0.01** | **5.0** | **0.337899** | **0.230878** |
| 1.0 | 0.10 | 10.0 | 0.388761 | 0.474671 |
| 1.0 | 0.01 | 10.0 | 0.400564 | 0.427697 |

**Escolha: `Q = 1.0, dNuc = 0.01, dRep = 5.0`.** As 6 melhores
combinações têm `Q = 1.0` (a menor carga). Cargas maiores produzem
repulsão excessiva e o enxame não se adensa o suficiente em torno do
ótimo. `dRep = 5` cobre quase todo o domínio Rastrigin (lado ≈ 10.24)
sem permitir interações distantes, e `dNuc = 0.01` deixa partículas se
aproximarem bem antes de cortar a repulsão.

Observação: o desempenho do CP em Rastrigin (mean ≈ 0.34) é pior por
ordem de grandeza que PSO (≈ 0.006) e PP (≈ 0.007). A repulsão atrapalha
a convergência fina nesta paisagem multimodal — esperado.

### Charged Particle — `inertia` — Rosenbrock

Top 3:

| `Q` | `dNuc` | `dRep` | mean f | std f |
|-----|--------|--------|--------|-------|
| **3.0** | **0.10** | **2.0** | **0.011092** | **0.010163** |
| 1.0 | 0.10 | 5.0 | 0.015360 | 0.010998 |
| 1.0 | 0.01 | 10.0 | 0.020431 | 0.002374 |

**Escolha: `Q = 3.0, dNuc = 0.10, dRep = 2.0`.** Diferente da Rastrigin:
a repulsão moderada (`Q = 3`) com curto alcance (`dRep = 2`) e dNuc não
tão pequeno mantém o enxame coeso ao longo do vale, evitando que
partículas se atropelem. Em Rosenbrock o ótimo é único e a diversidade
agressiva oferecida por `Q` pequeno + `dRep` largo prejudica.

## Resumo: hiperparâmetros escolhidos

A struct abaixo é a fonte oficial dos valores aplicados no protocolo
final (Stage 3b), espelhada em `main.c`:

```c
typedef struct {
    double vMaxClamp;          //PSO base (clamp); herdado por PP/CP variante clamp
    double ppAlpha, ppA, ppLambda;
    double cpQ, cpDNuc, cpDRep;
} ChosenParams;

static const ChosenParams RASTRIGIN = {
    .vMaxClamp = 2.0,
    .ppAlpha   = 1.0, .ppA = 1.0, .ppLambda = 2.0,
    .cpQ       = 1.0, .cpDNuc = 0.01, .cpDRep = 5.0
};

static const ChosenParams ROSENBROCK = {
    .vMaxClamp = 2.0,
    .ppAlpha   = 0.1, .ppA = 1.0, .ppLambda = 2.0,
    .cpQ       = 3.0, .cpDNuc = 0.10, .cpDRep = 2.0
};
```

Hiperparâmetros não varridos (literatura): `wMax = 0.9`, `wMin = 0.4`,
`c1 = c2 = 2.0`. Para PP e CP em variante clamp no protocolo final, o
`vMax` aplicado é o escolhido pelo PSO base (`2.0`) — vale para ambas
as funções.
