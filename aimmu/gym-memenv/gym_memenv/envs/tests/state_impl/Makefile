CC=g++
LIB_DIR ?= ./lib
SRC_ROOT ?= ./src
BUILD_DIR ?= ./build
BIN_DIR ?= ./bin

CFLAGS=-std=c++11 -O3 -ggdb -fPIC
LDFLAGS += -L. -L$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) -fPIC
PYBIND11_INC = `python3 -m pybind11 --includes`

LIB_GRAPH := graph_pybind.so

INCS ?=  -I$(SRC_ROOT) -I./../../configs

all: configs $(LIB_GRAPH) 

SRCS := $(shell find ./$(SRC_ROOT) -name *.cpp)
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

configs:
	@${MAKE} -C ../../configs
	$(MKDIR_P) $(LIB_DIR)
	cp ../../configs/libconfig.so ./lib

$(LIB_GRAPH): $(OBJS)
	$(MKDIR_P) $(BIN_DIR)
	$(CC) -shared $(OBJS) $(INCS) $(PYBIND11_INC) -o $(BIN_DIR)/$@ $(LDFLAGS) -lconfig

$(BUILD_DIR)/%.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCS) $(PYBIND11_INC) -c $< -o $@

clean:
	rm -rf *.o .*.swo .*.swn .*.swp *.log test
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(BIN_DIR)
	$(RM) -r $(LIB_DIR)
	$(RM) -r *.log *.out mem_env.egg-info __pycache__ 
	#@${MAKE} -C configs clean

-include $(DEPS)

MKDIR_P ?= mkdir -p
