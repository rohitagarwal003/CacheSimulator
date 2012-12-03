SIM = simulator
SIM_O = main.o Memory.o Cache.o UI.o
SIM_GCH = Memory.h.gch Cache.h.gch UI.h.gch

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
	c++ -c $^

Cache.o: Cache.cpp Cache.h Memory.h
	c++ -c $^

UI.o: UI.cpp UI.h
	c++ -c $^

main.o: main.cpp Cache.h Memory.h UI.h
	c++ -c $^


$(TG): $(TG_O)
	g++ $(TG_O) -o $(TG)

clean:
	rm -f $(SIM) $(SIM_O) $(SIM_GCH) $(TG) $(TG_O)
