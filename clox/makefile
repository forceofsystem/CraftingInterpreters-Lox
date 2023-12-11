obj = main.o chunk.o debug.o memory.o value.o vm.o scanner.o compiler.o object.o table.o

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

vm.o : vm.c
	$(CC) -c vm.c -g

scanner.o : scanner.c
	$(CC) -c scanner.c -g

compiler.o : compiler.c
	$(CC) -c compiler.c -g

object.o : object.c
	$(CC) -c object.c -g

table.o : table.c
	$(CC) -c table.c -g

run : clox
	./clox test.lox

.PHONY : clean
clean :
	rm clox $(obj)
