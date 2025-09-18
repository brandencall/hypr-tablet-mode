# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -Iinclude -MMD -MP
PKGS = libinput libsystemd
CXXFLAGS += $(shell pkg-config --cflags $(PKGS))
LDFLAGS  += $(shell pkg-config --libs $(PKGS)) -ludev

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
IPC_DIR = ipc
INPUT_DAEMON_DIR = input_daemon
HYPR_CONTROL_DIR = hypr_control

# Binaries
DAEMON_TARGET = $(BIN_DIR)/input_daemon
HYPR_TARGET = $(BIN_DIR)/hypr_control


# Sources
IPC_SRCS = $(shell find $(SRC_DIR)/$(IPC_DIR) -name '*.cpp')
DAEMON_SRCS = $(shell find $(SRC_DIR)/$(INPUT_DAEMON_DIR) -name '*.cpp') 
HYPR_SRCS = $(shell find $(SRC_DIR)/$(HYPR_CONTROL_DIR) -name '*.cpp') 

IPC_OBJ  = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(IPC_SRCS))
DAEMON_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(DAEMON_SRCS))
HYPR_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(HYPR_SRCS))

DEPS = $(OBJECTS:.o=.d)

# Targets
all: $(DAEMON_TARGET) $(HYPR_TARGET)

$(DAEMON_TARGET): $(DAEMON_OBJ) $(IPC_OBJ) | $(BIN_DIR)
	$(CXX) $^ $(DAEMON_OBJ) $(LDFLAGS) -o $@

$(HYPR_TARGET): $(HYPR_OBJ) $(IPC_OBJ) | $(BIN_DIR)
	$(CXX) $^ $(HYPR_OBJ) $(LDFLAGS) -o $@

# Compilation rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Directories
$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(DEPS)
.PHONY: all clean
