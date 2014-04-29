CXX      = g++
CXXFLAGS = -Wall -ansi -g
CC       = gcc
CCFLAGS  = -g
OBJS     = src/main.o src/processInWindows.o src/DecisionTreeClass.o src/aweighting.o src/featureExtraction.o src/kiss_fft130/kiss_fft.o

tester : $(OBJS)
	$(CXX) -o $@ $(OBJS)

OBJ/%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $<

OBJ/%.o : %.c
	$(CC) -c $(CCFLAGS) $<
