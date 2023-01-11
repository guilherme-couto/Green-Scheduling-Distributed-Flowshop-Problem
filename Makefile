CXX				:= g++
INCLUDE_DIRS	:= -I./include
CXX_FLAGS		:= -g -std=c++2a -O3 $(INCLUDE_DIRS)

BIN		:= bin
SRC		:= source
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:=
EXECUTABLE	:= green

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

init:
	mkdir -p $(BIN)
	mkdir -p $(SRC)
	mkdir -p $(INCLUDE)
	mkdir -p $(LIB)

green: $(BIN)/$(EXECUTABLE)

run: green
	@echo "\n\nExecutando aplicação...\n\n"
	$(BIN)/$(EXECUTABLE)
	@echo "\n\nEncerrando aplicação...\n\n"

clean:
	@echo "\nRemovendo executavel...\n"
	rm $(BIN)/$(EXECUTABLE)
	@echo "\nRemovido com sucesso...\n"
