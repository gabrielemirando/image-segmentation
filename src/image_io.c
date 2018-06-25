/*
 * File: image_io.c
 * --------------------
 * Contains the functions for handling image I/O. It is just a "wrapper"
 * for the stb_image.h and stb_image_write.h libraries.
 */

#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../libs/stb_image.h"
#include "../libs/stb_image_write.h"

#include "image_io.h"

/*
 * Function: img_load
 * --------------------
 * Opens an image and loads the values of its pixels into a matrix.
 *
 *  char *img_file    -- filepath of the image to open
 *  int  *width       -- outputs the width of the image in pixels
 *  int  *height      -- outputs the height of the image in pixels
 *  int  *n_channels  -- outputs the number of color components of the image
 *
 *  return byte_t*    -- a matrix allocated in memory containing the values
 *                       of each color channel of all the pixels of the image
 */
byte_t *img_load(char *img_file, int *width, int *height, int *n_channels)
{
    byte_t *data;

    data = stbi_load(img_file, width, height, n_channels, 0);
    if (data == NULL) {
        fprintf(stderr, "ERROR LOADING IMAGE: << Invalid file name or format >> \n");
        exit(EXIT_FAILURE);
    }

    return data;
}

/*
 * Function: img_save
 * --------------------
 * Creates a new image file using the pixels values provided.
 *
 *  char   *img_file   -- filepath of the image to create
 *  byte_t *data       -- matrix containing the color values of the pixels
 *  int    width       -- width of the image in pixels
 *  int    height      -- height of the image in pixels
 *  int    n_channels  -- number of color components of the image
 */
void img_save(char *img_file, byte_t *data, int width, int height, int n_channels)
{
    char *ext;

    ext = strrchr(img_file, '.');

    if (!ext) {
        fprintf(stderr, "ERROR SAVING IMAGE: << Unspecified format >> \n\n");
        return;
    }

    if ((strcmp(ext, ".jpeg") == 0) || (strcmp(ext, ".jpg") == 0)) {
        stbi_write_jpg(img_file, width, height, n_channels, data, 100);
    } else if (strcmp(ext, ".png") == 0) {
        stbi_write_png(img_file, width, height, n_channels, data, width * n_channels);
    } else if (strcmp(ext, ".bmp") == 0) {
        stbi_write_bmp(img_file, width, height, n_channels, data);
    } else if (strcmp(ext, ".tga") == 0) {
        stbi_write_tga(img_file, width, height, n_channels, data);
    } else {
        fprintf(stderr, "ERROR SAVING IMAGE: << Unsupported format >> \n\n");
    }
}
