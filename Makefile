CXX = g++ $(CXXFLAGS)
CXXFLAGS = -g -std=c++11 -m64 -O -w -I include/
BIN = ./bin
OBJ = .obj/
SRC = src/

all: $(BIN)/pgenmi

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) $(SQLCXXFLAGS) -c $< -o $@  

$(BIN)/pgenmi: $(OBJ)/pgenmi.o $(OBJ)/trainer.o $(OBJ)/pgm.o $(OBJ)/table.o $(OBJ)/helper.o
	$(CXX) $(LDFLAGS) $^ $(SQLLDFLAGS) -o $@
	
clean : 
	rm $(OBJ)/*.o
	rm $(BIN)/*
