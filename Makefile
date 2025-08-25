PROJ_DIR?=.

SRC_DIR:=$(PROJ_DIR)/src
LIB_DIR:=$(PROJ_DIR)/libs
RESOURCE_DIR:=$(PROJ_DIR)/resources
BUILD_DIR:=$(PROJ_DIR)/build
STATIC_RESOURCE_DIR:=$(PROJ_DIR)/static_resources
INT_INC_DIR:=$(PROJ_DIR)/src/internal

OBJ_DIR:=$(BUILD_DIR)/objs
WEBROOT:=$(BUILD_DIR)/webroot
BUILT_RESOURCE_DIR:=$(BUILD_DIR)/resources

CC:=emcc
CFLAGS:=-O3
CFLAGS+= -I$(INT_INC_DIR) -I$(LIB_DIR)
LDFLAGS:=-lglfw3
EMCCFLAGS:=-s NO_EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']" -s USE_WEBGL2=1 -s USE_GLFW=3 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2  --preload-file $(RESOURCE_DIR)


SRCS:=$(shell find $(SRC_DIR) -type f -name "*.c")
RESOURCES:=$(shell find $(RESOURCE_DIR) -type f)
HTML:=$(WEBROOT)/index.html

OBJS:=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

default: $(HTML) $(BUILT_RESOURCES)

clean:
	rm -rf $(BUILD_DIR)

serve: default
	cd $(WEBROOT); \
	python3 -m http.server

$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<

$(HTML): $(OBJS) $(STATIC_RESOURCE_DIR)/shell_minimal.html $(RESOURCES)
	@mkdir -p $(@D)
	$(CC) -o $@ $(LDFLAGS) --shell-file $(STATIC_RESOURCE_DIR)/shell_minimal.html $(OBJS) $(EMCCFLAGS)

