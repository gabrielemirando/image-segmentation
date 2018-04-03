#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "../libs/stb_image.h"
#include "../libs/stb_image_write.h"

#include "img_io.h"
#include "utils.h"

byte_t *img_load(char *img_file, int *width, int *height, int *n_channels)
{
    byte_t *data;
    char *error_msg;

    data = stbi_load(img_file, width, height, n_channels, 0);
    if (data == NULL) {
        error_msg = "ERROR LOADING IMAGE: << %s >> \n";
        fprintf(stderr, error_msg, stbi_failure_reason());
        exit(EXIT_FAILURE);
    }

    return data;
}

void img_save(char *img_file, byte_t *data, int width, int height, int n_channels)
{
    char *ext;

    ext = file_extension(img_file);

    if (strings_equal(ext, "jpeg") || strings_equal(ext, "jpg")) {
        stbi_write_jpg(img_file, width, height, n_channels, data, 100);
    } else if (strings_equal(ext, "png")) {
        stbi_write_png(img_file, width, height, n_channels, data, width * n_channels);
    } else if (strings_equal(ext, "bmp")) {
        stbi_write_bmp(img_file, width, height, n_channels, data);
    } else if (strings_equal(ext, "tga")) {
        stbi_write_tga(img_file, width, height, n_channels, data);
    } else {
        fprintf(stderr, "ERROR SAVING IMAGE: << Unsupported format >> \n\n");
    }
}
