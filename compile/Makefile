# Macros

CC = gcc
CFLAGS = -g
LIBS = -lJudy -lm
INCLUDE = -I../h
OBJ=main.o nbc.o helper.o

nbc: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o nbc $(OBJ) $(LIBS)

main.o: ./../sys/main.c ../h/nbc.h ../h/helper.h
	$(CC) $(CFLAGS) $(INCLUDE) -c ../sys/main.c

nbc.o: ./../sys/nbc.c ../h/nbc.h
	$(CC) $(CFLAGS) $(INCLUDE) -c ../sys/nbc.c

helper.o: ./../sys/helper.c ../h/helper.h
	$(CC) $(CFLAGS) $(INCLUDE) -c ../sys/helper.c
