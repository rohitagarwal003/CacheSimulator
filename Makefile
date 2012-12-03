SIM = simulator
SIM_O = main.o Memory.o Cache.o UI.o

TG = tracegenerator
TG_O = TraceGenerator.o

# This is the first rule (the default)
# Executed when either 'make' or 'make all' is typed.
# It will build both the simulator and the tracegenerator.
all: $(SIM) $(TG)

# The simulator depends on the four .o files
# I have not specified the rules for .o files,
# the default rules will be used for them.
# That's why I named the object files after the source files.
# After the object files are built, the indented line is executed.
$(SIM): $(SIM_O)
	g++ $(SIM_O) -o $(SIM)


$(TG): $(TG_O)
	g++ $(TG_O) -o $(TG)

clean:
	rm -f $(SIM) $(SIM_O) $(TG) $(TG_O)
