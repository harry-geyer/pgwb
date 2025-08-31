PROJ_DIR?=.

SRC_DIR:=$(PROJ_DIR)/src
LIB_DIR:=$(PROJ_DIR)/libs
RESOURCE_DIR:=$(PROJ_DIR)/resources
BUILD_DIR:=$(PROJ_DIR)/build
STATIC_RESOURCE_DIR:=$(PROJ_DIR)/static_resources
STATIC_ASSET_DIR=$(STATIC_RESOURCE_DIR)/assets
INT_INC_DIR:=$(PROJ_DIR)/src/internal

OBJ_DIR:=$(BUILD_DIR)/objs
WEBROOT:=$(BUILD_DIR)/webroot
BUILT_RESOURCE_DIR:=$(BUILD_DIR)/resources

HTML_TEMPLATE:=$(STATIC_RESOURCE_DIR)/template.html

CC:=emcc
CFLAGS:=-O3
CFLAGS+= -I$(INT_INC_DIR) -I$(LIB_DIR)
CFLAGS+=-Wall -Werror -pedantic
LDFLAGS:=-lglfw3
EMCCFLAGS:=-s NO_EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']" -s USE_WEBGL2=1 -s USE_GLFW=3 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2  --preload-file $(RESOURCE_DIR)


SRCS:=$(shell find $(SRC_DIR) -type f -name "*.c")
RESOURCES:=$(shell find $(RESOURCE_DIR) -type f)
HTML:=$(WEBROOT)/index.html
ASSET_SRCS:=$(shell find $(STATIC_ASSET_DIR) -type f)
ASSETS:=$(patsubst $(STATIC_ASSET_DIR)/%,$(WEBROOT)/assets/%,$(ASSET_SRCS))

OBJS:=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

default: $(HTML) $(ASSETS)

clean:
	rm -rf $(BUILD_DIR)

serve: default
	cd $(WEBROOT); \
	python3 -m http.server

$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<

$(HTML): $(OBJS) $(HTML_TEMPLATE) $(RESOURCES)
	@mkdir -p $(@D)
	$(CC) -o $@ $(LDFLAGS) --shell-file $(HTML_TEMPLATE) $(OBJS) $(EMCCFLAGS)

$(ASSETS): $(WEBROOT)/assets/%: $(STATIC_ASSET_DIR)/%
	@mkdir -p $(@D)
	cp $< $@

