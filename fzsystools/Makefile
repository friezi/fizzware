################################################################################
################### erzeugt statische Bibliothek ###############################
################################################################################

LIBRARY = fzsystools

MAJOR_VERSION = 1
MINOR_VERSION = 0

VERSION_NUMBER = $(MAJOR_VERSION).$(MINOR_VERSION)

OBJECTS = messagequeue socket tcpserver tcpclient msgqueueregclient semaphor signalhandler stdmessageserver

ROOT_DIR = ../
LSRC = src

#old: 

TT = $(ROOT_DIR)/fztooltempl

HEADERS_ONLY = $(TT)/$(LSRC)/exception $(TT)/$(LSRC)/datastructures

EDITOR ?= vi

LIB_DIR = $(ROOT_DIR)/lib
DIST_DIR = $(ROOT_DIR)/dist
DT_DIR = $(DIST_DIR)/$(LIBRARY)

################################################################################

SRC = $(LSRC)
OBJ = build
OBJECT_MAKEFILES_DIR = makefiles

DOC = doc

EXAMPLES = codeexamples

DOXY = $(DOC)/$(LIBRARY).doxyfile

OBJECTNAMES = $(OBJECTS:%=$(OBJ)/%.o)
SOURCES = $(OBJECTS:%=$(SRC)/%.cpp)
HEADERS = $(OBJECTS:%=$(SRC)/%.hpp)
IMPORTANT_HEADERS = $(HEADERS_ONLY:%=%.hpp)

LIB_NAME = $(LIB_DIR)/lib$(LIBRARY).a

OBJECT_MAKEFILES = $(OBJECTS:%=%_makefile)
OBJECT_MAKEFILE_PATHS = $(OBJECT_MAKEFILES:%=$(OBJECT_MAKEFILES_DIR)/%)

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
		-e 's/^[ ]*\(OUTPUT_DIRECTORY\)[ ]*=.*$$/\1 = \.\.\/$(DOC)/g'\
		-e 's/^[ ]*\(EXAMPLE_PATH\)[ ]*=.*$$/\1 = \.\.\/$(EXAMPLES)/g'\
		$(DOC)/doxyfile >$(DOXY)

#		-e 's/^[ ]*\(PREDEFINED\)[ ]*=.*$$/\1 = DOXYGEN_SHOULD_SKIP_THIS/g'\

	cd $(SRC) && doxygen ../$(DOXY)
	rm $(DOXY)

	rm $(DOC)/doxyfile
	@echo '****************' documentation generated '*****************'
	@echo

cleandoc:
	rm -r $(DOC)

$(LIB_NAME): $(OBJECTNAMES) $(IMPORTANT_HEADERS)
	@echo
	@echo '****************' generating the static-library $(LIB_NAME) '****************'
	$(foreach mk,$(OBJECT_MAKEFILES),$(MAKE) -k -C $(OBJECT_MAKEFILES_DIR) -f $(mk);)
	$(AR) rs $(LIB_NAME) $(OBJECTNAMES)
	@echo '****************' generation of $(LIB_NAME) finished '***********************'
	@echo

$(OBJ)/%.o:$(SRC)/%.cpp $(SRC)/%.hpp
	@echo '***' compiling module $(@) for static-library  $(LIB_NAME) '***'
	$(MAKE) -k -C $(OBJECT_MAKEFILES_DIR) -f $(subst $(OBJ)/,,$(@:%.o=%_makefile))
	@echo '*********************' finished '******************************'

clean:	
	rm -f $(OBJECTNAMES)
	rm -f $(LIB_NAME)
	rm -r -f $(DOC)

ed:
#	$(foreach mked,$(OBJECT_MAKEFILES),$(MAKE) -k -C $(OBJECT_MAKEFILES_DIR) -f $(mked) ed;)
#	$(EDITOR) Makefile &
	$(EDITOR) $(SOURCES) $(HEADERS) $(OBJECT_MAKEFILES_PATHS) $(IMPORTANT_HEADERS) Makefile &

dist:
	[[ -d $(DIST_DIR) ]] || mkdir $(DIST_DIR)
	[[ -d $(DT_DIR) ]] || mkdir $(DT_DIR)
	cp --target-directory=$(DT_DIR) $(OBJECTS:=.?pp) $(OBJECTS:=_makefile) $(HEADERS_ONLY:=.hpp) Makefile README CHANGES LGPL*

	[[ -d $(DT_DIR)/$(EXAMPLES) ]] || mkdir $(DT_DIR)/$(EXAMPLES)
	cp --target-directory=$(DT_DIR)/$(EXAMPLES) $(EXAMPLES)/*[!~]
