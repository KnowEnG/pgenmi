CXX = g++ $(CXXFLAGS)
CXXFLAGS = -g -std=c++11 -m64 -O -w -I include/
BIN = ./bin
OBJ = .obj
SRC = src

all: $(BIN)/dbeta $(BIN)/pgmeval $(BIN)/pgmllr $(BIN)/post $(BIN)/pSweep

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) $(SQLCXXFLAGS) -c $< -o $@  

$(BIN)/dbeta : $(OBJ)/dbeta.o $(OBJ)/helper.o
	$(CXX) $(LDFLAGS) $^ $(SQLLDFLAGS) -o $@

$(BIN)/post : $(OBJ)/post.o $(OBJ)/table.o $(OBJ)/pgm.o $(OBJ)/helper.o
	$(CXX) $(LDFLAGS) $^ $(SQLLDFLAGS) -o $@

$(BIN)/pgmeval : $(OBJ)/pgmeval.o $(OBJ)/table.o $(OBJ)/pgm.o $(OBJ)/helper.o
	$(CXX) $(LDFLAGS) $^ $(SQLLDFLAGS) -o $@

$(BIN)/pgmllr : $(OBJ)/pgmllr.o $(OBJ)/table.o $(OBJ)/pgm.o $(OBJ)/helper.o
	$(CXX) $(LDFLAGS) $^ $(SQLLDFLAGS) -o $@

$(BIN)/pSweep : $(OBJ)/pSweep.o $(OBJ)/table.o $(OBJ)/pgm.o $(OBJ)/helper.o
	$(CXX) $(LDFLAGS) $^ $(SQLLDFLAGS) -o $@

clean : 
	rm .obj/*.o
	rm .bin/*.o
