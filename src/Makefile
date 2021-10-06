TIMESTAMP ?= $(shell date +%Y%m%d%H%M%S)
GITHASH := $(shell ( echo "$(GITHASH)" | sed 's/  */\n/g' ; git rev-parse HEAD  ) | sort -u )
PROJECT ?= com.gft.techoffice.event-20211108

SUBDIRS := c go docker

.PHONY : all clean distclean lib bin dep include docker test

all:
	@for dir in $(SUBDIRS) ; do \
	    TIMESTAMP="$(TIMESTAMP)" \
	    GITHASH="$(GITHASH)" \
	    PROJECT="$(PROJECT)" \
		make -C  $$dir ; \
	done

clean:
	@for dir in $(SUBDIRS) ; do \
	    TIMESTAMP="$(TIMESTAMP)" \
	    GITHASH="$(GITHASH)" \
	    PROJECT="$(PROJECT)" \
		make -C  $$dir clean ; \
	done

docker:
	@for dir in $(SUBDIRS) ; do \
	    TIMESTAMP="$(TIMESTAMP)" \
	    GITHASH="$(GITHASH)" \
	    PROJECT="$(PROJECT)" \
		make -C  $$dir docker ; \
	done

distclean:
	@for dir in $(SUBDIRS) ; do \
	    TIMESTAMP="$(TIMESTAMP)" \
	    GITHASH="$(GITHASH)" \
	    PROJECT="$(PROJECT)" \
		make -C  $$dir distclean ; \
	done

test:
	@echo "GITHASH: $(GITHASH)"
	@for dir in $(SUBDIRS) ; do \
	    TIMESTAMP="$(TIMESTAMP)" \
	    GITHASH="$(GITHASH)" \
	    PROJECT="$(PROJECT)" \
		make -C $$dir test; \
	done
