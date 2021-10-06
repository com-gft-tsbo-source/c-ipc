SHELL := /bin/bash
GITHASH := $(shell ( echo "$(GITHASH)" | sed 's/  */\n/g' ; git rev-parse HEAD  ) | sort -u )
PROJECT ?= com.gft.techoffice.event-20211108

COMPONENTS := server client tail ms-measure ms-random

all: docker

.PHONY: docker clean distclean $(COMPONENTS)

docker:
	@for dir in $(COMPONENTS) ; do \
	    TIMESTAMP="$(TIMESTAMP)" \
	    GITHASH="$(GITHASH)" \
	    PROJECT="$(PROJECT)" \
		make -C  $$dir docker ; \
	done

clean:
	@for dir in $(COMPONENTS) ; do \
	    TIMESTAMP="$(TIMESTAMP)" \
	    GITHASH="$(GITHASH)" \
	    PROJECT="$(PROJECT)" \
		make -C  $$dir clean ; \
	done

distclean:
	@for dir in $(COMPONENTS) ; do \
	    TIMESTAMP="$(TIMESTAMP)" \
	    GITHASH="$(GITHASH)" \
	    PROJECT="$(PROJECT)" \
		make -C  $$dir distclean ; \
	done
	@for image in $$( docker image ls --filter "label=PROJECT=$(PROJECT)" --format "{{.ID}}" ) ; do \
		for container in $$(docker container ls --filter "ancestor=$$image" --format "{{.ID}}" ) ; do \
			docker container stop --time 5 "$$container" ; \
		done ; \
		docker image rm -f $$image ; \
	done

ls:
	@for dir in $(COMPONENTS) ; do \
	    TIMESTAMP="$(TIMESTAMP)" \
	    GITHASH="$(GITHASH)" \
	    PROJECT="$(PROJECT)" \
		make -C  $$dir ls ; \
	done

test:
	@echo "GITHASH: $(GITHASH)"
	@for dir in $(BINS_SUBDIRS) $(LIBS_SUBDIRS) ; do \
	    TIMESTAMP="$(TIMESTAMP)" \
	    GITHASH="$(GITHASH)" \
		make -C $$dir test; \
	    PROJECT="$(PROJECT)" \
	done
