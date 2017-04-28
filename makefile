CFLAGS = -Wall -pedantic -Werror
LFLAGS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf
OBJS   = imgEdit.o ortho.o main.o
FOLDER = src
PROG   = OnePicOrtho_v1.1
CC    = g++ -std=c++11 -std=gnu++14

# top-level rule to create the program.
all: $(PROG)

# compiling other source files.
%.o: $(FOLDER)/%.cpp $(FOLDER)/%.h
	$(CC) $(CFLAGS) -c $<

# linking the program.
$(PROG): $(OBJS)
	$(CC) $(OBJS) -o $(PROG) $(LFLAGS)

# cleaning everything that can be automatically recreated with "make".
clean:
	rm $(PROG) *.o
