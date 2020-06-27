TARGET = synth
CXX = g++
CXXFLAGS = -D__LINUX_ALSA__ -std=c++14 -g -Wall -Wextra -Wshadow -Werror -DNDEBUG 
LIBS = -lasound -lpthread -lrtaudio -lX11
OBJECTS = $(TARGET).o
HEADER = SoundPlayer.h
.PHONY:clean all

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CXX) $(CXXFLAGS)  $(OBJECTS) -o $(TARGET) $(LIBS)

%.o : %.cpp $(HEADER)
	$(CXX) $(CXXFLAGS)-c $< -o $@ $(LIBS) 

clean:
	rm -f $(TARGET) $(wildcard *.o)