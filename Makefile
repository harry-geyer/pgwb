
PROJ_DIR:=.

SRC_DIR:=$(PROJ_DIR)/src
BUILD_DIR:=$(PROJ_DIR)/build
OBJ_DIR:=$(BUILD_DIR)/objs
WEBROOT:=$(BUILD_DIR)/webroot

CC:=emcc
CFLAGS:=-O3
LDFLAGS:=

SRCS:=$(shell find $(SRC_DIR) -type f)
OBJS:=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
HTML:=$(WEBROOT)/index.html

default: $(HTML)

clean:
	rm -rf $(BUILD_DIR)

serve: default
	cd $(WEBROOT); \
	python3 -m http.server

$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $(CFLAGS) $<

$(HTML): $(OBJS)
	@mkdir -p $(@D)
	$(CC) -o $@ $(LDFLAGS) $<

