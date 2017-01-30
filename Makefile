CF1 = -Wnarrowing -fpermissive -std=gnu++11
CF2 = $(shell pkg-config --cflags opencv)
CXXFLAGS = $(CF1) $(CF2) -g
LDFLAGS= -L /usr/local/share/OpenCV/3rdparty/lib -lstdc++ -lm $(shell pkg-config --libs opencv)
OBJS = main.o
TARGET = main

$(TARGET): $(OBJS)
	g++ $^ $(LDFLAGS) -o $@ $(LIBS)

clean:
	-rm $(OBJS) $(TARGET)
