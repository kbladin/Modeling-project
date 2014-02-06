EXEC = program
OBJS = $(wildcard objects/*)
SOURCE = $(wildcard src/*)
CC = clang++

#The glfw lib
GLFWINCLUDE = -I"/usr/local/Cellar/glfw3/3.0.3/include"
GLFWLIB = -L"/usr/local/Cellar/glfw3/3.0.3/lib"
GLFW = $(GLFWINCLUDE) $(GLFWLIB) -lglfw3

#The glew lib
GLEWINCLUDE = -I"/usr/local/Cellar/glew/1.10.0/include"
GLEWLIB = -L"/usr/local/Cellar/glew/1.10.0/lib"
GLEW = $(GLEWINCLUDE) $(GLEWLIB) -lglew

CFLAGS =
INCLUDE = -I"include"
LDFLAGS =


FRAMEWORKS = $(GLFW) $(GLEW) -framework Opengl -framework AGL -framework Cocoa -framework IOKit


$(EXEC): objects
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(INCLUDE) $(FRAMEWORKS)

objects: $(SOURCE)
	mkdir objects; \
	clang++ -c $(SOURCE) $(INCLUDE) $(GLFWINCLUDE) $(GLEWINCLUDE); \
	mv *.o objects/; 

run:
	./program

clean:
	rm -rf objects/; \
	rm $(EXEC)