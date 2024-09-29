#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dvdread/dvd_reader.h>
#include <dvdread/ifo_read.h>
#include <dvdread/ifo_types.h>

#define CHUNK_SIZE 100 // Number of sectors per chunk

// Function to process the data read from the DVD (e.g., write to file)
void process_title_data(uint8_t *data, size_t data_size, unsigned int title_number) {
    // Example: Write the data to a file
    char filename[256];
    snprintf(filename, sizeof(filename), "title_%02u.vob", title_number);

    FILE *fp = fopen(filename, "ab"); // Use "ab" to append data if file exists
    if (!fp) {
        fprintf(stderr, "Failed to open file %s for writing\n", filename);
        return;
    }

    fwrite(data, 1, data_size, fp);
    fclose(fp);

    printf("Processed %zu bytes for Title %u\n", data_size, title_number);
}

// Function to check if a sector contains a NAV packet
int is_nav_packet(uint8_t *sector) {
    // NAV packets start with packet_start_code_prefix (0x000001) and stream_id (0xBF)
    return (sector[0] == 0x00 && sector[1] == 0x00 &&
            sector[2] == 0x01 && sector[3] == 0xBF);
}

// Function to process a buffer for NAV packets
void process_buffer_for_nav_packets(uint8_t *buffer, uint32_t sectors, unsigned int title_number) {
    for (uint32_t i = 0; i < sectors; i++) {
        uint8_t *sector = buffer + i * 2048;

        if (is_nav_packet(sector)) {
            // Process the NAV packet
            printf("Found NAV packet in Title %u at sector offset %u\n", title_number, i);
            // Additional processing can be done here
        }
    }
}

