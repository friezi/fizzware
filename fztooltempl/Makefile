################################################################################
################### erzeugt statische Bibliothek ###############################
################################################################################

LIBRARY = fztooltempl

VERSION_NUMBER = 1.93

OBJECTS = exception messagequeue socket tcpserver tcpclient msgqueueregclient primlist \
	semaphor mathexpression signalhandler stdmessageserver cmdlparser propertyreader

#old: 

HEADERS_ONLY = functions datastructures graph

EDITOR ?= vi

DIST_DIR = ../dist
DT_DIR = $(DIST_DIR)/classes

################################################################################

DOC = doc

EXAMPLES = codeexamples

DOXY = $(DOC)/$(LIBRARY).doxyfile

OBJECTNAMES = $(OBJECTS:%=%.o)

IMPORTANT_HEADERS = $(HEADERS_ONLY:%=%.hpp)

LIB_NAME = lib$(LIBRARY).a

MAKEFILE_CALLS = $(OBJECTS:%=%_makefile)

OBJ_START = true
OBJ_END = false

.PHONY: all clean ed cleandoc dist

all:	$(LIB_NAME) $(DOC)

$(DOC):


	@echo
	@echo '****************' generating documentation '****************'
	[[ -d $(DOC) ]] || mkdir $(DOC)
	doxygen -g $(DOC)/doxyfile %1>/dev/zero
	sed	-e 's/^[ ]*\(PROJECT_NAME\)[ ]*=.*$$/\1 = $(LIBRARY)/g'\
		-e 's/^[ ]*\(PROJECT_NUMBER\)[ ]*=.*$$/\1 = $(VERSION_NUMBER)/g'\
		-e 's/^[ ]*\(QUIET\)[ ]*=.*$$/\1 = YES/g'\
		-e 's/^[ ]*\(EXTRACT_PRIVATE\)[ ]*=.*$$/\1 = YES/g'\
		-e 's/^[ ]*\(ALPHABETICAL_INDEX\)[ ]*=.*$$/\1 = YES/g'\
		-e 's/^[ ]*\(WARN_IF_UNDOCUMENTED\)[ ]*=.*$$/\1 = NO/g'\
		-e 's/^[ ]*\(INTERNAL_DOCS\)[ ]*=.*$$/\1 = YES/g'\
		-e 's/^[ ]*\(SORT_MEMBER_DOCS\)[ ]*=.*$$/\1 = NO/g'\
		-e 's/^[ ]*\(OUTPUT_DIRECTORY\)[ ]*=.*$$/\1 = $(DOC)/g'\
		-e 's/^[ ]*\(EXAMPLE_PATH\)[ ]*=.*$$/\1 = $(EXAMPLES)/g'\
		-e 's/^[ ]*\(PREDEFINED\)[ ]*=.*$$/\1 = DOXYGEN_SHOULD_SKIP_THIS/g' $(DOC)/doxyfile >$(DOXY)
	doxygen $(DOXY)
	rm $(DOC)/doxyfile
	rm $(DOXY)
	@echo '****************' documentation generated '*****************'
	@echo

cleandoc:
	rm -r $(DOC)

$(LIB_NAME): $(OBJECTNAMES) $(IMPORTANT_HEADERS)
	@echo
	@echo '****************' generating the static-library $(LIB_NAME) '****************'
	$(foreach mk,$(MAKEFILE_CALLS),$(MAKE) -k -f $(mk);)
	ar rs $(LIB_NAME) $(OBJECTNAMES)
	@echo '****************' generation of $(LIB_NAME) finished '***********************'
	@echo

%.o:%.cpp %.hpp
	@echo '***' compiling module $(@) for static-library  $(LIB_NAME) '***'
	$(MAKE) -f $(@:%.o=%_makefile)
	@echo '*********************' finished '******************************'

clean:	
	rm -f $(OBJECTNAMES)
	rm -f $(LIB_NAME)
	rm -r -f $(DOC)

ed:
#	$(foreach mked,$(MAKEFILE_CALLS),$(MAKE) -k -f $(mked) ed;)
#	$(EDITOR) Makefile &
	$(EDITOR) $(OBJECTS:%=%.cpp) $(OBJECTS:%=%.hpp) $(HEADERS_ONLY:%=%.hpp) $(OBJECTS:%=%_makefile) Makefile &

dist:
	[[ -d $(DIST_DIR) ]] || mkdir $(DIST_DIR)
	[[ -d $(DT_DIR) ]] || mkdir $(DT_DIR)
	cp --target-directory=$(DT_DIR) $(OBJECTS:=.?pp) $(OBJECTS:=_makefile) $(HEADERS_ONLY:=.hpp) Makefile LGPL*

	[[ -d $(DT_DIR)/$(EXAMPLES) ]] || mkdir $(DT_DIR)/$(EXAMPLES)
	cp --target-directory=$(DT_DIR)/$(EXAMPLES) $(EXAMPLES)/*[!~]
