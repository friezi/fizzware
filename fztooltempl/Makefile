################################################################################
################### erzeugt statische Bibliothek ###############################
################################################################################

LIBRARY = fztooltempl

VERSION_NUMBER = 1.81

OBJECTS = exception bashautomat messagequeue socket tcpserver tcpclient msgqueueregclient primlist \
	semaphor process mathexpression signalhandler stdmessageserver cmdlparser propertyreader

#old: 

HEADERS_ONLY = functions storage graph

EDITOR = emacs

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
	sed 's/PROJECT_NAME[ ]\+=/PROJECT_NAME = $(LIBRARY)/g' $(DOC)/doxyfile\
	| sed 's/PROJECT_NUMBER[ ]\+=/PROJECT_NUMBER = $(VERSION_NUMBER)/g'\
	| sed 's/QUIET[ ]\+=[ ]\+NO/QUIET = YES/g'\
	| sed 's/EXTRACT_PRIVATE[ ]\+=[ ]\+NO/EXTRACT_PRIVATE = YES/g'\
	| sed 's/ALPHABETICAL_INDEX[ ]\+=[ ]\+NO/ALPHABETICAL_INDEX = YES/g'\
	| sed 's/WARN_IF_UNDOCUMENTED[ ]\+=[ ]\+YES/WARN_IF_UNDOCUMENTED = NO/g'\
	| sed 's/INTERNAL_DOCS[ ]\+=[ ]\+NO/INTERNAL_DOCS = YES/g'\
	| sed 's/SORT_MEMBER_DOCS[ ]\+=[ ]\+YES/SORT_MEMBER_DOCS = NO/g'\
	| sed 's/OUTPUT_DIRECTORY[ ]\+=/OUTPUT_DIRECTORY = $(DOC)/g'\
	| sed 's/EXAMPLE_PATH[ ]\+=/EXAMPLE_PATH = $(EXAMPLES)/g'\
	| sed 's/PREDEFINED[ ]\+=/PREDEFINED = DOXYGEN_SHOULD_SKIP_THIS/g' >$(DOXY)
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
	make -f $(@:%.o=%_makefile)
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
