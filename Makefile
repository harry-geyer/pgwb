PROJ_DIR:=.

SRC_DIR:=$(PROJ_DIR)/src
RESOURCE_DIR:=$(PROJ_DIR)/resources
BUILD_DIR:=$(PROJ_DIR)/build
STATIC_RESOURCE_DIR:=$(PROJ_DIR)/static_resources

OBJ_DIR:=$(BUILD_DIR)/objs
WEBROOT:=$(BUILD_DIR)/webroot
BUILT_RESOURCE_DIR:=$(BUILD_DIR)/resources

CC:=emcc
CFLAGS:=-O3
LDFLAGS:=
EMCCFLAGS:=-s NO_EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']"

SRCS:=$(shell find $(SRC_DIR) -type f)
RESOURCES:=$(shell find $(RESOURCE_DIR) -type f)
HTML:=$(WEBROOT)/index.html

OBJS:=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
BUILT_RESOURCES:=$(patsubst $(RESOURCE_DIR)/%,$(BUILT_RESOURCE_DIR)/%,$(RESOURCES))

default: $(HTML) $(BUILT_RESOURCES)

clean:
	rm -rf $(BUILD_DIR)

serve: default
	cd $(WEBROOT); \
	python3 -m http.server

$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<

$(HTML): $(OBJS) $(STATIC_RESOURCE_DIR)/shell_minimal.html
	@mkdir -p $(@D)
	$(CC) -o $@ $(LDFLAGS) --shell-file $(STATIC_RESOURCE_DIR)/shell_minimal.html $< $(EMCCFLAGS)

$(BUILT_RESOURCES): $(BUILT_RESOURCE_DIR)/%: $(RESOURCE_DIR)/%
	@mkdir -p $(@D)
	cp $< $@

