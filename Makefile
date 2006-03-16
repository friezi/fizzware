DIRS = cliparser focal funtest matchit msgqrd msgqrclient prim primfak process properties semaphoren
#old: 
DIST_DIR = dist

DIST_PREFIX = dist

DIST_IDENTIFIER = c++

DATE := $(shell date -I)

DIST_FILE = $(DIST_PREFIX)_$(DIST_IDENTIFIER)_$(DATE)_.tar

EDITOR ?= vi

all:
	$(foreach dir,$(DIRS),echo ; echo \*\*\*\*\*\*\*\*\*\*\*\*; cd $(dir); $(MAKE) -k; cd ../; echo \*\*\*\*\*\*\*\*\*\*\*\*; echo ;)

.PHONY:	all clean ed dist

clean:
	$(foreach dir,$(DIRS),echo ; echo \*\*\*\*\*\*\*\*\*\*\*\*; cd $(dir); $(MAKE) -k clean; cd ../; echo \*\*\*\*\*\*\*\*\*\*\*\*; echo ;)

ed:
	$(EDITOR) Makefile&

dist:
	cd classes; $(MAKE) -k dist; cd -;
	cp --target-directory=$(DIST_DIR) Makefile  Makefile_template GPL
	$(foreach dir,$(DIRS),cd $(dir); $(MAKE) -k dist; cd ../;)
	tar cf $(DIST_FILE) dist/
	bzip2 -9 $(DIST_FILE)
