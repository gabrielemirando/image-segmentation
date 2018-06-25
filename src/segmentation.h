/*
 * File: segmentation.h
 * --------------------
 * Header file containing the declarations of the functions defined in
 * segmentation_serial.c and segmentation_omp.c
 */

#ifndef SEGMENTATION_H
#define SEGMENTATION_H

void kmeans_segm(byte_t *data, int n_pixels, int n_channels, int n_clusts, int *n_iters, double *sse);
void kmeans_segm_omp(byte_t *data, int n_pixels, int n_channels, int n_clusts, int *n_iters, double *sse, int n_threads);

#endif
