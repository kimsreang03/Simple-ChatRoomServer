exec = server
sources = $(wildcard src/server/*.c)
objects = $(sources:.c=.o)
flags = -g -Wall


$(exec): $(objects)
	cc $(objects) $(flags) -o $(exec)

%.o: %.c include/%.h
	cc -c $(flags) $< -o $@

client: 
	cc $(flags) src/client.c -o client

clean:
	@rm src/server/*.o
	