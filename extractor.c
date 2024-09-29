#include <stdio.h>
#include <stdlib.h>
#include "dvdnav.h"

void extract_segments(const char *vob_file) {
    DVDNavHandle *dvdnav_handle;
    DVDNavStatus status;
    unsigned int vobu_s_ptm, vobu_e_ptm, c_eltm;

    // Initialize DVD navigation
    dvdnav_handle = dvdnav_open_file(vob_file, &status);
    if (dvdnav_handle == NULL) {
        fprintf(stderr, "Error opening DVD navigation.\n");
        return;
    }

    // Loop to read navigation packets
    while ((status = dvdnav_read_nav_packet(dvdnav_handle)) == DVDNAV_OK) {
        // Extract the required fields from the packet
        dvdnav_get_vobu_s_ptm(dvdnav_handle, &vobu_s_ptm);
        dvdnav_get_vobu_e_ptm(dvdnav_handle, &vobu_e_ptm);
        dvdnav_get_c_eltm(dvdnav_handle, &c_eltm);

        // Logic to determine if a segment change has occurred
        if (/* condition to detect segment change */) {
            // Log the timestamp to timestamps.txt
            FILE *file = fopen("timestamps.txt", "a");
            if (file) {
                fprintf(file, "Segment change detected at PTM: %u\n", vobu_s_ptm);
                fclose(file);
            }
        }
    }

    // Clean up
    dvdnav_close(dvdnav_handle);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path_to_VOB_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    extract_segments(argv[1]);
    return EXIT_SUCCESS;
}
