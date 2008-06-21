################################################################################
################### erzeugt statische Bibliothek ###############################
################################################################################

LIBRARY = fztooltempl

MAJOR_VERSION = 2
MINOR_VERSION = 1alpha

VERSION_NUMBER = $(MAJOR_VERSION).$(MINOR_VERSION)

OBJECTS = exception primlist mathexpression cmdlparser propertyreader utils graph lex test

#old: 

HEADERS_ONLY = functions datastructures

EDITOR ?= vi

LIB_DIR = ../lib
DIST_DIR = ../dist
DT_DIR = $(DIST_DIR)/$(LIBRARY)

################################################################################

DOC = doc

CP = gcp

EXAMPLES = codeexamples

DOXY = $(DOC)/$(LIBRARY).doxyfile

OBJECTNAMES = $(OBJECTS:%=%.o)

IMPORTANT_HEADERS = $(HEADERS_ONLY:%=%.hpp)

DYN_SUFFIX = dylib
DYN_OBJECT_SUFFIX = dyn.o
DYN_OBJECTNAMES = $(OBJECTS:%=%.$(DYN_OBJECT_SUFFIX))

STATIC_LIB_NAME = $(LIB_DIR)/lib$(LIBRARY).a

#linux
DYNLIB_SONAME = $(LIB_DIR)/lib$(LIBRARY).$(DYN_SUFFIX).$(MAJOR_VERSION)
DYNAMIC_LIB_NAME = $(DYNLIB_SONAME).$(MINOR_VERSION)

#linux
#DYN_GCC = g++ -Wall -ansi -pedantic -shared -Wl,-soname,$(DYNLIB_SONAME) -o $(DYNAMIC_LIB_NAME)

#mac
LINKER_OPTIONS =  -Wl,-dylib -Wl,-dylib_compatibility_version,$(MAJOR_VERSION) -Wl,-dylib_current_version,$(MINOR_VERSION) -lc
DYN_GCC = g++ -Wall -ansi -pedantic -dynamic -fPIC -o $(LIBRARY)

MAKEFILE_CALLS = $(OBJECTS:%=%_makefile)

OBJ_START = true
OBJ_END = false

.PHONY: all clean ed cleandoc dist dynamic

all:	$(STATIC_LIB_NAME) $(DOC)

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
		$(DOC)/doxyfile >$(DOXY)

#		-e 's/^[ ]*\(PREDEFINED\)[ ]*=.*$$/\1 = DOXYGEN_SHOULD_SKIP_THIS/g'\

	doxygen $(DOXY)
	rm $(DOC)/doxyfile
	rm $(DOXY)
	@echo '****************' documentation generated '*****************'
	@echo

cleandoc:
	rm -r $(DOC)

$(STATIC_LIB_NAME): $(OBJECTNAMES) $(IMPORTANT_HEADERS)
	@echo
	@echo '****************' generating the static-library $(STATIC_LIB_NAME) '****************'
	$(foreach mk,$(MAKEFILE_CALLS),$(MAKE) -k -f $(mk);)
	$(AR) rs $(STATIC_LIB_NAME) $(OBJECTNAMES)
	@echo '****************' generation of $(STATIC_LIB_NAME) finished '***********************'
	@echo

%.o:%.cpp %.hpp
	@echo '***' compiling module $(@) for library  $(LIBRARY) '***'
	$(MAKE) -f $(@:%.o=%_makefile)
	@echo '*********************' finished '******************************'

$(DYN_OBJECTNAMES):

clean:	
	rm -f $(OBJECTNAMES)
	rm -f $(STATIC_LIB_NAME)
	rm -r -f $(DOC)

ed:
#	$(foreach mked,$(MAKEFILE_CALLS),$(MAKE) -k -f $(mked) ed;)
#	$(EDITOR) Makefile &
	$(EDITOR) $(OBJECTS:%=%.cpp) $(OBJECTS:%=%.hpp) $(HEADERS_ONLY:%=%.hpp) $(OBJECTS:%=%_makefile) Makefile &

dist:
	[[ -d $(DIST_DIR) ]] || mkdir $(DIST_DIR)
	[[ -d $(DT_DIR) ]] || mkdir $(DT_DIR)
	$(CP) --target-directory=$(DT_DIR) $(OBJECTS:=.?pp) $(OBJECTS:=_makefile) $(HEADERS_ONLY:=.hpp) Makefile README CHANGES LGPL*

	[[ -d $(DT_DIR)/$(EXAMPLES) ]] || mkdir $(DT_DIR)/$(EXAMPLES)
	$(CP) --target-directory=$(DT_DIR)/$(EXAMPLES) $(EXAMPLES)/*[!~]

dynamic: $(DYNAMIC_LIB_NAME)

$(DYNAMIC_LIB_NAME): $(DYN_OBJECTNAMES) $(IMPORTANT_HEADERS)
	@echo
	@echo '****************' generating the dynamic-library $(DYNAMIC_LIB_NAME) '****************'
	$(foreach mk,$(MAKEFILE_CALLS),$(MAKE) -k -f $(mk) dynamic;)
	$(DYN_GCC) $(DYN_OBJECTNAMES) $(LINKER_OPTIONS)
	@echo '****************' generation of $(DYNAMIC_LIB_NAME) finished '***********************'
	@echo
