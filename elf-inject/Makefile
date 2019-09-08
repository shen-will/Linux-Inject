

files=$(wildcard *.c)

objs=$(files:.c=.o)

inject:$(objs)
	gcc $(objs) -o inject

%.o:%.c
	gcc -c $^ -o $@

install:
	cp inject ../inject

clean:
	rm *.o inject
