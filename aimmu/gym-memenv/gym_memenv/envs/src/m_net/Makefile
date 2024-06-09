.PHONY : clean

TARGET1=libm_net.so
TARGET2=libm_net.a

all: $(TARGET1) $(TARGET2)

CPPFLAGS = -std=c++11 -fPIC -g -O3
LDFLAGS = -shared 

SOURCES = $(shell echo *.cpp)
HEADERS = $(shell echo *.hpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET1) $(TARGET2)

clean:
	rm -f $(OBJECTS) $(TARGET1) $(TARGET2)
	rm -f *.a .*.so .*.swp .*.swo

$(TARGET1) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
	mv $(TARGET1) ../

$(TARGET2) : $(OBJECTS)
	ar rcs $@ $^
	mv $(TARGET2) ../
