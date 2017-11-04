VULKAN_INCLUDES = /usr/include/vulkan/
VULKAN_LIBS = /usr/lib64/
CFLAGS = -std=c++11 -I$(VULKAN_INCLUDES)
LDFLAGS = -L$(VULKAN_LIBS) `pkg-config --static --libs glfw3` -lvulkan

build: main.cpp
	g++ $(CFLAGS) -o es3_test main.cpp $(LDFLAGS)

clean:
	rm -f es3_test
