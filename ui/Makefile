all: ui clean

ui: 
		gcc -Wall `pkg-config --cflags gtk+-3.0` ui.c -o ui `pkg-config --libs gtk+-3.0`

clean:
	rm -rf *.o