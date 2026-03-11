# Robô Aspirador

Simulação de um agente inteligente de limpeza em C.

## Como rodar

**CLion:** Basta abrir a pasta do projeto e clicar em **Run**.

**Terminal** (pré-requisitos: CMake >= 3.20 e GCC/Clang):

```bash
mkdir build && cd build
cmake ..
cmake --build .
./robo_aspirador
```

O programa pedirá as dimensões do ambiente e a quantidade de sujeiras.

## Legenda

| Símbolo | Significado |
|---------|-------------|
| `o`     | Agente      |
| `C`     | Carregador  |
| `x`     | Sujeira     |
| `-`     | Lugar limpo |

