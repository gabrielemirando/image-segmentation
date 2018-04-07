#ifndef KMEANS_H
#define KMEANS_H

/*
 * Function:  rand_centers
 * --------------------
 * Select a set of centers at random from the pixels of the image.
 *
 *  data:       array containing the values of each color channel for all the
 *              pixels of the image
 *  n_pixels:   number of pixels of the image
 *  n_channels: number of channels of the image
 *  n_clusts:   number of clusters
 *
 *  returns:    an array containing the initial set of centers choosed
 *              selecting at random different pixels
 */
double *rand_centers(byte_t *data, int n_pixels, int n_channels, int n_clusts);

/*
* Function:  sqr_distance
* --------------------
* Calculates the squared euclidean distance between one pixel and a center.
*
*  pixel:      array containing the channel values of a pixel
*  center:     array containing the channel values of a cluster center
*  n_channels: number of channels of the image
*
*  returns:    squared euclidean distance between a pixel and a cluster center
*/
double sqr_distance(byte_t *pixel, double *center, int n_channels);

/*
 * Function:  closest_clust
 * --------------------
 * Discovers the cluster center closest to a given pixel.
 *
 *  pixel:      array containing the channel values of a pixel
 *  centers:    array containing the values of the clusters centers
 *  n_clusts:   number of clusters
 *  n_channels: number of channels of the image
 *  clust_dist: will contain the distance to the closest cluster (passed by reference)
 *
 *  returns:    label of the cluster closest to the given pixel
 */
int closest_clust(byte_t *pixel, double *centers, int n_clusts, int n_channels, double *clust_dist);

/*
* Function:  farthest_pixel
* --------------------
* Discovers the pixel that is farthest to the center of its cluster.
*
*  dists:      array containing the distances of each pixel to the center of its cluster
*  n_pixels:   number of pixels of the image
*
*  returns:    number of the pixel that is farthest to the center of its cluster
*/
int farthest_pixel(double *dists, int n_pixels);

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
