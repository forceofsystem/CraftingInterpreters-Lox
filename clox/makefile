obj = main.o chunk.o debug.o memory.o value.o

clox : $(obj)
	$(CC) -o clox $(obj) -g

main.o : main.c
	$(CC) -c main.c -g

chunk.o : chunk.c
	$(CC) -c chunk.c -g

debug.o : debug.c
	$(CC) -c debug.c -g

memory.o : memory.c
	$(CC) -c memory.c -g

value.o : value.c
	$(CC) -c value.c -g

run : clox
	./clox

.PHONY : clean
clean :
	rm clox $(obj)
