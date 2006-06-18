DIRS = cliparser focal funtest matchit msgqrd msgqrclient prim primfak process properties semaphoren graph tree
#old: 
DIST_DIR = dist

DIST_PREFIX = dist

DIST_IDENTIFIER = c++

DATE := $(shell date -I)

DIST_FILE = $(DIST_PREFIX)_$(DIST_IDENTIFIER)_$(DATE)_.tar

EDITOR ?= vi

all:
	$(foreach dir,$(DIRS),echo ; echo \*\*\*\*\*\*\*\*\*\*\*\*; cd $(dir); \
		echo \*\*\* generating Makefile \*\*\*; \
		cat Makefile_head ../Makefile_body > Makefile; \
		$(MAKE) -k; cd ../; echo \*\*\*\*\*\*\*\*\*\*\*\*; echo ; \
	)

.PHONY:	all clean ed dist

clean:
	$(foreach dir,$(DIRS),echo ; echo \*\*\*\*\*\*\*\*\*\*\*\*; cd $(dir); $(MAKE) -k clean; cd ../; echo \*\*\*\*\*\*\*\*\*\*\*\*; echo ;)

ed:
	$(EDITOR) Makefile&

dist:
	$(MAKE) -k -f Makefile_dist dist
