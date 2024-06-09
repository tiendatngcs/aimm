CXX = g++
CXXFLAGS = -std=c++11 -ggdb -O3 -pg
CCFLAGS = -Wall -O3 -g -pg
LEX = flex
YACC   = bison -y

TARGET := libconfig.so

TARGET2 := libconfig.a

# simulator source files
CPP_SRCS = $(wildcard *.cpp) $(wildcard */*.cpp)
CPP_HDRS = $(wildcard *.hpp) $(wildcard */*.hpp)
CPP_DEPS = $(CPP_SRCS:.cpp=.d)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)

LEX_SRCS = lex.yy.c
LEX_OBJS = lex.yy.o

YACC_SRCS = y.tab.c
YACC_HDRS = y.tab.h
YACC_OBJS = y.tab.o

OBJS :=  $(CPP_OBJS) $(LEX_OBJS) $(YACC_OBJS)

.PHONY: clean

all: $(TARGET) $(TARGET2)

$(TARGET): $(OBJS)
	 $(CXX) $(CXXFLAGS) -shared $^ -o $@

$(TARGET2): $(OBJS)
	 ar rcs $@ $^

$(LEX_SRCS): config.l
	$(LEX) $<

$(YACC_SRCS) $(YACC_HDRS): config.y
	$(YACC) -d $<

$(LEX_OBJS): $(LEX_SRCS) $(YACC_HDRS)
	$(CC) $(CCFLAGS) -fPIC -c $< -o $@

$(YACC_OBJS): $(YACC_SRCS)
	$(CC) $(CCFLAGS) -fPIC -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

clean:
	rm -f $(YACC_SRCS) $(YACC_HDRS)
	rm -f $(LEX_SRCS)
	rm -f $(CPP_DEPS)
	rm -f $(OBJS) *.a
	rm -f $(TARGET) .*.swp *.gch


-include $(CPP_DEPS)
