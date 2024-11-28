# Makefile

CC = gcc
AR = ar
MKDIR_P = mkdir -p

INCLUDES = -Iincludes -Isrc

BUILD_DIR = build
RELEASE_DIR = $(BUILD_DIR)/release
DEBUG_DIR = $(BUILD_DIR)/debug

LIB_NAME = meteoswiss
STATIC_LIB = lib$(LIB_NAME).a
SHARED_LIB = lib$(LIB_NAME).so
PKGCONFIG_DIR = $(DESTDIR)/usr/lib/pkgconfig
PREFIX = /usr/local
LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include

SRC_DIR = src
TEST_DIR = test

CFLAGS = -DHTTP_WRAPPER_DESKTOP=1 $(INCLUDES)

VERSION = 0.0.1

# Release build flags
RELEASE_CFLAGS = -Werror -O3 -flto -fstack-protector-strong -fPIC -D_FORTIFY_SOURCE=2
RELEASE_LDFLAGS = -shared -flto -fPIC

# Debug build flags
DEBUG_CFLAGS = -O0 -ggdb -g3
DEBUG_LDFLAGS =

LIB_SOURCES = $(shell find $(SRC_DIR) -iname "*.c")
LIB_HEADERS = $(shell find $(SRC_DIR) -iname "*.h")

LIB_DEBUG_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(DEBUG_DIR)/obj/%.o, $(LIB_SOURCES))
LIB_RELEASE_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(RELEASE_DIR)/obj/%.o, $(LIB_SOURCES))

.PHONY: all
all: lib

.PHONY: test
test: $(DEBUG_DIR)/test/test_app
	@echo "Running tests..."
	$(DEBUG_DIR)/test/test_app

$(DEBUG_DIR)/test/test_app: $(DEBUG_DIR)/test/main.o $(DEBUG_DIR)/$(STATIC_LIB)
	$(MKDIR_P) $(DEBUG_DIR)/test
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o $@ $^ -lcurl

$(DEBUG_DIR)/test/main.o: $(TEST_DIR)/main.c
	$(MKDIR_P) $(DEBUG_DIR)/test
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -c $< -o $@

$(DEBUG_DIR)/$(STATIC_LIB): $(LIB_DEBUG_OBJECTS)
	$(MKDIR_P) $(DEBUG_DIR)
	$(AR) rcs $@ $^

$(DEBUG_DIR)/obj/%.o: $(SRC_DIR)/%.c $(LIB_HEADERS)
	$(MKDIR_P) $(DEBUG_DIR)/obj
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -c $< -o $@

.PHONY: lib
lib: $(RELEASE_DIR)/$(STATIC_LIB) $(RELEASE_DIR)/$(SHARED_LIB) $(RELEASE_DIR)/$(LIB_NAME).pc

$(RELEASE_DIR)/$(STATIC_LIB): $(LIB_RELEASE_OBJECTS)
	$(MKDIR_P) $(RELEASE_DIR)
	$(AR) rcs $@ $^

$(RELEASE_DIR)/$(SHARED_LIB): $(LIB_RELEASE_OBJECTS)
	$(MKDIR_P) $(RELEASE_DIR)
	$(CC) $(RELEASE_LDFLAGS) -o $@ $^

$(RELEASE_DIR)/obj/%.o: $(SRC_DIR)/%.c $(LIB_HEADERS)
	$(MKDIR_P) $(RELEASE_DIR)/obj
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS) -c $< -o $@

$(RELEASE_DIR)/$(LIB_NAME).pc: $(RELEASE_DIR)/$(STATIC_LIB) $(RELEASE_DIR)/$(SHARED_LIB)
	echo "Generating pkg-config file"
	$(MKDIR_P) $(RELEASE_DIR)
	echo 'prefix=$(PREFIX)' > $(RELEASE_DIR)/$(LIB_NAME).pc
	echo 'exec_prefix=$${prefix}' >> $(RELEASE_DIR)/$(LIB_NAME).pc
	echo 'libdir=$${exec_prefix}/lib' >> $(RELEASE_DIR)/$(LIB_NAME).pc
	echo 'includedir=$${prefix}/include/meteoswiss' >> $(RELEASE_DIR)/$(LIB_NAME).pc
	echo '' >> $(RELEASE_DIR)/$(LIB_NAME).pc
	echo 'Name: $(LIB_NAME)' >> $(RELEASE_DIR)/$(LIB_NAME).pc
	echo 'Description: MeteoSwiss API Library' >> $(RELEASE_DIR)/$(LIB_NAME).pc
	echo 'Version: $(VERSION)' >> $(RELEASE_DIR)/$(LIB_NAME).pc
	echo 'Requires: libcurl' >> $(RELEASE_DIR)/$(LIB_NAME).pc
	echo 'Cflags: -I$${includedir}' >> $(RELEASE_DIR)/$(LIB_NAME).pc
	echo 'Libs: -L$${libdir} -l:lib$(LIB_NAME).so' >> $(RELEASE_DIR)/$(LIB_NAME).pc
	echo 'Libs.private: -L$${libdir} -l:lib$(LIB_NAME).a' >> $(RELEASE_DIR)/$(LIB_NAME).pc

.PHONY: lib-debug
lib-debug: $(DEBUG_DIR)/$(STATIC_LIB)

.PHONY: install
install: lib
	@echo "Installing libraries and pkg-config file"
	$(MKDIR_P) $(DESTDIR)$(LIBDIR)
	$(MKDIR_P) $(DESTDIR)$(INCLUDEDIR)/meteoswiss 
	$(MKDIR_P) $(DESTDIR)$(LIBDIR)/pkgconfig
	# Install the static library
	install -m 644 $(RELEASE_DIR)/$(STATIC_LIB) $(DESTDIR)$(LIBDIR)
	# Install the shared library
	install -m 755 $(RELEASE_DIR)/$(SHARED_LIB) $(DESTDIR)$(LIBDIR)
	# Install the header files
	install -m 644 includes/*.h $(DESTDIR)$(INCLUDEDIR)/meteoswiss
	# Install the pkg-config file
	install -m 644 $(RELEASE_DIR)/$(LIB_NAME).pc $(DESTDIR)$(LIBDIR)/pkgconfig
	sudo ldconfig

.PHONY: uninstall
uninstall:
	rm -rf $(DESTDIR)$(INCLUDEDIR)/meteoswiss
	rm -f $(DESTDIR)$(LIBDIR)/$(STATIC_LIB)
	rm -f $(DESTDIR)$(LIBDIR)/$(SHARED_LIB)
	rm -f $(DESTDIR)$(LIBDIR)/pkgconfig/$(LIB_NAME).pc

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
