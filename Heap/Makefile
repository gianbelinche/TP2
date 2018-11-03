CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Wbad-function-cast -Wshadow -Wpointer-arith -Wunreachable-code -Wformat=2 -Werror
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2

CFILES = *.c
HFILES = *.h
EXEC_S = pruebas
PADRON = G47

build: $(CFILES)
	$(CC) $(CFLAGS) -o $(EXEC_S) $(CFILES)

run: build
	./$(EXEC_S)

valgrind: build
	valgrind $(VFLAGS) ./$(EXEC_S)

gdb: build
	gdb $(GDBFLAGS) ./$(EXEC_S)

test: build
	$(MAKE) run; test $$? -eq 0
	$(MAKE) valgrind; test $$? -eq 0
	$(MAKE) clean

runtime: build
	time ./$(EXEC_S)

clean:
	rm -f *.o $(EXEC_S)

zip:
	zip $(PADRON).zip $(CFILES) $(HFILES)
