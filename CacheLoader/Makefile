CC=gcc
CFLAGS=-I. -O0 -lm
DEPS = cache.h
OBJ = cache_test.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cache_test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
	
clean: 
	rm -f *.o cache_test
