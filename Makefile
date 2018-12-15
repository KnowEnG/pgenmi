CXX = g++ $(CXXFLAGS)
CXXFLAGS = -g -std=c++11 -m64 -O -w -I include/
BIN = ./bin
OBJ = .obj/
SRC = src/

all: $(BIN)/pgenmi $(BIN)/postodds

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(OBJ)
	$(CXX) $(CXXFLAGS) $(SQLCXXFLAGS) -c $< -o $@  

$(BIN)/postodds: $(OBJ)/postodds.o $(OBJ)/pgm.o $(OBJ)/table.o $(OBJ)/helper.o
	@mkdir -p $(BIN)
	$(CXX) $(LDFLAGS) $^ $(SQLLDFLAGS) -o $@

$(BIN)/pgenmi: $(OBJ)/pgenmi.o $(OBJ)/trainer.o $(OBJ)/pgm.o $(OBJ)/table.o $(OBJ)/helper.o
	@mkdir -p $(BIN)
	$(CXX) $(LDFLAGS) $^ $(SQLLDFLAGS) -o $@
	
clean : 
	rm $(OBJ)/*.o
	rm $(BIN)/*
