DIRS = fizzcal matchit msgqrd msgqrclient prim primfak process hpreis

EDITOR ?= vi

all:	test
	$(foreach dir,$(DIRS),echo ; echo \*\*\*\*\*\*\*\*\*\*\*\*; pushd $(dir); \
		echo \*\*\* generating Makefile \*\*\*; \
		cat Makefile_head ../Makefile_body > Makefile; \
		echo \*\*\* calling make for project $(dir) \*\*\* ; \
		$(MAKE) -k; popd ; echo \*\*\*\*\*\*\*\*\*\*\*\*; echo ; \
	)

.PHONY:	all clean ed dist test

test:
	cd test/; \
	$(MAKE) -k; \
	cd ../;

clean:
	cd test/;\
	$(MAKE) -k clean; \
	cd ../;
	$(foreach dir,$(DIRS),echo ; echo \*\*\*\*\*\*\*\*\*\*\*\*; pushd $(dir); $(MAKE) -k clean; popd; echo \*\*\*\*\*\*\*\*\*\*\*\*; echo ;)

ed:
	$(EDITOR) Makefile Makefile_body Makefile_head_template &

dist:
	$(MAKE) -k -f Makefile_dist dist

