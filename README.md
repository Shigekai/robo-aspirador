# Robô Aspirador

Simulação de um agente inteligente de limpeza em C.

## Como rodar

**CLion:** Basta abrir a pasta do projeto e clicar em **Run**.

**Terminal com Make** (pré-requisito: GCC):

```bash
make
./robo_aspirador        # Linux/macOS
.\robo_aspirador.exe    # Windows
```

**Terminal com CMake** (pré-requisitos: CMake >= 3.20 e GCC/Clang):

```bash
mkdir build && cd build
cmake ..
cmake --build .
./robo_aspirador        # Linux/macOS
.\robo_aspirador.exe    # Windows
```

O programa pedirá as dimensões do ambiente e a quantidade de sujeiras.

## Legenda

| Símbolo | Significado |
|---------|-------------|
| `o`     | Agente      |
| `C`     | Carregador  |
| `x`     | Sujeira     |
| `-`     | Lugar limpo |

## Outras tarefas

- [Tarefa 03](tarefa-03/) — Simulated Annealing e Stochastic Local Beam para a função Rastrigin 2D
