# Project: UDPChat
# Makefile created by Dev-C++ 4.9.9.2

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = main.o $(RES)
LINKOBJ  = main.o $(RES)
LIBS =  -L"d:/Dev-Cpp/lib" -mwindows libwsock32.a  
INCS =  -I"d:/Dev-Cpp/include" 
CXXINCS =  -I"d:/Dev-Cpp/lib/gcc/mingw32/3.4.2/include"  -I"d:/Dev-Cpp/include/c++/3.4.2/backward"  -I"d:/Dev-Cpp/include/c++/3.4.2/mingw32"  -I"d:/Dev-Cpp/include/c++/3.4.2"  -I"d:/Dev-Cpp/include" 
BIN  = UDPChat.exe
CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS)  
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before UDPChat.exe all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "UDPChat.exe" $(LIBS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)
