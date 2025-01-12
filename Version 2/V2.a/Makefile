TARGET_EXEC ?= V2.a

BUILD_DIR ?= ./build
TARGET_DIR ?= .
SRC_DIRS ?= .

SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS ?= $(INC_FLAGS) -MMD -MP -O3 -DNDEBUG -Wall -Wextra -pedantic -std=c11 -Wno-unused-parameter -fPIE
LDFLAGS ?= -pie

$(TARGET_DIR)/$(TARGET_EXEC): $(OBJS)
	$(MKDIR_P) $(TARGET_DIR)
	$(CC) $(OBJS) -o $(TARGET_DIR)/$(TARGET_EXEC) $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
