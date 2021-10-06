TIMESTAMP ?= $(shell date +%Y%m%d%H%M%S)
GITHASH := $(shell ( echo "$(GITHASH)" | sed 's/  */\n/g' ; git rev-parse HEAD  ) | sort -u )
COMPONENT ?= server
TARGET ?= $(COMPONENT)

BASE_BIN_DIR ?= ../../../bin
BIN_DIR ?= ../dist/bin
LIB_DIR ?= ../dist/lib
OBJ_DIR ?= ../dist/obj/$(COMPONENT)

_SRCS := server config
SRCS := $(SRCS:%=%.c)
OBJS := $(_SRCS:%=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := ../dist/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

_CPPFLAGS := $(INC_FLAGS) -MMD -MP $(_CPPFLAGS) $(CPPFLAGS) 

LINK_DIR := ../dist/lib
LINK_STATIC := daemon lockfile logging
LINK_SYSTEM := rt bsd
LINK_FLAGS := $(addprefix -l,$(LINK_SYSTEM)) $(addprefix -L,$(LINK_DIR)) $(addprefix -l,$(LINK_STATIC)) 
LINK_DEPS := $(addsuffix .a,$(addprefix $(LINK_DIR)/lib,$(LINK_STATIC)))

CP ?= cp -pv
MKDIR ?= mkdir -p
LN ?= ln
RM ?= rm

all: bin

.PHONY: bin dep lib include clean distclean docker

bin: $(BIN_DIR)/$(TARGET)
dep:
include:
lib:

$(BIN_DIR)/$(TARGET): $(OBJS) $(LINK_DEPS)
	@$(MKDIR) $(dir $@)
	$(CC) $(OBJS) -o $@ $(LINK_FLAGS) $(LDFLAGS)
	@$(CP) $@ $(BASE_BIN_DIR)

$(OBJ_DIR)/%.o: %.c
	@$(MKDIR) $(dir $@)
	$(CC) $(INC_FLAGS) -MMD -MP $(_CPPFLAGS) $(CFLAGS) -c $< -o $@

docker:
	@docker image build -f ../../docker/$(COMPONENT)/Dockerfile \
	  --build-arg GITHASH="$(GITHASH)" \
	  --build-arg TIMESTAMP="$(TIMESTAMP)" \
	  --build-arg COMPONENT=$(COMPONENT) \
	  --tag $(COMPONENT):latest \
	  --tag $(COMPONENT):"$(GITHASH)" \
	  --label GITHASH="$(GITHASH)" \
	  --label COMPONENT=$(COMPONENT) \
	  --label BUILDSTAMP="$(TIMESTAMP)" \
	  ../..

clean:
	$(RM) -rf $(OBJ_DIR)

distclean: clean
	$(RM) -f $(BIN_DIR)/$(TARGET) $(BASE_BIN_DIR)/$(TARGET)

test:
	@echo "GITHASH: $(GITHASH)"
	
-include $(DEPS)
