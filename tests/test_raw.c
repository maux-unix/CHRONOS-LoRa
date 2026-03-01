#include <stdio.h>
#include <stdlib.h>

const int xres = 256;
const int yres = 256;
const int bpp = 3; // 3 bytes per pixel (R, G, B)

int main() {
    unsigned char *imagedata = malloc(xres * yres * bpp);
    FILE *infile;

    // Open file in binary read mode
    infile = fopen("image.raw", "rb");
    if (!infile) {
        perror("Error opening file");
        return 1;
    }

    // Read the entire image data into the buffer
    size_t elements_read = fread(imagedata, 1, xres * yres * bpp, infile);

    if (elements_read != xres * yres * bpp) {
        fprintf(stderr, "Error reading file or file size mismatch\n");
    }

    // Close the file
    fclose(infile);

    // You can now process the imagedata array (e.g., display it with a graphics library)

    // Free the allocated memory
    free(imagedata);

    return 0;
}
