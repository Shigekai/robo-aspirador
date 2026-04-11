CC = cc
CFLAGS = -Wall -std=c11
LDFLAGS = -lm
SRC = main.c common.c pso.c objectiveFunction.c
TARGET = main

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
