SHELL := /bin/bash
GITHASH := $(shell ( echo "$(GITHASH)" | sed 's/  */\n/g' ; git rev-parse HEAD  ) | sort -u )
_GITHASH := $(shell ( echo "$(GITHASH)" | sed 's/^ *//; s/  *$$//; s/  */\\|/g') )
PROJECT ?= com.gft.techoffice.event-20211108
COMPONENT ?= client
WRAPPER ?= ms-measure

CP ?= cp -pv
MKDIR ?= mkdir -p
LN ?= ln
RM ?= rm

all: docker

.PHONY: clean distclean docker ls

docker: ../dist/$(COMPONENT)-simple.iid \
		../dist/$(COMPONENT)-wrapped.iid

../dist/$(COMPONENT)-simple.iid: Dockerfile-simple ../../c/$(COMPONENT)/*.[ch] \
						../../c/$(COMPONENT)/Makefile \
						../../c/dist/lib/*.a \
						../../c/dist/include/*.h
	@if [ -f "../dist/$(COMPONENT)-simple.iid" ] ; then i=$$( cat "../dist/$(COMPONENT)-simple.iid" ); docker image rm -f $$i ; rm -f "../dist/$(COMPONENT)-simple.iid"  2>/dev/null ; fi
	@docker image build -f ./Dockerfile-simple \
	  --build-arg GITHASH="$(_GITHASH)" \
	  --build-arg TIMESTAMP="$(TIMESTAMP)" \
	  --build-arg COMPONENT=$(COMPONENT) \
	  --tag $(COMPONENT):simple \
	  --label VARIANT="0" \
	  --label PROJECT="$(PROJECT)" \
	  --label GITHASH="$(_GITHASH)" \
	  --label COMPONENT=$(COMPONENT) \
	  --label BUILDSTAMP="$(TIMESTAMP)" \
	  --iidfile "../dist/$(COMPONENT)-simple.iid" \
	  ../..

../dist/$(COMPONENT)-wrapped.iid: Dockerfile-wrapped ../../c/$(COMPONENT)/*.[ch] \
						../../c/$(COMPONENT)/Makefile \
						../../c/dist/lib/*.a \
						../../c/dist/include/*.h \
						../../go/$(WRAPPER)/*/*.go \
						../../go/$(WRAPPER)/Makefile
	@if [ -f "../dist/$(COMPONENT)-wrapped.iid" ] ; then i=$$( cat "../dist/$(COMPONENT)-wrapped.iid" ); docker image rm -f $$i ; rm -f "../dist/$(COMPONENT)-wrapped.iid"  2>/dev/null ; fi
	@docker image build -f ./Dockerfile-wrapped \
	  --build-arg GITHASH="$(_GITHASH)" \
	  --build-arg TIMESTAMP="$(TIMESTAMP)" \
	  --build-arg COMPONENT=$(COMPONENT) \
	  --build-arg WRAPPER=$(WRAPPER) \
	  --tag $(COMPONENT):wrapped \
	  --label PROJECT="$(PROJECT)" \
	  --label VARIANT="1" \
	  --label GITHASH="$(_GITHASH)" \
	  --label COMPONENT=$(COMPONENT) \
	  --label BUILDSTAMP="$(TIMESTAMP)" \
	  --iidfile "../dist/$(COMPONENT)-wrapped.iid" \
	  ../..

clean:
	@if [ -f "../dist/$(COMPONENT)-simple.iid" ] ; then \
	    i=$$( cat "../dist/$(COMPONENT)-simple.iid" ); \
		for container in $$(docker container ls --filter "ancestor=$$i" --format "{{.ID}}" ) ; do \
	    	docker container stop --time 5 "$$container" ; \
		done ; \
	    docker image rm -f $$i ; \
		rm -f "../dist/$(COMPONENT)-simple.iid" ; \
	fi
	@if [ -f "../dist/$(COMPONENT)-wrapped.iid" ] ; then \
	    i=$$( cat "../dist/$(COMPONENT)-wrapped.iid" ); \
		for container in $$(docker container ls --filter "ancestor=$$i" --format "{{.ID}}" ) ; do \
	    	docker container stop --time 5 "$$container" ; \
		done ; \
	    docker image rm -f $$i ; \
		rm -f "../dist/$(COMPONENT)-wrapped.iid" ; \
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
