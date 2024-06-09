.PHONY : clean

TARGET1=libp.so
TARGET2=libp.a
#ifeq ($(HOME_DIR),)
	HOME_DIR=../
#endif

all: $(TARGET1) $(TARGET2)

CPPFLAGS = -std=c++11 -fPIC -g -O3 -I$(HOME_DIR)/configs -I$(HOME_DIR)
LDFLAGS = -shared

SOURCES = $(shell echo *.cpp)
HEADERS = $(shell echo *.hpp)
OBJECTS = $(SOURCES:.cpp=.o)


$(TARGET1) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
	mv $(TARGET1) ../

$(TARGET2) : $(OBJECTS)
	ar rcs $@ $^



clean:
	rm -f $(OBJECTS) $(TARGET1) $(TARGET2)
	rm -f *.a *.so *.swp 


