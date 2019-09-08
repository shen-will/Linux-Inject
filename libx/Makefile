
libx:$(wildcard *.c) $(wildcard *.S)  $(wildcard *.h)
	gcc -nostdlib -e xboot *.c *.S -o libx


install:
	cp libx ../libx.m

clean:
	rm libx
