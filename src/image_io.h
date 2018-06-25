/*
 * File: image_io.h
 * --------------------
 * Headers file containing the declarations of the functions defined in image_io.c
 */

#ifndef IMAGE_IO_H
#define IMAGE_IO_H

/*
 * Type Definition: byte_t
 * --------------------
 * The intensity of each channel (red, green, blue or gray) of a pixel is a
 * value from 0 to 255. One byte of memory is sufficient to store it and
 * that's why 'unsigned char' type is used. For convenience 'unsigned char'
 * is renamed to 'byte_t'.
 */
typedef unsigned char byte_t;

/*
 * Function: img_load
 * --------------------
 * Opens an image and loads the values of its pixels into an array.
 *
 *  char *img_file    -- filepath of the image to open
 *  int  *width       -- outputs the width of the image in pixels
 *  int  *height      -- outputs the height of the image in pixels
 *  int  *n_channels  -- outputs the number of color components of the image
 *
 *  return byte_t*    -- an array allocated in memory containing the values
 *                       of each color channel for all the pixels of the image
 */
byte_t *img_load(char *img_file, int *width, int *height, int *n_channels);

/*
 * Function: img_save
 * --------------------
 * Creates a new image file using the pixels values provided.
 *
 *  char   *img_file   -- filepath of the image to create
 *  byte_t *data       -- array containing the color values for the pixels
 *  int    width       -- width of the image in pixels
 *  int    height      -- height of the image in pixels
 *  int    n_channels  -- number of color components of the image
 */
void img_save(char *img_file, byte_t *data, int width, int height, int n_channels);

#endif
