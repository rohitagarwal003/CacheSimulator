SIM = simulator
SIM_O = main.o Memory.o Cache.o UI.o

TG = tracegenerator
TG_O = TraceGenerator.o

# This is the first rule (the default)
# Executed when either 'make' or 'make all' is typed.
# It will build both the simulator and the tracegenerator.
all: $(SIM) $(TG)

# The simulator depends on the four .o files
# After the object files are built, the indented line is executed.
$(SIM): $(SIM_O)
	g++ $(SIM_O) -o $(SIM)

Memory.o: Memory.cpp Memory.h
	c++ -c -o $@ Memory.cpp

# Cache.cpp uses code from Memory.cpp by means of Memory.h
# I just need to add Memory.h and not Memory.cpp
# This is because Cache.o needs to be recompiled only when Memory.h changes and not when Memory.cpp changes.
# When Memory.cpp changes, Memory.o will be recompiled and the linker will generate a new executable.
Cache.o: Cache.cpp Cache.h Memory.h
	c++ -c -o $@ Cache.cpp

UI.o: UI.cpp UI.h
	c++ -c -o $@ UI.cpp

main.o: main.cpp Cache.h Memory.h UI.h
	c++ -c -o $@ main.cpp


$(TG): $(TG_O)
	g++ $(TG_O) -o $(TG)

clean:
	rm -f $(SIM) $(SIM_O) $(TG) $(TG_O)
