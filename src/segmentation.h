/*
 * Name:   segmentation.h
 * Author: Gabriele Mirando
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
 *  data:       array containing the values of pixels of the image. It will
 *              be modified after the segmentation
 *  n_pixels:   number of pixels of the image
 *  n_channels: number of channels of the image
 *  n_clusts:   number of clusters to use for the segmentation
 *  n_iters:    pointer to the number of maximum iterations of the kmeans algorithm.
 *              It will contain the numbers of iterations that took to complete.
 *  sse:        will contain the of sum of squared errors produced by k-means algorithm
 */
void kmeans_segm(byte_t *data, int n_pixels, int n_channels, int n_clusts, int *n_iters, double *sse);

/*
 * Function:  kmeans_segm_omp
 * --------------------
 * Variant of "kmeans_segm" using OpenMP. The additional parameter n_threads is
 * used to specify the number of threads to use.
 */
void kmeans_segm_omp(byte_t *data, int n_pixels, int n_channels, int n_clusts, int *n_iters, double *sse, int n_threads);

#endif
