TARGET1 = demo_car_gamepad
TARGET2 = demo_car_simple

TARGETS = $(TARGET1) $(TARGET2)

COPSIM_DIR=/opt/CoppeliaSim

API_DIR = $(COPSIM_DIR)/programming

SRC_C_API = \
    $(API_DIR)/remoteApi/extApi.c \
    $(API_DIR)/remoteApi/extApiPlatform.c \
    $(API_DIR)/common/shared_memory.c \

SRC_CPP_TG1 = $(TARGET1).cpp gamepad.cpp copsim_car.cpp trackview.cpp
SRC_CPP_TG2 = $(TARGET2).cpp copsim_car.cpp

SRC_H_TG1 = gamepad.h copsim_car.h trackview.h \
	#Utils.h \

SRC_H_TG2 = copsim_car.h

OBJ_C_API = $(notdir $(SRC_C_API:%.c=%.o))
OBJ_CPP_TG1 = $(SRC_CPP_TG1:%.cpp=%.o)
OBJ_CPP_TG2 = $(SRC_CPP_TG2:%.cpp=%.o)

DEFINES_ALL += -DNON_MATLAB_PARSING
DEFINES_ALL += -DMAX_EXT_API_CONNECTIONS=16

INCLUDES_ALL = -I $(API_DIR)/include -I $(API_DIR)/remoteApi

CFLAGS += -g -pthread $(DEFINES_ALL) $(INCLUDES_ALL)
CPPFLAGS += $(CFLAGS)
LDFLAGS += -lrt
LDFLAGS+=$(shell pkg-config --libs opencv)

vpath %.c $(dir $(SRC_C_API))

all: $(TARGETS)

$(TARGET1): $(OBJ_C_API) $(OBJ_CPP_TG1) $(SRC_H_TG1)
	g++ $(CPPFLAGS) $(OBJ_C_API) $(OBJ_CPP_TG1) $(LDFLAGS) -o $@

$(TARGET2): $(OBJ_C_API) $(OBJ_CPP_TG2) $(SRC_H_TG2)
	g++ $(CPPFLAGS) $(OBJ_C_API) $(OBJ_CPP_TG2) $(LDFLAGS) -o $@

clean:
	rm -rf $(TARGETS) *.o
