################################################################################
########################### FOR EDITING ########################################

TARGET = focal

MAIN_MODULE = main
LOCAL_MODULES =
EXTERN_MODULES =

EM_PATH = ../classes

CLASSLIBRARY_PATH = $(EM_PATH)
CLASSLIBRARY = fztooltempl
CLASS_MODULES =  mathexpression exception

IMPORTANT_HEADERS =

LIBRARY_INCLUDE_PATHS = /usr/include/readline
LIBRARIES = readline
LIBRARY_PATHS = /usr/lib

EDITOR = emacs

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
CM_HEADERS = $(CLASS_MODULES:%=$(EM_PATH)/%.hpp)
CM_MAKEFILES = $(CLASS_MODULES:%=-f $(CLASSLIBRARY_PATH)/%/Makefile)
LI_PATHS = $(LIBRARY_INCLUDE_PATHS:%=-I %)
LIBS = $(LIBRARIES:%=-l%)
L_PATHS = $(LIBRARY_PATHS:%=-L %)
CL_LIB = $(CLASSLIBRARY:%=-l%)
CL_PATH = $(CLASSLIBRARY_PATH:%=-L %)
LIB_NAME = $(CLASSLIBRARY:%=$(CLASSLIBRARY_PATH)/lib%.a)
MAKE_CL_LIB = $(CLASSLIBRARY_PATH:%=$(MAKE) -k -C $(CLASSLIBRARY_PATH) -f %/Makefile)

#bei systemspezifischer Programmierung "-ansi" ausschalten
CC = g++ -Wall -Wconversion -pedantic -O3 -I $(EM_PATH) $(LI_PATHS) $(L_PATHS) $(CL_PATH)

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
	rm $(TARGET) *.o $(EM_OBJECTS)

#Editor aufrufen
ed:
	$(EDITOR) Makefile $(MAIN_MODULE:=.?pp) $(LOCAL_MODULES:=.?pp) $(EXTERN_MODULES:%=$(EM_PATH)/%.?pp) $(CLASS_MODULES:%=$(CLASSLIBRARY_PATH)/%.?pp) $(IMPORTANT_HEADERS) &

# erstelle Packet fuer Distribution
dist:
	cd ../classes; make -k dist; cd -;
	[[ -d $(DIST_DIR) ]] || mkdir $(DIST_DIR)
	[[ -d $(DT_DIR) ]] || mkdir $(DT_DIR)
	cp --target-directory=$(DT_DIR) $(MAIN_MODULE:=.?pp) $(LOCAL_MODULES:=.?pp) $(EXTERN_MODULES:%=$(EM_PATH)/%.?pp) Makefile
