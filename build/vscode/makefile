#source file search paths
VPATH = ../../src 

# compiler include search paths
INCS = \
	-I../../../boost/boost1_72

DEPS = walker.h

# folder for .o files
ODIR=./obj

# sources
_OBJ = LazyHillWalker.o walker.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.cpp $(DEPS)
	g++   -std=c++17 -c -o $@ $< $(INCS)

walker: $(OBJ) 
	g++ -o ../../bin/walker.exe $^ 


.PHONY: clean

clean:
	del /q obj