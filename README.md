# DVD Navigation Packet Extractor

## Overview

**DVD Navigation Packet Extractor** is a powerful command-line tool designed to parse DVD IFO files, extract detailed title and cell information, and pinpoint the exact sectors within VOB files where each title and cell starts. This facilitates efficient extraction and processing of specific DVD content, such as NAV packets, video streams, audio tracks, and subtitles.

## Features

- **Parse DVD IFO Files**: Reads `VIDEO_TS.IFO` and `VTS_XX_0.IFO` files to extract comprehensive title and cell information.
- **Sector Mapping**: Identifies and maps the start and end sectors for each title and cell within the VOB files.
- **Data Extraction**: Reads data from VOB files based on sector ranges and writes them into separate `.vob` files for each title.
- **NAV Packet Detection**: Scans for NAV packets within the extracted data to retrieve navigation information.
- **Cross-Platform Support**: Compatible with macOS systems and utilizes `libdvdread` and `libdvdcss` libraries.
- **Efficient Processing**: Reads data in chunks to manage memory usage effectively, making it suitable for processing large DVDs.

## Table of Contents

- [Installation](#installation)
- [Build Instructions](#build-instructions)
  - [Prerequisites](#prerequisites)
  - [Build](#build)
  - [Clean Build](#clean-build)
- [Usage](#usage)
  - [Syntax](#syntax)
  - [Example](#example)
- [Example Output](#example-output)
- [Dependencies](#dependencies)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Installation

### Prerequisites

Before installing **DVD Navigation Packet Extractor**, ensure you have the following prerequisites installed on your system:

- **C Compiler**: GCC or Clang.
- **Git**: For cloning repositories.
- **Make**: For building the project.
- **libdvdread**, **libdvdnav**, and **libdvdcss**: Libraries for reading and decrypting DVD content.

### Clone the Repository

Begin by cloning the repository to your local machine:

```bash
git clone https://github.com/yourusername/dvd_nav_packet_extract.git
cd dvd_nav_packet_extract

