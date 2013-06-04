HDR = $(wildcard incl/*.hpp)
SRC = $(wildcard src/*.cpp)
OBJ = ${SRC:.cpp=.o}
LINK = 
EXE = runme.exe
CPP = g++
CPPFLAGS = -O3 -g -Iincl/
STRIP = strip
.PHONY = clean export clean-all
TEMP = $(shell ls | grep -v "makefile")

all: ${EXE}

$(EXE): ${OBJ}
		${CPP} ${CPPFLAGS} ${OBJ} ${LINK} -o ${EXE}
		${STRIP} ${EXE}

Floyd-Warshall.o : makefile Floyd-Warshall.cpp Floyd-Warshall.hpp
		${CPP} ${CPPFLAGS} ${LINK} -c Floyd-Warshall.cpp -o Floyd-Warshall.o

GraphIR.o : makefile GraphIR.cpp GraphIR.hpp
		${CPP} ${CPPFLAGS} ${LINK} -c GraphIR.cpp -o GraphIR.o

RandomGraph.o : makefile RandomGraph.cpp RandomGraph.hpp
		${CPP} ${CPPFLAGS} ${LINK} -c RandomGraph.cpp -o RandomGraph.o

main.o : makefile Floyd-Warshall.cpp GraphIR.cpp RandomGraph.cpp
		${CPP} ${CPPFLAGS} -c main.cpp -o main.o

# Make modifiers
clean:
		rm -rf ${OBJ} ${EXE}
		-rm ${TEMP} #'-' at the start of the line forces make to ignore errors from this command

clean-all:
		rm -rf ${OBJ} ${EXE} input/* output/*

export:
		rm -rf ${OBJ} ${EXE}
		tar --exclude=*git* --exclude=*input* --exclude=*output* -czvf SA-Dependencies.tar.gz ../SA-Dependencies/*
