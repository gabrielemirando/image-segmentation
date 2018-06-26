/*
 * File: segmentation_serial.c
 * --------------------
 * This file represents the core of the program. It contains the serial
 * implementation of color-based segmentation using k-means clustering algorithm.
 *
 * MATRICES IMPLEMENTATION POLICY
 *
 * In this program, matrices have been implemented using unidimensional arrays.
 * According to this practice, a matrix of N rows and M columns is stored using
 * an array of size N * M. To get the (i,j) element of the matrix, we'll need to
 * access the [i * M + j] index of the array (since row-major ordering has been
 * applied). Adopting this solution, memory allocation is easy to implement
 * and the access to the elements of the matrix is fast, but may result
 * complicated in terms of code readability. In the comments, unidimensional
 * arrays used as matrices are directly referred as matrices.
 */

#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <omp.h>

#include "image_io.h"
#include "segmentation.h"

void initialize_centers(byte_t *data, double *centers, int n_px, int n_ch, int n_clus);
int find_closest(byte_t *pixel, double *centers, int n_ch, int n_clus, double *dist);
double sqr_distance(byte_t *pixel, double *center, int n_ch);
int find_farthest(double *dists, int n_px);
double sum_sqr_errors(double *dists, int n_px);
void update_data(byte_t *data, double *centers, int *labels, int n_px, int n_ch);

/*
 * Function:  kmeans_segm_omp
 * --------------------
 * Performs color-based segmentation on the image data using the OpenMP parallel
 * version k-means clustering algorithm.
 *
 *  byte_t *data      --  matrix containing the color values of the pixels of the image,
 *                        outputs the color values of the pixel after the segmentation
 *  int    n_px       --  number of pixels of the image
 *  int    n_ch       --  number of color components of the image
 *  int    n_clus     --  number of clusters to use for the segmentation
 *  int    *n_iters   --  number of maximum iterations for k-means algorithm,
 *                        outputs the numbers of iterations that took to complete
 *  double *sse       --  outputs the of sum of squared errors of k-means algorithm
 *  int    n_threads  --  number of threads to use
 */
void kmeans_segm_omp(byte_t *data, int n_px, int n_ch, int n_clus, int *n_iters, double *sse, int n_threads)
{
    int px, ch, k;
    int iter, max_iters, changes;
    int min_k, far_px;
    int *labels;     // indexes of the clusters to which each pixel belongs
    int *counts;     // number of pixels belonging to each cluster
    double *centers; // matrix of the clusters centers
    double *dists;   // distances of each pixel from the center of the cluster they belong

    max_iters = *n_iters;

    labels = malloc(n_px * sizeof(int));
    counts = malloc(n_clus * sizeof(int));
    centers = malloc(n_clus * n_ch * sizeof(double));
    dists = malloc(n_px * sizeof(double));

    omp_set_num_threads(n_threads);

    initialize_centers(data, centers, n_px, n_ch, n_clus);

    for (iter = 0; iter < max_iters; iter++) {
        changes = 0;

        // ASSIGN EACH PIXEL TO ITS CLOSEST CLUSTER

        #pragma omp parallel for schedule(static) private(px, min_k)
        for (px = 0; px < n_px; px++) {
            min_k = find_closest(&data[px * n_ch], centers, n_ch, n_clus, &dists[px]);

            if (labels[px] != min_k) {
                labels[px] = min_k;
                changes = 1;
            }
        }

        // CHECKING FOR COMPLETION

        if (!changes) {
            break;
        }

        // UPDATE CLUSTERS CENTERS

        // Resetting centers and clusters counters
        for (k = 0; k < n_clus; k++) {
            counts[k] = 0;
            for (ch = 0; ch < n_ch; ch++) {
                centers[k * n_ch + ch] = 0;
            }
        }

        // Computing partial sums of the centers and updating clusters counters
        for (px = 0; px < n_px; px++) {
            min_k = labels[px];
            counts[min_k]++;
            for (ch = 0; ch < n_ch; ch++) {
                centers[min_k * n_ch + ch] += data[px * n_ch + ch];
            }
        }

        // Dividing to obtain the centers mean
        for (k = 0; k < n_clus; k++) {
            if (counts[k]) {
                for (ch = 0; ch < n_ch; ch++) {
                    centers[k * n_ch + ch] /= counts[k];
                }
            } else {
                far_px = find_farthest(dists, n_px);

                for (ch = 0; ch < n_ch; ch++) {
                    centers[k * n_ch + ch] = data[far_px * n_ch + ch];
                }

                dists[far_px] = 0;
            }
        }
    }

    *n_iters = iter;

    *sse = sum_sqr_errors(dists, n_px);

    update_data(data, centers, labels, n_px, n_ch);

    free(centers);
    free(labels);
    free(counts);
    free(dists);
}

