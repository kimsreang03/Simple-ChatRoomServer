exec = server
sources = $(wildcard src/server/*.c)
objects = $(sources:.c=.o)
flags = -g -Wall


$(exec): $(objects)
	cc $(objects) $(flags) -o $(exec)

%.o: %.c include/%.h
	cc -c $(flags) $< -o $@


clean:
	@rm src/server/*.o
	@rm src/*.o
	