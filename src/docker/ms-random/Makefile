SHELL := /bin/bash
GITHASH := $(shell ( echo "$(GITHASH)" | sed 's/  */\n/g' ; git rev-parse HEAD  ) | sort -u )
_GITHASH := $(shell ( echo "$(GITHASH)" | sed 's/^ *//; s/  *$$//; s/  */\\|/g') )
PROJECT ?= com.gft.techoffice.event-20211108
COMPONENT ?= ms-random

CP ?= cp -pv
MKDIR ?= mkdir -p
LN ?= ln
RM ?= rm

all: docker

.PHONY: clean distclean docker ls

docker: ../dist/$(COMPONENT).iid

../dist/$(COMPONENT).iid: ../../go/$(COMPONENT)/*/*.go \
						../../go/$(COMPONENT)/Makefile
	@if [ -f "../dist/$(COMPONENT).iid" ] ; then i=$$( cat "../dist/$(COMPONENT).iid" ); docker image rm -f $$i ; rm -f "../dist/$(COMPONENT).iid"  2>/dev/null ; fi
	@docker image build -f ./Dockerfile \
	  --build-arg GITHASH="$(_GITHASH)" \
	  --build-arg TIMESTAMP="$(TIMESTAMP)" \
	  --build-arg COMPONENT=$(COMPONENT) \
	  --tag $(COMPONENT):latest \
	  --label PROJECT="$(PROJECT)" \
	  --label GITHASH="$(_GITHASH)" \
	  --label COMPONENT=$(COMPONENT) \
	  --label BUILDSTAMP="$(TIMESTAMP)" \
	  --iidfile "../dist/$(COMPONENT).iid" \
	  ../..

clean:
	@if [ -f "../dist/$(COMPONENT).iid" ] ; then \
	    i=$$( cat "../dist/$(COMPONENT).iid" ); \
		for container in $$(docker container ls --filter "ancestor=$$i" --format "{{.ID}}" ) ; do \
	    	docker container stop --time 5 "$$container" ; \
		done ; \
	    docker image rm -f $$i ; \
		rm -f "../dist/$(COMPONENT).iid" ; \
	fi

distclean: clean
	@for image in $$( docker image ls --filter "label=PROJECT=$(PROJECT)" --filter "label=COMPONENT=$(COMPONENT)" --format "{{.ID}}" ) ; do \
		for container in $$(docker container ls --filter "ancestor=$$image" --format "{{.ID}}" ) ; do \
			docker container stop --time 5 "$$container" ; \
		done ; \
		docker image rm -f $$image ; \
	done

ls:
	@while read img imgname ; do \
		echo "I $$img $$imgname" ; \
		while read id state name image ; do \
			printf 'C %-7s %-10s %-20s %s\n' "$$id" "$$state" "$$name" "$$image" ; \
		done < <( docker container ls --filter "ancestor=$$img" --format='{{.ID}} {{.State}} {{.Names}} {{.Image}}'  | sort ) ; \
	done < <(docker image ls --filter "label=PROJECT=$(PROJECT)" --filter "label=COMPONENT=$(COMPONENT)" --format='{{.ID}} {{.Repository}}:{{.Tag}}' | sort)

test:
	@echo "GITHASH: $(_GITHASH)"
	
-include $(DEPS)
