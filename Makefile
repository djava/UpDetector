CXXFLAGS += --std=gnu++2a -I ./include/
BUILDDIR = ./build
EXES = isUpServer

all: $(EXES)

clean:
	rm -f *.o $(EXES)
tidy:
	rm -f *.o

isUpServer: isUpServer.cpp ./include/*
	$(CXX) $(CXXFLAGS)  isUpServer.cpp -o $(BUILDDIR)/isUpServer
