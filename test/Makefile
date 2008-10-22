DIRS = cliparser funtest properties semaphoren graph tree bitmatrix lexer ringbuffer observer testunit llparser

EDITOR ?= vi

all:
	$(foreach dir,$(DIRS),echo ; echo \*\*\*\*\*\*\*\*\*\*\*\*; pushd $(dir); \
		echo \*\*\* generating Makefile \*\*\*; \
		cat Makefile_head ../../Makefile_body > Makefile; \
		echo \*\*\* calling make for project $(dir) \*\*\* ; \
		$(MAKE) -k; popd ; echo \*\*\*\*\*\*\*\*\*\*\*\*; echo ; \
	)

.PHONY:	all clean ed dist

clean:
	$(foreach dir,$(DIRS),echo ; echo \*\*\*\*\*\*\*\*\*\*\*\*; pushd $(dir); $(MAKE) -k clean; popd; echo \*\*\*\*\*\*\*\*\*\*\*\*; echo ;)

ed:
	$(EDITOR) Makefile&

dist:
	$(MAKE) -k -f Makefile_dist dist

