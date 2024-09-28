# dvd_nav_packet_extract

## Overview

This project extracts timestamps from DVD navigation packets in `.vob` files using VLC and DVDnav libraries.

## Requirements

You will need the following dependencies:
- VLC (LibVLC)
- DVDnav (Libdvdnav)

The installation of these dependencies can be automated for macOS and Linux via the provided `Makefile`.

## Installation

### macOS (Homebrew) and Linux (apt-get)

To install the necessary dependencies and compile the project:

1. **Clone the repository**:
    ```bash
    git clone <repository-url>
    cd dvd_nav_packet_extract
    ```

2. **Install dependencies**:
    Run the following command to install VLC and DVDnav:
    ```bash
    make install
    ```

3. **Compile the project**:
    After installing the dependencies, compile the program with:
    ```bash
    make
    ```

### Windows

For Windows, you will need to manually install VLC and DVDnav using a package manager like `vcpkg`, or download them from their respective sources:
- [VLC Development Libraries](https://www.videolan.org/vlc/download-windows.html)
- [Libdvdnav](https://code.videolan.org/videolan/libdvdnav)

Once installed, you can compile the project using MinGW or a similar environment.

## Running the Program

To run the program on a `.vob` file:

```bash
make run VOB_PATH=/path/to/your/file.vob
