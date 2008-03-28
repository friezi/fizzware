################################################################################
########################### FOR EDITING ########################################

TARGET = semaphoren

MAIN_MODULE = main
LOCAL_MODULES =
EXTERN_MODULES =
EM_PATH =

TT = ../../fztooltempl
ST = ../../fzsystools

CLASSLIBRARY_PATH = ../../lib
CLASSLIBRARIES = fztooltempl fzsystools
CLASS_MODULES_PATHS = $(TT) $(ST)
CLASS_MODULES = $(ST)/semaphor $(TT)/exception

IMPORTANT_HEADERS =

LIBRARY_INCLUDE_PATHS =
LIBRARIES =
LIBRARY_PATHS =

ADDITIONAL_DISTFILES =

# in case that ansi is not allowed
NOANSI = true

### check the field CC ########################################################
################################################################################
################################################################################
DIST_DIR = ../dist
DT_DIR = $(DIST_DIR)/$(TARGET)

# Default-editor if Env-Variable "EDITOR" is not defined
EDITOR ?= vi

ifeq ($(NOANSI),true)
     ANSI =
else
     ANSI = -ansi
endif

MM_OBJECT = $(MAIN_MODULE:=.o)
MM_SOURCE = $(MAIN_MODULE:=.cpp)
MM_HEADER = $(MAIN_MODULE:=.hpp)
LM_SOURCES = $(LOCAL_MODULES:=.cpp)
LM_HEADERS = $(LOCAL_MODULES:=.hpp)
LM_OBJECTS = $(LOCAL_MODULES:=.o)
EM_SOURCES = $(EXTERN_MODULES:%=$(EM_PATH)/%.cpp)
EM_HEADERS = $(EXTERN_MODULES:%=$(EM_PATH)/%.hpp)
EM_OBJECTS = $(EXTERN_MODULES:%=$(EM_PATH)/%.o)
CM_HEADERS = $(CLASS_MODULES:%=%.hpp)
IM_HEADERS = $(IMPORTANT_HEADERS:%=%.hpp)
INC_LI_PATHS = $(LIBRARY_INCLUDE_PATHS:%=-I%)
LIBS = $(LIBRARIES:%=-l%)
INC_L_PATHS = $(LIBRARY_PATHS:%=-L%)
CL_LIB = $(CLASSLIBRARIES:%=-l%)
INC_CL_PATH = $(CLASSLIBRARY_PATH:%=-L%)
INC_EM_PATH = $(EM_PATH:%=-I%)
INC_CM_PATHS = $(CLASS_MODULES_PATHS:%=-I%)
LIB_NAMES = $(CLASSLIBRARIES:%=$(CLASSLIBRARY_PATH)/lib%.a)

#bei systemspezifischer Programmierung "-ansi" ausschalten
CC = g++ -Wall -Wconversion $(ANSI) -pedantic -O3 $(INC_EM_PATH) $(INC_CM_PATHS) $(INC_LI_PATHS) $(INC_L_PATHS) $(INC_CL_PATH)

.PHONY: all clean all_libraries ed dist

# falls von lokaler Bibliothek abhaengig, soll diese erst generiert werden
all: all_libraries $(TARGET)

# <TARGET> compilieren
$(TARGET): $(MM_OBJECT) $(LM_OBJECTS) $(EM_OBJECTS) $(LIB_NAMES)
	$(CC) $(MM_OBJECT) $(LM_OBJECTS) $(EM_OBJECTS) -o $(TARGET) $(LIBS) $(CL_LIB)
	strip $(TARGET)

# Zeilen zaehlen, falls gewuenscht
#	wc -l $(MM_SOURCE) $(MM_HEADER) $(LM_SOURCES) $(LM_HEADERS) $(EM_SOURCES) $(EM_HEADERS)

# compilieren der Module 
%.o: %.cpp %.hpp $(IM_HEADERS)
	$(CC) $< -c -o $@

# <MAIN_MODULE> haengt von allen Header-files ab
$(MM_OBJECT): $(LM_HEADERS) $(EM_HEADERS) $(CM_HEADERS) $(IM_HEADERS)

# es koennen zusaezliche spezielle Abhaengigkeiten definiert werden


# Generierung der lokalen Bibliothek (ist so eingestellt, dass sie nur bei Aenderungen generiert wird)
all_libraries:
	$(foreach mklib,$(CLASS_MODULES_PATHS),$(MAKE) -k -C $(mklib) -f Makefile;)

$(LIB_NAMES):

# alle Module loeschen
clean:
	rm -f $(TARGET) *.o $(EM_OBJECTS)

# Editor aufrufen
ed:
	$(EDITOR) Makefile $(MAIN_MODULE:=.?pp) $(LOCAL_MODULES:=.?pp) $(EXTERN_MODULES:%=$(EM_PATH)/%.?pp) $(CLASS_MODULES:%=%.?pp) $(IM_HEADERS) &

# lokales Makefile neu generieren bei Aenderung des lokalen Makefile-Kopfes
Makefile: Makefile_head
	cat Makefile_head ../Makefile_body > Makefile;

# erstelle Packet fuer Distribution
dist:
	cd ../classes; make -k dist; cd -;
	[[ -d $(DIST_DIR) ]] || mkdir $(DIST_DIR)
	[[ -d $(DT_DIR) ]] || mkdir $(DT_DIR)
	cp --target-directory=$(DT_DIR) $(MAIN_MODULE:=.?pp) $(LOCAL_MODULES:=.?pp) $(EM_SOURCES) $(EM_HEADERS) Makefile Makefile_head \
		$(ADDITIONAL_DISTFILES)

