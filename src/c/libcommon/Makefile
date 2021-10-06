TIMESTAMP ?= $(shell date +%Y%m%d%H%M%S)
GITHASH := $(shell ( echo "$(GITHASH)" | sed 's/  */\n/g' ; git rev-parse HEAD  ) | sort -u )
COMPONENT ?= common
TARGET ?= lib$(COMPONENT).a

BASE_BIN_DIR ?= ../../../bin
BIN_DIR ?= ../bin
LIB_DIR ?= ../lib
OBJ_DIR ?= ../dist/obj/lib$(COMPONENT)

INC_DIRS := ../dist/include

CP ?= cp -pv
MKDIR ?= mkdir -p
LN ?= ln
RM ?= rm

all: include

.PHONY: bin dep lib include clean distclean docker

lib: include
bin:
dep:

include:
	@$(MKDIR) ../dist/include
	@$(RM) -f ../dist/include/config.h ../dist/include/ipc_common.h
	$(LN) -s ../../libcommon/config.h ../dist/include/config.h
	$(LN) -s ../../libcommon/ipc_common.h ../dist/include/ipc_common.h

clean:
	$(RM) -r $(OBJ_DIR)

distclean: clean
	$(RM) -f ../dist/include/config.h ../dist/include/ipc_common.h

test:
	@echo "GITHASH: $(GITHASH)"
	
-include $(DEPS)
