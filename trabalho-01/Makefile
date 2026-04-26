CC = cc
CFLAGS = -Wall -std=c11
LDFLAGS = -lm
LIB_SRC = common.c pso.c predator_prey.c charged_particle.c objectiveFunction.c

main: main.c $(LIB_SRC)
	$(CC) $(CFLAGS) main.c $(LIB_SRC) -o main $(LDFLAGS)

sweep: sweep.c $(LIB_SRC)
	$(CC) $(CFLAGS) sweep.c $(LIB_SRC) -o sweep $(LDFLAGS)

clean:
	rm -f main sweep
