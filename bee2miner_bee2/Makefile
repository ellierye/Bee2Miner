
ifndef BUILD_DIR
BUILD_DIR:= .
$(warning BUILD_DIR is not set, setting it to $(BUILD_DIR))
endif

ifndef SRC_DIR
SRC_DIR:= .
$(warning SRC_DIR is not set, setting it to $(SRC_DIR))
endif

# target location for binaries
OBJS_DIR = $(BUILD_DIR)/objs
BIN_DIR = $(BUILD_DIR)/bin
TARGET_DIRS := $(OBJS_DIR) $(BIN_DIR)

ARCH := $(shell uname -m)
ifeq ($(ARCH), ppc) 
	CC=gcc
endif
ifeq ($(ARCH), x86_64) 
	ifeq ($(NOXC), 1)
		CC=gcc -DNOXC
	else 
		CC=/opt/ppc/bin/powerpc-405-linux-gnu-gcc	
	endif

endif

ifndef USER
USER = root
$(warning USER is not set, setting it to $(USER))
endif

ifndef BEE2HOST
BEE2HOST = bee2-sm
$(warning BEE2HOST is not set, setting it to $(BEE2HOST))
endif

.SUFFIXES: .c .o

TARGETS :=	minernet simpletest

HEADER_FILES = 	4q_ppc_user_logic.h 		\
		coredrv.h 			\
		4q_bee2lib.h 			\
		4q_jtag.h                       \
		quadrwlib.h                     \
		simplenet.h                     \


LIBS = $(OBJS_DIR)/4q_bee2lib.o 
LIBS += $(OBJS_DIR)/4q_jtag.o 
LIBS += $(OBJS_DIR)/simplenet.o 
LIBS += $(OBJS_DIR)/quadrwlib.o

SRCS = $(addsuffix .c, $(TARGETS)) $(LIBS,.o=.c)
TARGETSBIN =$(addprefix $(BIN_DIR)/,$(TARGETS))

OBJS = $(addprefix objs/, $(SRCS,.c=.o))
CFLAGS = -g -Wall
.PRECIOUS: $(OBJS_DIR)/%.o 


# search path for src files
vpath %.c $(SRC_DIR)
vpath %.h $(SRC_DIR)
#vpath %.sh $(SRC_DIR)
vpath %.o $(OBJS_DIR)

#################################### RULES #####################################
all: $(TARGETSBIN)

$(TARGET_DIRS):
	@echo
	@echo Creating target directory: $@
	@echo ==================================================
	mkdir $@


$(BIN_DIR)/%.sh: %.sh
	@echo
	@echo copying bash script file $@
	@echo ==================================================
	cp $< $@
	chmod 777 $@

$(BIN_DIR)/% : %.o $(LIBS) $(BIN_DIR)
	@echo
	@echo Generating executable file $@
	@echo ==================================================
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@

$(OBJS_DIR)/%.o : %.c $(HEADER_FILES) $(OBJS_DIR)
	@echo
	@echo Generating object file $@
	@echo ==================================================
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm objs/*.o
	-rm $(BIN_DIR)/*

upload: all
	@echo 
	@echo Uploading binaries to user $(USER)@bee2-sm:4q_software/bin
	@echo =============================================================
	ssh $(USER)@$(BEE2HOST) "rm -rf 4q_software; mkdir 4q_software; mkdir 4q_software/bin;"
	scp -r -p $(BIN_DIR)/* $(USER)@$(BEE2HOST):4q_software/bin

cleanbee2:
	ssh $(USER)@bee2-sm "rm -rf 4q_software;"

.PHONY: all clean cleanbee2 help

