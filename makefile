LIBS = /usr/lib64/
CFLAGS = -std=c++11
LDFLAGS = -L$(LIBS) `pkg-config --static --libs glfw3`

build: main.cpp
	g++ $(CFLAGS) -o es3_test main.cpp $(LDFLAGS)

clean:
	rm -f es3_test