/*
 * Function: initialize_centers
 * --------------------
 * Initialize the clusters centers with the values of randomly selected pixels.
 *
 *  byte_t *data     -- matrix containing the color values of the pixels of the image
 *  double *centers  -- matrix of the clusters centers
 *  int    n_px      -- number of pixels of the image
 *  int    n_ch      -- number of color components of the image
 *  int    n_clus    -- number of clusters to use for segmentation
 */
void initialize_centers(byte_t *data, double *centers, int n_px, int n_ch, int n_clus)
{
    int k, ch, rnd;

    for (k = 0; k < n_clus; k++) {
        rnd = rand() % n_px;

        for (ch = 0; ch < n_ch; ch++) {
            centers[k * n_ch + ch] = data[rnd * n_ch + ch];
        }
    }
}

/*
 * Function: find_closest
 * --------------------
 * For a given pixel, finds the index of the closest cluster.
 *
 *  byte_t *pixel    -- array of the color values of a pixel
 *  double *centers  -- matrix of the clusters centers
 *  int    n_ch      -- number of color components of the image
 *  int    n_clus    -- number of clusters used for segmentation
 *  double *dist     -- outputs the distance from the closest cluster
 *
 *  return int       -- index of the closest cluster
 */
int find_closest(byte_t *pixel, double *centers, int n_ch, int n_clus, double *dist)
{
    int k, min_k;
    double tmp_dist, min_dist;

    min_dist = DBL_MAX;
    for (k = 0; k < n_clus; k++) {
        tmp_dist = sqr_distance(pixel, &centers[k * n_ch], n_ch);
        if (tmp_dist < min_dist) {
            min_dist = tmp_dist;
            min_k = k;
        }
    }

    *dist = min_dist;

    return min_k;
}

/*
 * Function: sqr_distance
 * --------------------
 * Computes the squared Euclidean distance between a pixel and a cluster center.
 *
 *  byte_t *pixel   -- array of the color values of a pixel
 *  double *center  -- array of the values of a cluster center
 *  int    n_ch     -- number of color components of the image
 *
 *  return double   -- squared Euclidean distance
 */
double sqr_distance(byte_t *pixel, double *center, int n_ch)
{
    int ch;
    double diff, dist = 0;

    for (ch = 0; ch < n_ch; ch++) {
        diff = (double)(pixel[ch] - center[ch]);
        dist += diff * diff;
    }

    return dist;
}

/*
 * Function: find_farthest
 * --------------------
 * Finds the index of the pixel which is farthest from the center of the cluster
 * to which it belongs.
 *
 *  double *dists  -- distances of each pixel from the center of the cluster they belong
 *  int    n_px    -- number of pixels of the image
 *
 *  return int     -- index of the pixel which is farthest from its cluster center
 */
int find_farthest(double *dists, int n_px)
{
    int px, far_px;
    double max_dist;

    max_dist = 0;
    for (px = 0; px < n_px; px++) {
        if (dists[px] > max_dist) {
            max_dist = dists[px];
            far_px = px;
        }
    }

    return far_px;
}

/*
 * Function: sum_sqr_errors
 * --------------------
 * Computes the Sum of Squared Errors of a cluster configuration.
 *
 *  double *dists  -- array of the distances of each pixel from its cluster center
 *  int    n_px    -- number of pixels of the image
 *
 *  return double  -- sum of squared rrrors
 */
double sum_sqr_errors(double *dists, int n_px)
{
    int px;
    double sse = 0;

    #pragma omp parallel for private(px) reduction(+:sse)
    for (px = 0; px < n_px; px++) {
        sse += dists[px];
    }

    return sse;
}

/*
 * Function: update_data
 * --------------------
 * Update the pixel data of the initial image according to the results of the
 * color-based segmentation using kmeans. The values of each pixel are replaced
 * with the ones of center of the cluster to which the pixel belongs.
 *
 *  byte_t *data     -- matrix containing the color values of the pixels of the image
 *  double *centers  -- matrix of the clusters centers
 *  int    *labels   -- indexes of the cluster to which each pixel belongs
 *  int    n_px      -- number of pixels of the image
 *  int    n_ch      -- number of color components of the image
 *  int    k_size    -- size of the clusters centers "array"
 */
void update_data(byte_t *data, double *centers, int *labels, int n_px, int n_ch)
{
    int px, ch, min_k;

    #pragma omp parallel for schedule(static) private(px, ch, min_k)
    for (px = 0; px < n_px; px++) {
        min_k = labels[px];

        for (ch = 0; ch < n_ch; ch++) {
            data[px * n_ch + ch] = (byte_t)round(centers[min_k * n_ch + ch]);
        }
    }
}
