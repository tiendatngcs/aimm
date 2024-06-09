.PHONY : clean

TARGET1=libv2p.so
TARGET2=libv2p.a

#all: $(TARGET1) $(TARGET2)
all: $(TARGET1) 

CPPFLAGS = -std=c++11 -fPIC -g -O3
LDFLAGS = -shared 
PYBIND11_INC = `python3 -m pybind11 --includes`

SOURCES = $(shell echo *.cpp)
HEADERS = $(shell echo *.hpp)
OBJECTS = $(SOURCES:.cpp=.o)

#all: $(TARGET1) $(TARGET2)

clean:
	rm -f $(OBJECTS) $(TARGET1) #$(TARGET2)
	rm -f *.a *.so *.swp *.swo

$(TARGET1) : $(OBJECTS)
	#$(CC) $(CFLAGS) $(PYBIND11_INC) $(OBJECTS) -o $@ $(LDFLAGS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
	mv $(TARGET1) ../

#$(TARGET2) : $(OBJECTS)
#	ar rcs $@ $^
#	mv $(TARGET2) ../
