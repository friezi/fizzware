################################################################################
########################### FOR EDITING ########################################

TARGET = ringbuffer

# change if modules are in a deeper directory
ROOT_DIR = ../../

MAIN_MODULE = main
LOCAL_MODULES =
EXTERN_MODULES =
EM_PATH =

TT = $(ROOT_DIR)/fztooltempl

CLASSLIBRARY_PATH = $(ROOT_DIR)/lib/
CLASSLIBRARIES = fztooltempl
CLASS_MODULES_PATHS = $(TT)
CLASS_MODULES = $(TT)/exception

IMPORTANT_HEADERS = $(TT)/datastructures

LIBRARY_INCLUDE_PATHS =
LIBRARIES =
LIBRARY_PATHS =

ADDITIONAL_DISTFILES =

# in case that ansi is not allowed
# NOANSI = true

### check the field CC ########################################################
################################################################################
################################################################################
ROOT_DIR ?= ../

SRC = src
OBJ = build

DIST_DIR = $(ROOT_DIR)/dist
DT_DIR = $(DIST_DIR)/$(TARGET)
TESTENV_LIB = fztestenv
LIBTEST_DIR = $(ROOT_DIR)/$(TESTENV_LIB)/

# Default-editor if Env-Variable "EDITOR" is not defined
EDITOR ?= vi

ifeq ($(NOANSI),true)
     ANSI =
else
     ANSI = -ansi
endif

MM_OBJECT = $(MAIN_MODULE:%=$(OBJ)/%.o)
MM_SOURCE = $(MAIN_MODULE:%=$(SRC)/%.cpp)
MM_HEADER = $(MAIN_MODULE:%=$(SRC)/%.hpp)
LM_SOURCES = $(LOCAL_MODULES:%=$(SRC)/%.cpp)
LM_HEADERS = $(LOCAL_MODULES:%=$(SRC)/%.hpp)
LM_OBJECTS = $(LOCAL_MODULES:%=$(OBJ)/%.o)
EM_SOURCES = $(EXTERN_MODULES:%=$(EM_PATH)/%.cpp)
EM_HEADERS = $(EXTERN_MODULES:%=$(EM_PATH)/%.hpp)
EM_OBJECTS = $(EXTERN_MODULES:%=$(EM_PATH)/%.o)
CM_SOURCES = $(CLASS_MODULES:%=%.cpp)
CM_HEADERS = $(CLASS_MODULES:%=%.hpp)
IM_HEADERS = $(IMPORTANT_HEADERS:%=%.hpp)
INC_LI_PATHS = $(LIBRARY_INCLUDE_PATHS:%=-I%)
LIBS = $(LIBRARIES:%=-l%)
INC_L_PATHS = $(LIBRARY_PATHS:%=-L%)
CL_LIB = $(CLASSLIBRARIES:%=-l%)
INC_CL_PATH = $(CLASSLIBRARY_PATH:%=-L%)
INC_EM_PATH = $(EM_PATH:%=-I%)
LIB_NAMES = $(CLASSLIBRARIES:%=$(CLASSLIBRARY_PATH)/lib%.a)

MAINTEST = maintest

#bei systemspezifischer Programmierung "-ansi" ausschalten
CC = g++ -Wall -Wconversion $(ANSI) -pedantic -O3 -I$(ROOT_DIR) $(INC_EM_PATH) $(INC_LI_PATHS) $(INC_L_PATHS) $(INC_CL_PATH)

.PHONY: all clean all_libraries ed dist test compile_test run

# falls von lokaler Bibliothek abhaengig, soll diese erst generiert werden
all: all_libraries $(TARGET)

# <TARGET> compilieren
$(TARGET): Makefile $(MM_OBJECT) $(LM_OBJECTS) $(EM_OBJECTS) $(LIB_NAMES)
	$(CC) $(MM_OBJECT) $(LM_OBJECTS) $(EM_OBJECTS) -o $(TARGET) $(LIBS) $(CL_LIB)
	strip $(TARGET)

# Zeilen zaehlen, falls gewuenscht
#	wc -l $(MM_SOURCE) $(MM_HEADER) $(LM_SOURCES) $(LM_HEADERS) $(EM_SOURCES) $(EM_HEADERS)

# compilieren der Module 
$(OBJ)/%.o: $(SRC)/%.cpp $(SRC)/%.hpp $(IM_HEADERS)
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
	rm -f $(TARGET) $(OBJ)/*.o $(EM_OBJECTS)

compile_test: $(MAINTEST)
$(MAINTEST): $(OBJ)/$(MAINTEST).o $(LM_HEADERS)
	$(MAKE) -k -C $(LIBTEST_DIR) -f Makefile
	$(CC) -I$(LIBTEST_DIR) $(OBJ)/$(MAINTEST).o -o $(MAINTEST) $(LIBS) $(CL_LIB) -l$(TESTENV_LIB)

test: all_libraries compile_test
	$(MAINTEST)

run: $(TARGET)
	$(TARGET)

# Editor aufrufen
ed:	Makefile
	$(EDITOR) Makefile_head $(MM_SOURCE) $(MM_HEADER) $(LM_SOURCES) $(LM_HEADERS) $(EM_SOURCES) $(EM_HEADERS) $(CM_SOURCES) $(CM_HEADERS) $(IM_HEADERS) $(SRC)/$(MAINTEST).?pp &

# lokales Makefile neu generieren bei Aenderung des lokalen Makefile-Kopfes
Makefile: Makefile_head $(ROOT_DIR)/Makefile_body
	cat Makefile_head $(ROOT_DIR)/Makefile_body > Makefile;

# erstelle Packet fuer Distribution
dist:
	cd ../classes; make -k dist; cd -;
	[[ -d $(DIST_DIR) ]] || mkdir $(DIST_DIR)
	[[ -d $(DT_DIR) ]] || mkdir $(DT_DIR)
	[[ -d $(DT_DIR)/$(SRC) ]] || mkdir $(DT_DIR)/$(SRC)
	[[ -d $(DT_DIR)/$(OBJ) ]] || mkdir $(DT_DIR)/$(OBJ)
	cp --target-directory=$(DT_DIR)/$(SRC) $(MM_SOURCE) $(MM_HEADER) $(LM_SOURCES) $(LM_HEADERS)
	cp --target-directory=$(DT_DIR) $(EM_SOURCES) $(EM_HEADERS) Makefile Makefile_head $(ADDITIONAL_DISTFILES)
