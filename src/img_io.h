#ifndef IMG_IO_H
#define IMG_IO_H

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
 * Function:  img_load
 * --------------------
 * Opens an image and loads the values of its pixels.
 *
 *  img_file:   path of the image to open
 *  width:      will contain the width of the image (passed by reference)
 *  height:     will contain the height of the image (passed by reference)
 *  n_channels: will contain the number of color channels
 *              of the image (passed by reference)
 *
 *  returns:    an array allocated in memory containing the values
 *              of each color channel for all the pixels of the image.
 */
byte_t *img_load(char *img_file, int *width, int *height, int *n_channels);

/*
 * Function:  img_save
 * --------------------
 * Creates a new JPEG image using the pixels values provided.
 *
 *  img_file:   path of the image to create
 *  data:       array containing the values of each color
 *              channel for all the pixels of the image
 *  width:      width of the image
 *  height:     height of the image
 *  n_channels: number of color channels of the image
 */
void img_save(char *img_file, byte_t *data, int width, int height, int n_channels);

#endif
