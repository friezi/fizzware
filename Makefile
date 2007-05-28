DIRS = cliparser focal funtest matchit msgqrd msgqrclient prim primfak process properties semaphoren graph tree bitmatrix

EDITOR ?= vi

all:
	$(foreach dir,$(DIRS),echo ; echo \*\*\*\*\*\*\*\*\*\*\*\*; pushd $(dir); \
		echo \*\*\* generating Makefile \*\*\*; \
		cat Makefile_head ../Makefile_body > Makefile; \
		$(MAKE) -k; popd ; echo \*\*\*\*\*\*\*\*\*\*\*\*; echo ; \
	)

.PHONY:	all clean ed dist

clean:
	$(foreach dir,$(DIRS),echo ; echo \*\*\*\*\*\*\*\*\*\*\*\*; cd $(dir); $(MAKE) -k clean; cd ../; echo \*\*\*\*\*\*\*\*\*\*\*\*; echo ;)

ed:
	$(EDITOR) Makefile&

dist:
	$(MAKE) -k -f Makefile_dist dist
