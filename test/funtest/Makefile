################################################################################
########################### FOR EDITING ########################################

TARGET = funtest

MAIN_MODULE = main
LOCAL_MODULES = 
EXTERN_MODULES =
IMPORTANT_HEADERS = ../classes/functions.hpp

EM_PATH = ../classes

LIBRARY_INCLUDE_PATHS = $(EM_PATH)
LIBRARIES = fztooltempl
LIBRARY_PATHS = $(EM_PATH)

EDITOR ?= vi

DIST_DIR = ../dist
DT_DIR = $(DIST_DIR)/$(TARGET)

### check the field CC ########################################################
################################################################################
################################################################################

MM_OBJECT = $(MAIN_MODULE:=.o)
MM_SOURCE = $(MAIN_MODULE:=.cpp)
MM_HEADER = $(MAIN_MODULE:=.hpp)
LM_SOURCES = $(LOCAL_MODULES:=.cpp)
LM_HEADERS = $(LOCAL_MODULES:=.hpp)
LM_OBJECTS = $(LOCAL_MODULES:=.o)
EM_SOURCES = $(EXTERN_MODULES:%=$(EM_PATH)/%.cpp)
EM_HEADERS = $(EXTERN_MODULES:%=$(EM_PATH)/%.hpp)
EM_OBJECTS = $(EXTERN_MODULES:%=$(EM_PATH)/%.o)
LI_PATHS = $(LIBRARY_INCLUDE_PATHS:%=-I %)
LIBS = $(LIBRARIES:%=-l%)
LIB_PATHS = $(LIBRARY_PATHS:%=-L %) 

#bei systemspezifischer Programmierung "-ansi" ausschalten
CC = g++ -Wall -Wconversion -ansi -pedantic -O3 -I $(EM_PATH) $(LI_PATHS) $(LIB_PATHS)

all: $(TARGET)

# <TARGET> compilieren
$(TARGET): $(MM_OBJECT) $(LM_OBJECTS) $(EM_OBJECTS)
	$(CC) $(MM_OBJECT) $(LM_OBJECTS) $(EM_OBJECTS) -o $(TARGET) $(LIBS)
	strip $(TARGET)

# Zeilen zählen, falls gewünscht
#	wc -l $(MM_SOURCE) $(MM_HEADER) $(LM_SOURCES) $(LM_HEADERS) $(EM_SOURCES) $(EM_HEADERS)

# compilieren der Module 
%.o: %.cpp %.hpp
	$(CC) $< -c -o $@

# <MAIN_MODULE> hängt von allen Header-files ab
$(MM_OBJECT): $(LM_HEADERS) $(EM_HEADERS)

# es können zusäzliche spezielle Abhängigkeiten definiert werden
main.o:	$(EM_PATH)/functions.hpp

.PHONY: all clean dist ed

# alle Module löschen
clean:
	rm $(TARGET) *.o $(EM_OBJECTS)

#Editor aufrufen
ed:
	$(EDITOR) Makefile $(MAIN_MODULE:=.?pp) $(LOCAL_MODULES:=.?pp) $(EXTERN_MODULES:%=$(EM_PATH)/%.?pp) $(IMPORTANT_HEADERS) &

# erstelle Packet fuer Distribution
dist:
	cd ../classes; make -k dist; cd -;
	[[ -d $(DIST_DIR) ]] || mkdir $(DIST_DIR)
	[[ -d $(DT_DIR) ]] || mkdir $(DT_DIR)
	cp --target-directory=$(DT_DIR) $(MAIN_MODULE:=.?pp) $(LOCAL_MODULES:=.?pp) $(EXTERN_MODULES:%=$(EM_PATH)/%.?pp) Makefile
