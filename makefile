all:target

target: main.o writer.o reader.o listID.o
	gcc -o client main.o listID.o -g
	gcc -o writer writer.o
	gcc -o reader reader.o listID.o -g

main.o: main.c
	gcc -c main.c

writer.o: writer.c
	gcc -c writer.c

reader.o: reader.c
	gcc -c reader.c

listID.o: listID.c
	gcc -c listID.c

clean:
	rm -rf *.o common/*.id common/*.fifo mirror client writer reader log_file
