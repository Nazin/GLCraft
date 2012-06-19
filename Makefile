MAIN = Project
GLTOOLSPATH = ../GLTools
INCGLTOOLSPATH = $(GLTOOLSPATH)/include
SRCGLTOOLSPATH = $(GLTOOLSPATH)/src
OBJGLTOOLSPATH = $(GLTOOLSPATH)/obj
UTILSSPATH = ../utils
INCUTILSSPATH = $(UTILSSPATH)/include
SRCUTILSSPATH = $(UTILSSPATH)/src
OBJUTILSSPATH = $(UTILSSPATH)/obj
LIBDIRS = -L/usr/X11R6/lib -L/usr/X11R6/lib64 -L/usr/local/lib
INCDIRS = -I/usr/include -I/usr/local/include -I/usr/include/GL -I$(INCGLTOOLSPATH) -I$(INCGLTOOLSPATH)/GL -I$(INCUTILSSPATH) -I.
CC = g++
CFLAGS = -Wall -g -O2 $(INCDIRS)
LIBS = -lX11 -lglut -lGLEW -lGL -lGLU -lm
OBJECTS = $(OBJGLTOOLSPATH)/math3d.o $(OBJGLTOOLSPATH)/GLTools.o $(OBJGLTOOLSPATH)/GLBatch.o $(OBJGLTOOLSPATH)/GLTriangleBatch.o $(OBJGLTOOLSPATH)/GLShaderManager.o $(OBJUTILSSPATH)/shaders.o $(OBJUTILSSPATH)/obj.o Camera.o InputManager.o BlockManager.o ShaderManager.o

$(MAIN): $(OBJECTS) $(MAIN).cpp
	@echo Compilation of $(MAIN).cpp...
	$(CC) $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) $(OBJECTS) $(MAIN).cpp -o $(MAIN)
	@echo Compilation $(MAIN).cpp complete

Camera.o: Camera.cpp Camera.h
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) Camera.cpp -o Camera.o
	@echo Compilation Camera.cpp complete
InputManager.o: InputManager.cpp Managers.h
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) InputManager.cpp -o InputManager.o
	@echo Compilation InputManager.cpp complete
BlockManager.o: BlockManager.cpp Managers.h
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) BlockManager.cpp -o BlockManager.o
	@echo Compilation InputManager.cpp complete
ShaderManager.o: ShaderManager.cpp Managers.h
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) ShaderManager.cpp -o ShaderManager.o
	@echo Compilation InputManager.cpp complete
$(OBJGLTOOLSPATH)/math3d.o: $(SRCGLTOOLSPATH)/math3d.cpp
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) $(SRCGLTOOLSPATH)/math3d.cpp -o $(OBJGLTOOLSPATH)/math3d.o
	@echo Compilation math3d.c complete
$(OBJGLTOOLSPATH)/GLTools.o: $(SRCGLTOOLSPATH)/GLTools.cpp
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) $(SRCGLTOOLSPATH)/GLTools.cpp -o $(OBJGLTOOLSPATH)/GLTools.o
	@echo Compilation GLTools.cpp complete
$(OBJGLTOOLSPATH)/GLBatch.o: $(SRCGLTOOLSPATH)/GLBatch.cpp
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) $(SRCGLTOOLSPATH)/GLBatch.cpp -o $(OBJGLTOOLSPATH)/GLBatch.o
	@echo Compilation GLBatch.cpp complete
$(OBJGLTOOLSPATH)/GLTriangleBatch.o: $(SRCGLTOOLSPATH)/GLTriangleBatch.cpp
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) $(SRCGLTOOLSPATH)/GLTriangleBatch.cpp -o $(OBJGLTOOLSPATH)/GLTriangleBatch.o
	@echo Compilation GLTriangleBatch.cpp complete
$(OBJGLTOOLSPATH)/GLShaderManager.o: $(SRCGLTOOLSPATH)/GLShaderManager.cpp
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) $(SRCGLTOOLSPATH)/GLShaderManager.cpp -o $(OBJGLTOOLSPATH)/GLShaderManager.o
	@echo Compilation GLShaderManager.cpp complete
$(OBJUTILSSPATH)/shaders.o: $(SRCUTILSSPATH)/shaders.cpp
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) $(SRCUTILSSPATH)/shaders.cpp -o $(OBJUTILSSPATH)/shaders.o
	@echo Compilation shaders.cpp complete
$(OBJUTILSSPATH)/obj.o: $(SRCUTILSSPATH)/obj.cpp
	$(CC) -O2 -g -c $(CFLAGS) $(INCDIRS) $(LIBDIRS) $(LIBS) $(SRCUTILSSPATH)/obj.cpp -o $(OBJUTILSSPATH)/obj.o
	@echo Compilation utils.cpp complete

clean:
	rm -f $(MAIN) $(OBJECTS)
