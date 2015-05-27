CC=gcc
CFLAGS=-I. -O0
DEPS = precise_timer.h
OBJ = timer_test.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

timer_test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
	
clean: 
	rm -f *.o timer_test 
