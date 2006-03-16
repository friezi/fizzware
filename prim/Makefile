TARGET = prim

MAIN_MODULE = main
LOCAL_MODULES = 
EXTERN_MODULES = primlist exception

EM_PATH = ../classes

MM_OBJECT = $(MAIN_MODULE:=.o)
MM_SOURCE = $(MAIN_MODULE:=.cpp)
MM_HEADER = $(MAIN_MODULE:=.hpp)
LM_SOURCES = $(LOCAL_MODULES:=.cpp)
LM_HEADERS = $(LOCAL_MODULES:=.hpp)
LM_OBJECTS = $(LOCAL_MODULES:=.o)
EM_SOURCES = $(EXTERN_MODULES:%=$(EM_PATH)/%.cpp)
EM_HEADERS = $(EXTERN_MODULES:%=$(EM_PATH)/%.hpp)
EM_OBJECTS = $(EXTERN_MODULES:%=$(EM_PATH)/%.o)

EDITOR ?= vi

DIST_DIR = ../dist
DT_DIR = $(DIST_DIR)/$(TARGET)

#bei systemspezifischer Programmierung "-ansi" ausschalten
CC = g++ -Wall -Wconversion -ansi -pedantic -O3 -I $(EM_PATH)

all: $(TARGET)

# <TARGET> compilieren
$(TARGET): $(MM_OBJECT) $(LM_OBJECTS) $(EM_OBJECTS)
	$(CC) $(MM_OBJECT) $(LM_OBJECTS) $(EM_OBJECTS) -o $(TARGET)
	strip $(TARGET)

# Zeilen zählen, falls gewünscht
#	wc -l $(MM_SOURCE) $(MM_HEADER) $(LM_SOURCES) $(LM_HEADERS) $(EM_SOURCES) $(EM_HEADERS)

# compilieren der Module 
%.o: %.cpp %.hpp
	$(CC) $< -c -o $@

# <MAIN_MODULE> hängt von allen Header-files ab
$(MM_OBJECT): $(LM_HEADERS) $(EM_HEADERS)

# es können zusäzliche spezielle Abhängigkeiten definiert werden

.PHONY: all clean ed dist

# alle Module löschen
clean:
	rm $(TARGET) *.o $(EM_OBJECTS)

#Editor aufrufen
ed:
	$(EDITOR) Makefile $(MAIN_MODULE:=.?pp) $(LOCAL_MODULES:=.?pp) $(EXTERN_MODULES:%=$(EM_PATH)/%.?pp) &

# erstelle Packet fuer Distribution
dist:
	cd ../classes; make -k dist; cd -;
	[[ -d $(DIST_DIR) ]] || mkdir $(DIST_DIR)
	[[ -d $(DT_DIR) ]] || mkdir $(DT_DIR)
	cp --target-directory=$(DT_DIR) $(MAIN_MODULE:=.?pp) $(LOCAL_MODULES:=.?pp) $(EXTERN_MODULES:%=$(EM_PATH)/%.?pp) Makefile
