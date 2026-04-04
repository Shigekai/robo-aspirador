CC     = gcc
CFLAGS = -std=c11 -Wall
SRC    = main.c vacuum.c plan.c

ifeq ($(OS),Windows_NT)
	TARGET = robo_aspirador.exe
	LIBS   =
else
	TARGET = robo_aspirador
	LIBS   = -lm
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f robo_aspirador robo_aspirador.exe
