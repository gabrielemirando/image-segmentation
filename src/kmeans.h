#ifndef KMEANS_H
#define KMEANS_H

/*
 * Function:  kmeans
 * --------------------
 * Executes the "standard" kmeans clustering algorithm on data of pixel provided.
 *
 *  data:       array containing the values of each color channel for all the
 *              pixels of the image
 *  n_pixels:   number of pixels of the image
 *  n_channels: number of channels of the image
 *  n_clusts:   number of clusters
 *  max_iters:  number of maximum iterations that the algorithm is allowed
 *              to do before it's stopped if still hasn't converged
 *  sse:        will be used to contain the of sum of squared errors of
 *              the resulting cluster configuration (passed by reference)
 *  iters:      will be used to contain the numbers of iterations that
 *              took the algorithm to complete (passed by reference)
 */
void kmeans(byte_t *data, int n_pixels, int n_channels, int n_clusts,
            int max_iters, double *sse, int *iters);

/*
 * Function:  kmeans_omp
 * --------------------
 * Variant of "kmeans" using OpenMP.
 *
 *  n_threads: the number of threads to use for multithreading. At the end of
 *             the execution it will contain the actual number of threads
 *             given to the program by the system
 */
void kmeans_omp(byte_t *data, int n_pixels, int n_channels, int n_clusts,
                int max_iters, double *sse, int *iters, int *n_threads);

/*
 * Function:  kmeans_cuda
 * --------------------
 * Variant of "kmeans" using CUDA.
 */
void kmeans_cuda(byte_t *data, int n_pixels, int n_channels, int n_clusts,
                 int max_iters, double *sse, int *iters);

#endif
