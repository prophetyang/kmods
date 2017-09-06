MAKE		= $(shell which make)
MKDIR		= $(shell which mkdir)
RM		= $(shell which rm)
CP		= $(shell which cp)

TARGET_DIRS 	= module user
OUTPUT_DIR	= output

all: create_output build_dirs

build_dirs:
	@for dir in $(TARGET_DIRS); do \
		cd $$dir && $(MAKE) ; \
		cd .. ; \
	done

clean_dirs:
	@for dir in $(TARGET_DIRS); do \
		cd $$dir && $(MAKE) clean ; \
		cd .. ; \
	done

clean: clean_output clean_dirs

create_output:
	@[ -d $(OUTPUT_DIR) ] || $(MKDIR) $(OUTPUT_DIR)

clean_output:
	@$(RM) -rf $(OUTPUT_DIR)

