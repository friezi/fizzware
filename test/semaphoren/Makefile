################################################################################
########################### FOR EDITING ########################################

TARGET = semaphoren

MAIN_MODULE = main
LOCAL_MODULES =
EXTERN_MODULES =
EM_PATH =

CLASSLIBRARY_PATH = ../classes
CLASSLIBRARY = fztooltempl
CLASS_MODULES = semaphor exception
CM_PATH = $(CLASSLIBRARY_PATH)

IMPORTANT_HEADERS =

LIBRARY_INCLUDE_PATHS =
LIBRARIES =
LIBRARY_PATHS =

# in case that ansi is not allowed
NOANSI = true

### check the field CC ########################################################
################################################################################
################################################################################
DIST_DIR = ../dist
DT_DIR = $(DIST_DIR)/$(TARGET)

# EDITOR ?= vi

ifeq ($(origin EDITOR), undefined)
	EDITOR = vi
endif

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
CM_HEADERS = $(CLASS_MODULES:%=$(CM_PATH)/%.hpp)
CM_MAKEFILES = $(CLASS_MODULES:%=-f $(CM_PATH)/%/Makefile)
INC_LI_PATHS = $(LIBRARY_INCLUDE_PATHS:%=-I%)
LIBS = $(LIBRARIES:%=-l%)
INC_L_PATHS = $(LIBRARY_PATHS:%=-L%)
CL_LIB = $(CLASSLIBRARY:%=-l%)
INC_CL_PATH = $(CLASSLIBRARY_PATH:%=-L%)
INC_EM_PATH = $(EM_PATH:%=-I%)
INC_CM_PATH = $(CM_PATH:%=-I%)
LIB_NAME = $(CLASSLIBRARY:%=$(CLASSLIBRARY_PATH)/lib%.a)
MAKE_CL_LIB = $(CLASSLIBRARY_PATH:%=$(MAKE) -k -C $(CLASSLIBRARY_PATH) -f %/Makefile)

#bei systemspezifischer Programmierung "-ansi" ausschalten
CC = g++ -Wall -Wconversion $(ANSI) -pedantic -O3 $(INC_EM_PATH) $(INC_CM_PATH) $(INC_LI_PATHS) $(INC_L_PATHS) $(INC_CL_PATH)

.PHONY: all clean $(LIB_NAME)_all dist

# falls von lokaler Bibliothek abhaengig, soll diese erst generiert werden
all: $(LIB_NAME)_all $(TARGET)

# <TARGET> compilieren
$(TARGET): $(MM_OBJECT) $(LM_OBJECTS) $(EM_OBJECTS) $(LIB_NAME)
	$(CC) $(MM_OBJECT) $(LM_OBJECTS) $(EM_OBJECTS) -o $(TARGET) $(LIBS) $(CL_LIB)
	strip $(TARGET)

# Zeilen zählen, falls gewünscht
#	wc -l $(MM_SOURCE) $(MM_HEADER) $(LM_SOURCES) $(LM_HEADERS) $(EM_SOURCES) $(EM_HEADERS)

# compilieren der Module 
%.o: %.cpp %.hpp
	$(CC) $< -c -o $@

# <MAIN_MODULE> hängt von allen Header-files ab
$(MM_OBJECT): $(LM_HEADERS) $(EM_HEADERS) $(CM_HEADERS)

# es können zusäzliche spezielle Abhängigkeiten definiert werden


# Generierung der lokalen Bibliothek (ist so eingestellt, dass sie nur bei Aenderungen generiert wird
$(LIB_NAME)_all:
	$(MAKE_CL_LIB)

$(LIB_NAME):
# hier braucht nichts zu passieren, da die Library sowieso im Feld $(TARGET) generiert wird

# alle Module löschen
clean:
	rm -f $(TARGET) *.o $(EM_OBJECTS)

# Editor aufrufen
ed:
	$(EDITOR) Makefile $(MAIN_MODULE:=.?pp) $(LOCAL_MODULES:=.?pp) $(EXTERN_MODULES:%=$(EM_PATH)/%.?pp) $(CLASS_MODULES:%=$(CM_PATH)/%.?pp) $(IMPORTANT_HEADERS:%=%.hpp) &

# erstelle Packet fuer Distribution
dist:
	cd ../classes; make -k dist; cd -;
	[[ -d $(DIST_DIR) ]] || mkdir $(DIST_DIR)
	[[ -d $(DT_DIR) ]] || mkdir $(DT_DIR)
	cp --target-directory=$(DT_DIR) $(MAIN_MODULE:=.?pp) $(LOCAL_MODULES:=.?pp) $(EXTERN_MODULES:%=$(EM_PATH)/%.?pp) Makefile Makefile_head
