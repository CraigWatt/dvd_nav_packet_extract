# Detect the operating system
UNAME := $(shell uname -s)
ARCH := $(shell uname -m)

# Default values
CC = clang
CFLAGS = 
LDFLAGS = 
TARGET = extractor
SRC = extractor.c

# Detect OS and architecture
ifeq ($(UNAME), Darwin)  # macOS
    ifeq ($(ARCH), arm64)  # macOS (M1/ARM)
        CFLAGS += -I/opt/homebrew/include
        LDFLAGS += -L/opt/homebrew/lib -lvlc -ldvdnav
    else  # macOS (Intel/x86)
        CFLAGS += -I/usr/local/include
        LDFLAGS += -L/usr/local/lib -lvlc -ldvdnav
    endif
endif

ifeq ($(UNAME), Linux)
    CFLAGS += -I/usr/include
    LDFLAGS += -L/usr/lib -lvlc -ldvdnav
endif

# For Windows, check for MSYS or MinGW environment
ifeq ($(UNAME), MINGW32_NT)  # Windows 32-bit
    CFLAGS += -IC:/path/to/vlc/include
    LDFLAGS += -LC:/path/to/vlc/lib -lvlc -ldvdnav
endif

ifeq ($(UNAME), MINGW64_NT)  # Windows 64-bit
    CFLAGS += -IC:/path/to/vlc/include
    LDFLAGS += -LC:/path/to/vlc/lib -lvlc -ldvdnav
endif

# Install dependencies based on the operating system
install:
ifeq ($(UNAME), Darwin)
	@if [ "$(ARCH)" = "arm64" ]; then \
		echo "Installing dependencies for macOS (M1/ARM)..."; \
		brew install vlc libdvdnav; \
	else \
		echo "Installing dependencies for macOS (Intel/x86)..."; \
		brew install vlc libdvdnav; \
	fi
endif

ifeq ($(UNAME), Linux)
	@echo "Installing dependencies for Linux..."; \
	sudo apt-get install -y vlc libvlc-dev libdvdnav-dev
endif

ifeq ($(UNAME), MINGW32_NT)
	@echo "Please manually install VLC and libdvdnav for Windows 32-bit."
endif

ifeq ($(UNAME), MINGW64_NT)
	@echo "Please manually install VLC and libdvdnav for Windows 64-bit."
endif

# Default target: build the executable
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean up the build artifacts
clean:
	rm -f $(TARGET)

# Run the program (usage: make run VOB_PATH=/path/to/file.vob)
run: $(TARGET)
	./$(TARGET) $(VOB_PATH)
