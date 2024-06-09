CC := g++
BUILD_DIR ?= ./builds
BIN_DIR ?= ./bin
LIB_DIR ?= ./lib
SRC_ROOT ?= ./src

CPPFLAGS += -std=c++11 -O3 -ggdb -openmp -fPIC
#CPPFLAGS += -std=c++11 -O3 -openmp -fPIC
LDFLAGS += -L. -L$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) -fPIC
PYBIND11_INC = `python3 -m pybind11 --includes`

LIB_RLMMU := rlmmu_pybind.so

INCS ?=  -I$(SRC_ROOT) -I$(SRC_ROOT)/m_net -I$(SRC_ROOT)/v2p -I./progs -I./configs

all: configs $(LIB_RLMMU) 


SRCS := $(shell find ./$(SRC_ROOT) -name *.cpp)
SRCS += $(shell find ./progs -name *.cpp)
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)


SRCS_TEST := $(shell find ./$(SRC_ROOT) ./progs \( -iname *.cpp ! -name main.cpp \) )
SRCS_TEST += $(shell find ./tests -name *.cpp)
OBJS_TEST := $(SRCS_TEST:%.cpp=$(BUILD_DIR)/%.o)
DEPS_TEST := $(OBJS_TEST:.o=.d)

configs:
	@${MAKE} -C configs
	$(MKDIR_P) $(LIB_DIR)
	cp configs/libconfig.so ./lib

$(LIB_RLMMU): $(OBJS)
	$(MKDIR_P) $(BIN_DIR)
	$(CC) -shared $(OBJS) $(INCS) $(PYBIND11_INC) -o $(BIN_DIR)/$@ $(LDFLAGS) -lconfig

test: configs $(OBJS_TEST)
	$(MKDIR_P) $(BIN_DIR)
	$(CC) $(OBJS_TEST) $(INCS) $(PYBIND11_INC) -o $(BIN_DIR)/$@ $(LDFLAGS) -lconfig

$(BUILD_DIR)/%.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCS) $(PYBIND11_INC) -c $< -o $@


.PHONY: clean configs

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(BIN_DIR)
	$(RM) -r $(LIB_DIR)
	$(RM) -r *.log *.out mem_env.egg-info __pycache__ 
	@${MAKE} -C configs clean

-include $(DEPS)

MKDIR_P ?= mkdir -p
