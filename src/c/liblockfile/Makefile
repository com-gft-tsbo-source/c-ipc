TARGET ?= liblockfile.a

BIN_DIR ?= ../../bin
LIB_DIR ?= ../../lib
OBJ_DIR ?= ./obj

SRCS := lockfile
OBJS := $(SRCS:%=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := ../../include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

CP ?= cp -p	

$(LIB_DIR)/$(TARGET): $(OBJS)
	$(AR) rcs $@ $(OBJS) 

$(OBJ_DIR)/%.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(OBJ_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p