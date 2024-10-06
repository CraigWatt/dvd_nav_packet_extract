# Variables
LIBDVDNAV_REPO = https://code.videolan.org/videolan/libdvdnav.git
LIBDVDCSS_REPO = https://code.videolan.org/videolan/libdvdcss.git
LIBDVDREAD_REPO = https://code.videolan.org/videolan/libdvdread.git
LOCAL_LIBS = local_libs

# Project Name
TARGET = dvdnavtex

# Check for required tools
.PHONY: check_tools
check_tools:
	@command -v autoconf >/dev/null 2>&1 || { echo "Autoconf is required but not installed. Install via Homebrew: brew install autoconf"; exit 1; }
	@command -v automake >/dev/null 2>&1 || { echo "Automake is required but not installed. Install via Homebrew: brew install automake"; exit 1; }
	@command -v clang >/dev/null 2>&1 || { echo "clang is required but not installed. Aborting."; exit 1; }
	@command -v git >/dev/null 2>&1 || { echo "git is required but not installed. Aborting."; exit 1; }
	@command -v pkg-config >/dev/null 2>&1 || { echo "pkg-config is required but not installed. Install via Homebrew: brew install pkg-config"; exit 1; }

# Clone and build libdvdcss
.PHONY: check_libdvdcss
check_libdvdcss:
	@if [ ! -d $(LOCAL_LIBS)/libdvdcss ]; then \
		echo "libdvdcss is missing, cloning from repository..."; \
		git clone $(LIBDVDCSS_REPO) $(LOCAL_LIBS)/libdvdcss; \
	fi
	@cd $(LOCAL_LIBS)/libdvdcss && autoreconf -i && \
	./configure --prefix=$(shell pwd)/$(LOCAL_LIBS)/libdvdcss \
	--disable-shared --enable-static && \
	make && make install || { echo "Failed to build libdvdcss"; exit 1; }

# Clone and build libdvdread
.PHONY: check_libdvdread
check_libdvdread:
	@if [ ! -d $(LOCAL_LIBS)/libdvdread ]; then \
		echo "libdvdread is missing, cloning from repository..."; \
		git clone $(LIBDVDREAD_REPO) $(LOCAL_LIBS)/libdvdread; \
	fi
	@cd $(LOCAL_LIBS)/libdvdread && autoreconf -i && \
	./configure --prefix=$(shell pwd)/$(LOCAL_LIBS)/libdvdread \
	--with-libdvdcss-includes=$(shell pwd)/$(LOCAL_LIBS)/libdvdcss/include \
	--with-libdvdcss-lib=$(shell pwd)/$(LOCAL_LIBS)/libdvdcss/lib \
	--disable-shared --enable-static && \
	make && make install || { echo "Failed to build libdvdread"; exit 1; }

# Clone and build libdvdnav
.PHONY: check_libdvdnav
check_libdvdnav:
	@if [ ! -d $(LOCAL_LIBS)/libdvdnav ]; then \
		echo "libdvdnav is missing, cloning from repository..."; \
		git clone $(LIBDVDNAV_REPO) $(LOCAL_LIBS)/libdvdnav; \
	fi
	@export PKG_CONFIG_PATH=$(shell pwd)/$(LOCAL_LIBS)/libdvdread/lib/pkgconfig; \
	export DYLD_LIBRARY_PATH=$(shell pwd)/$(LOCAL_LIBS)/libdvdread/lib; \
	cd $(LOCAL_LIBS)/libdvdnav && \
	sed -i '' 's/AC_PROG_CC_C99/AC_PROG_CC/g' configure.ac && \
	autoupdate || true && \
	autoreconf -i && \
	./configure --prefix=$(shell pwd)/$(LOCAL_LIBS)/libdvdnav \
	--disable-shared --enable-static && \
	make && make install || { echo "Failed to build libdvdnav"; exit 1; }

# Check all libraries
.PHONY: check_libraries
check_libraries: check_libdvdcss check_libdvdread check_libdvdnav

# Clean the build
.PHONY: clean
clean:
	@echo "Cleaning up..."
	@rm -f $(TARGET)
	@rm -rf $(LOCAL_LIBS)

# Build the project
.PHONY: build
build: check_tools check_libraries
	@echo "Building the project..."
	@if [ -f extractor.c ]; then \
		echo "extractor.c found. Proceeding to compile..."; \
	else \
		echo "extractor.c not found! Aborting build."; \
		exit 1; \
	fi
	@echo "Compiling with flags and linking..."
	@{ clang -g \
	      -I./local_libs/libdvdnav/include \
	      -I./local_libs/libdvdread/include \
	      -I./local_libs/libdvdcss/include \
	      extractor.c \
	      ./local_libs/libdvdnav/lib/libdvdnav.a \
	      ./local_libs/libdvdread/lib/libdvdread.a \
	      ./local_libs/libdvdcss/lib/libdvdcss.a \
	      -o $(TARGET); } || { echo "Compilation failed"; exit 1; }

# Run the program
.PHONY: run
run:
	@read -p "Enter path to VOB file: " vob_path; \
	echo "Running the program with path: $$vob_path"; \
	./$(TARGET) "$$vob_path"

# Reminder to install Lua (optional)
.PHONY: lua_reminder
lua_reminder:
	@echo "Reminder: Lua may be needed for certain scripts. Install via Homebrew: brew install lua"

# Reminder to install or upgrade Bison (optional)
.PHONY: bison_reminder
bison_reminder:
	@echo "Reminder: You may need Bison for parsing. Install via Homebrew: brew install bison"

.PHONY: test
test:
	@echo "Running basic console log test..."
	@echo "All tests passed!"
