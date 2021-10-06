TIMESTAMP ?= $(shell date +%Y%m%d%H%M%S)
GITHASH := $(shell ( echo "$(GITHASH)" | sed 's/  */\n/g' ; git rev-parse HEAD  ) | sort -u )
COMPONENT ?= logging
TARGET ?= lib$(COMPONENT).a

BASE_BIN_DIR ?= ../../../bin
BIN_DIR ?= ../dist/bin
LIB_DIR ?= ../dist/lib
OBJ_DIR ?= ../dist/obj/lib$(COMPONENT)

SRCS := logging
OBJS := $(SRCS:%=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := ../dist/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

_CPPFLAGS := $(INC_FLAGS) -MMD -MP $(_CPPFLAGS) $(CPPFLAGS) 

CP ?= cp -pv
MKDIR ?= mkdir -p
LN ?= ln
RM ?= rm

all: lib

.PHONY: bin dep lib include clean distclean docker

lib: include $(LIB_DIR)/$(TARGET)
bin:
dep:

include:
	@$(MKDIR) ../dist/include
	@$(RM) -f ../dist/include/logging.h
	$(LN) -s ../../liblogging/logging.h ../dist/include/logging.h

$(LIB_DIR)/$(TARGET): $(OBJS)
	@$(MKDIR) $(dir $@)
	$(AR) rcs $@ $(OBJS) 

$(OBJ_DIR)/%.o: %.c
	@$(MKDIR) $(dir $@)
	$(CC) $(_CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)

distclean: clean
	$(RM) -f $(LIB_DIR)/$(TARGET)
	$(RM) -f ../dist/include/logging.h

test:
	@echo "GITHASH: $(GITHASH)"
	
-include $(DEPS)