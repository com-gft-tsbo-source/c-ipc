TIMESTAMP:=$(shell date +%Y%m%d%H%M%S)
GITHASH:=$(shell git rev-parse HEAD)
TARGET ?= server

BIN_DIR ?= ../../bin
LIB_DIR ?= ../../lib
OBJ_DIR ?= ./obj

SRCS := server
OBJS := $(SRCS:%=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := ../../include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

LINK_DIR := ../../lib
LINK_STATIC := logging lockfile
LINK_SYSTEM := rt
LINK_FLAGS := $(addprefix -l,$(LINK_SYSTEM)) $(addprefix -L,$(LINK_DIR)) $(addprefix -l,$(LINK_STATIC)) 
LINK_DEPS := $(addsuffix .a,$(addprefix $(LINK_DIR)/lib,$(LINK_STATIC)))

$(BIN_DIR)/$(TARGET): $(OBJS) $(LINK_DEPS)
	$(CC) $(OBJS) -o $@ $(LINK_FLAGS) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(INC_FLAGS) -MMD -MP $(CPPFLAGS) $(CFLAGS) -c $< -o $@

docker:
	@docker image build -f Dockerfile \
	                    --tag $(TARGET):latest \
						--tag $(TARGET):$(GITHASH) \
						--label GITHASH=$(GITHASH) \
						--label COMPONENT=$(TARGET) \
						--label BUILDSTAMP=$(TIMESTAMP) \
						../..

.PHONY: clean docker

clean:
	$(RM) -r $(OBJ_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p