// Main function to parse IFO files and read VOB data
void parse_ifo_files(dvd_reader_t *dvd) {
    // Open VIDEO_TS.IFO
    ifo_handle_t *vmg_ifo = ifoOpen(dvd, 0);
    if (!vmg_ifo) {
        fprintf(stderr, "Failed to open VIDEO_TS.IFO.\n");
        return;
    }
    printf("Successfully opened VIDEO_TS.IFO\n");

    // Read TT_SRPT
    if (ifoRead_TT_SRPT(vmg_ifo) == 0 || vmg_ifo->tt_srpt == NULL) {
        fprintf(stderr, "Failed to read TT_SRPT from VIDEO_TS.IFO.\n");
        ifoClose(vmg_ifo);
        return;
    }

    printf("Number of Titles: %u\n", vmg_ifo->tt_srpt->nr_of_srpts);

    // Iterate over titles
    for (unsigned int i = 0; i < vmg_ifo->tt_srpt->nr_of_srpts; i++) {
        title_info_t *title = &vmg_ifo->tt_srpt->title[i];
        uint8_t title_set_nr = title->title_set_nr; // VTS number
        uint8_t vts_ttn = title->vts_ttn;           // Title Number within VTS

        printf("\nProcessing Title %u (VTS #%u, VTS_TTN #%u)\n", i + 1, title_set_nr, vts_ttn);

        // Open VTS_XX_0.IFO
        ifo_handle_t *vts_ifo = ifoOpen(dvd, title_set_nr);
        if (!vts_ifo) {
            fprintf(stderr, "Failed to open VTS_%02u_0.IFO.\n", title_set_nr);
            continue;
        }

        // Read PGCIT
        if (ifoRead_PGCIT(vts_ifo) == 0 || vts_ifo->vts_pgcit == NULL) {
            fprintf(stderr, "Failed to read PGC Information Table from VTS_%02u_0.IFO.\n", title_set_nr);
            ifoClose(vts_ifo);
            continue;
        }

        if (vts_ttn > vts_ifo->vts_pgcit->nr_of_pgci_srp) {
            fprintf(stderr, "VTS_TTN #%u is out of range in VTS_%02u_0.IFO.\n", vts_ttn, title_set_nr);
            ifoClose(vts_ifo);
            continue;
        }

        pgc_t *pgc = vts_ifo->vts_pgcit->pgci_srp[vts_ttn - 1].pgc;
        if (!pgc) {
            fprintf(stderr, "Failed to get PGC for VTS_TTN #%u in VTS_%02u_0.IFO.\n", vts_ttn, title_set_nr);
            ifoClose(vts_ifo);
            continue;
        }

        uint8_t num_cells = pgc->nr_of_cells;
        printf("Title %u has %u cells.\n", i + 1, num_cells);

        // Variables to hold the overall start and end sectors
        uint32_t title_start_sector = UINT32_MAX;
        uint32_t title_end_sector = 0;

        // Iterate over cells to find the start and end sectors
        for (uint8_t cell_idx = 0; cell_idx < num_cells; cell_idx++) {
            cell_playback_t *cell_playback = &pgc->cell_playback[cell_idx];
            uint32_t cell_start_sector = cell_playback->first_sector;
            uint32_t cell_end_sector = cell_playback->last_sector;

            printf("  Cell %u: Start Sector = %u, End Sector = %u\n",
                   cell_idx + 1, cell_start_sector, cell_end_sector);

            if (cell_start_sector < title_start_sector) {
                title_start_sector = cell_start_sector;
            }
            if (cell_end_sector > title_end_sector) {
                title_end_sector = cell_end_sector;
            }
        }

        // Calculate total sectors to read
        uint32_t num_sectors_to_read = title_end_sector - title_start_sector + 1;
        printf("Title %u overall: Start Sector = %u, End Sector = %u, Total Sectors = %u\n",
               i + 1, title_start_sector, title_end_sector, num_sectors_to_read);

        // Open VTS VOB files for reading
        dvd_file_t *vts_file = DVDOpenFile(dvd, title_set_nr, DVD_READ_TITLE_VOBS);
        if (!vts_file) {
            fprintf(stderr, "Failed to open VTS_%02u_X.VOB files.\n", title_set_nr);
            ifoClose(vts_ifo);
            continue;
        }

        // Open file for writing (or processing)
        char filename[256];
        snprintf(filename, sizeof(filename), "title_%02u.vob", i + 1);
        FILE *fp = fopen(filename, "wb");
        if (!fp) {
            fprintf(stderr, "Failed to open file %s for writing\n", filename);
            DVDCloseFile(vts_file);
            ifoClose(vts_ifo);
            continue;
        }

        // Read and process in chunks
        uint32_t sectors_read = 0;
        while (sectors_read < num_sectors_to_read) {
            uint32_t sectors_to_read = CHUNK_SIZE;
            if (sectors_read + sectors_to_read > num_sectors_to_read) {
                sectors_to_read = num_sectors_to_read - sectors_read;
            }

            uint8_t buffer[2048 * CHUNK_SIZE];
            if (DVDReadBlocks(vts_file, title_start_sector + sectors_read, sectors_to_read, buffer) != sectors_to_read) {
                fprintf(stderr, "Failed to read sectors for Title %u\n", i + 1);
                break;
            }

            // Write the buffer to file
            size_t bytes_written = fwrite(buffer, 2048, sectors_to_read, fp);
            if (bytes_written != sectors_to_read) {
                fprintf(stderr, "Failed to write data to file %s\n", filename);
                break;
            }

            // Optionally process the buffer for NAV packets
            process_buffer_for_nav_packets(buffer, sectors_to_read, i + 1);

            sectors_read += sectors_to_read;
            printf("Read and processed %u/%u sectors for Title %u\r", sectors_read, num_sectors_to_read, i + 1);
            fflush(stdout);
        }
        printf("\nTitle %u data written to %s\n", i + 1, filename);

        // Close file and VTS file
        fclose(fp);
        DVDCloseFile(vts_file);
        ifoClose(vts_ifo);
    }

    // Close the VMG IFO handle
    ifoClose(vmg_ifo);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <dvd_directory>\n", argv[0]);
        return 1;
    }

    // Set debug level (optional)
    // DVDReadSetDebugLevel(2);

    const char *dvd_directory = argv[1];

    // Open the DVD
    dvd_reader_t *dvd = DVDOpen(dvd_directory);
    if (!dvd) {
        fprintf(stderr, "Failed to open DVD at '%s'.\n", dvd_directory);
        return 1;
    }
    printf("Successfully opened DVD at '%s'.\n", dvd_directory);

    // Parse IFO files and extract data
    parse_ifo_files(dvd);

    // Close the DVD
    DVDClose(dvd);

    printf("Extraction completed.\n");
    return 0;
}
