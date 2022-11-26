CXXFLAGS += --std=gnu++2a -I ./include/
BUILDDIR = ./build
EXES = UpDetector

all: $(EXES)

clean:
	rm -f *.o $(EXES)
tidy:
	rm -f *.o

UpDetector: UpDetector.cpp ./include/*
	$(CXX) $(CXXFLAGS)  UpDetector.cpp -o $(BUILDDIR)/UpDetector
