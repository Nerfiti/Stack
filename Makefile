TARGET = stack.exe
CC = g++ -c
LINKER = g++
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))

$(TARGET) : $(OBJ)
	$(LINKER) $(OBJ) -o $(TARGET)

obj : $(SRC)
	$(CC) $(SRC)

clean :
	rm *.o