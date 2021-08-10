LIBS := logging lockfile
BINS := server client


LIBS_SUBDIRS := $(addprefix src/lib,$(LIBS))
BINS_SUBDIRS := $(addprefix src/,$(BINS))

SUBDIRS = $(LIBS_SUBDIRS) $(BINS_SUBDIRS)
all:
	@for dir in $(SUBDIRS) ; do \
		make -C  $$dir ; \
	done

.PHONY: lib
lib:
	@for dir in $(LIBS_SUBDIRS) ; do \
		make -C  $$dir ; \
	done

.PHONY: bin
bin:
	@for dir in $(BINS_SUBDIRS) ; do \
		make -C  $$dir ; \
	done

clean:
	@for dir in $(SUBDIRS) ; do \
		make -C  $$dir clean ; \
	done
