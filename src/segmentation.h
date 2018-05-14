#ifndef SEGMENTATION_H
#define SEGMENTATION_H

/*
 * Function:  kmeans_segm
 * --------------------
 * Performs color-based segmentation on the image data provided using
 * the standard k-means clustering algorithm.
 *
 *  data:       array containing the values of pixels of the image. It will
 *              contain image data after the segmentation has been performed
 *  n_pixels:   number of pixels of the image
 *  n_channels: number of channels of the image
 *  n_clusts:   number of clusters
 *  n_iters:    pointer to the number of maximum iterations of the kmeans algorithm.
 *              It will contain the numbers of iterations that took to complete
 *  sse:        will contain the of sum of squared errors produced by k-means algorithm
 */
void kmeans_segm(byte_t *data, int n_pixels, int n_channels, int n_clusts,
                 int *n_iters, double *sse);

/*
 * Function:  kmeans_segm_omp
 * --------------------
 * Variant of "kmeans_segm" using OpenMP.
 *
 *  n_threads: number of threads to use
 */
void kmeans_segm_omp(byte_t *data, int n_pixels, int n_channels, int n_clusts,
                     int *n_iters, double *sse, int n_threads);

/*
 * Function:  kmeans_segm_cuda
 * --------------------
 * Variant of "kmeans_segm" using CUDA.
 */
void kmeans_segm_cuda(byte_t *data, int n_pixels, int n_channels, int n_clusts,
                      int *n_iters, double *sse);

#endif
