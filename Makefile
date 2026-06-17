CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm
EXEC = scolarite

all: $(EXEC)

$(EXEC): main.c
	$(CC) $(CFLAGS) main.c -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(EXEC) etudiants.dat