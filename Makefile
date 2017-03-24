LDFLAGS = -lpthread -lglfw -lGL -lsporth -lsoundpipe -ldl -lm -lrunt
CFLAGS = -Wall -ansi
default: tracker3.so

OBJ = draw.o tracker3.o ugen.o runt.o

default: tracker3.so

%.o: %.c bitmaps.h tracker3.h
	$(CC) $(CFLAGS) -c -fPIC -shared $< -o $@

tracker3.so: $(OBJ)
	$(CC) $(CFLAGS) -fPIC -shared $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -rf tracker3.so $(OBJ)
