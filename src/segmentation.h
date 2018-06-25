/*
 * File: segmentation.h
 * --------------------
 * Header file containing the declarations of the functions defined in
 * segmentation_serial.c and segmentation_omp.c
 */

#ifndef SEGMENTATION_H
#define SEGMENTATION_H

/*
 * Function:  kmeans_segm
 * --------------------
 * Performs color-based segmentation on the image data provided using
 * the standard k-means clustering algorithm.
 *
 *  byte_t *data      --  array containing the values of pixels of the initial image,
 *                        outputs the values of the pixel after the segmentation
 *  int    n_pixels   --  number of pixels of the image
 *  int    n_channels --  number of color components of the image
 *  int    n_clusts   --  number of clusters to use for the segmentation
 *  int    *n_iters   --  number of maximum iterations for k-means algorithm,
 *                        outputs the numbers of iterations that took to complete
 *  double *sse       --  outputs the of sum of squared errors
 */
void kmeans_segm(byte_t *data, int n_pixels, int n_channels, int n_clusts, int *n_iters, double *sse);

/*
 * Function:  kmeans_segm_omp
 * --------------------
 * Parallel version of "kmeans_segm" using OpenMP. The additional parameter
 * n_threads is used to specify the number of threads to use.
 */
void kmeans_segm_omp(byte_t *data, int n_pixels, int n_channels, int n_clusts, int *n_iters, double *sse, int n_threads);

#endif
