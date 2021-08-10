BINS := ms-measure ms-random

BINS_SUBDIRS := $(addprefix src/,$(BINS))

SUBDIRS = $(BINS_SUBDIRS)

all:
	@for dir in $(SUBDIRS) ; do \
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
