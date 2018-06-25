/*
 * File: segmentation_serial.c
 * --------------------
 * Contains the serial implementation of color-based segmentation using
 * k-means clustering algorithm.
 *
 * MATRICES DISCLAIMER
 *
 * In this program, matrices have been implemented using unidimensional array.
 * According to this practice, a matrix of N rows and M columns is stored using
 * an array of size N * M. To get the (i,j) element of the matrix, we'll need to
 * access the [i * M + j] index of the array (since row-major ordering has been
 * applied). Adopting this solution, memory allocation is easy to implement
 * and the access to the elements of the matrix is fast, but may result
 * complicated in terms of code readability. In presence of loops, temporary
 * buffers have been used to reduce the number of multiplications to do to
 * access the matrix elements. In the comments, unidimensional arrays used as
 * matrices are directly referred as matrices.
 */

#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "image_io.h"
#include "segmentation.h"

void initialize_centers(byte_t *data, double *centers, int n_pixels, int n_channels, int n_clusts);
int closest_cluster(byte_t *pixel, double *centers, int n_channels, int n_clusts, double *clust_dist);
double sqr_distance(byte_t *pixel, double *center, int n_channels);
int farthest_pixel(double *dists, int n_pixels);
double sum_sqr_errors(double *dists, int n_pixels);
void update_data(byte_t *data, double *centers, int *labels, int n_pixels, int n_channels, int k_size);

/*
 * Function:  kmeans_segm
 * --------------------
 * Performs color-based segmentation on the image data using the standard
 * k-means clustering algorithm.
 *
 *  byte_t *data      --  matrix containing the values of pixels of the initial image,
 *                        outputs the values of the pixel after the segmentation
 *  int    n_pixels   --  number of pixels of the image
 *  int    n_channels --  number of color components of the image
 *  int    n_clusts   --  number of clusters to use for the segmentation
 *  int    *n_iters   --  number of maximum iterations for k-means algorithm,
 *                        outputs the numbers of iterations that took to complete
 *  double *sse       --  outputs the of sum of squared errors of k-means algorithm
 */
void kmeans_segm(byte_t *data, int n_pixels, int n_channels, int n_clusts, int *n_iters, double *sse)
{
    int px, ch, k, j;    // general purpose counters
    int px_off, k_off;   // buffers to speed up matrix accesses
    int iter;            // number of the current iteration of the algorithm
    int max_iters;       // maximum number of iterations of the algorithm
    int changes;         // flag for changes in the pixels assigment to clusters
    int min_k;           // buffer of the index of the cluster closest to a pixel
    int far_px;          // buffer of the index of the pixel farthest from its cluster center
    int *labels;         // clusters to which each pixel belongs
    double *centers;     // matrix of the clusters centers
    int k_size;          // size of the clusters centers "array"
    double *dists;       // distances of each pixel from the center of the cluster they belong
    int *counts;         // number of pixels belonging to each cluster

    max_iters = *n_iters;

    k_size = n_clusts * n_channels;

    centers = malloc(k_size * sizeof(double));
    labels = malloc(n_pixels * sizeof(int));
    dists = malloc(n_pixels * sizeof(double));
    counts = malloc(n_clusts * sizeof(int));

    initialize_centers(data, centers, n_pixels, n_channels, n_clusts);

    for (iter = 0; iter < max_iters; iter++) {
        changes = 0;

        // ASSIGN EACH PIXEL TO ITS CLOSEST CLUSTER

        for (px = 0; px < n_pixels; px++) {
            px_off = px * n_channels;

            min_k = closest_cluster(&data[px_off], centers, n_channels, n_clusts, &dists[px]);

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
        for (j = 0; j < k_size; j++) {
            centers[j] = 0;
        }

        for (k = 0; k < n_clusts; k++) {
            counts[k] = 0;
        }

        // Computing partial sums of the centers
        for (px = 0; px < n_pixels; px++) {
            min_k = labels[px];

            k_off = min_k * n_channels;
            px_off = px * n_channels;
            for (ch = 0; ch < n_channels; ch++) {
                centers[k_off + ch] += data[px_off + ch];
            }

            counts[min_k]++;
        }

        // Dividing to obtain the centers mean
        for (k = 0; k < n_clusts; k++) {
            k_off = k * n_channels;

            if (counts[k]) {
                for (ch = 0; ch < n_channels; ch++) {
                    centers[k_off + ch] /= counts[k];
                }
            } else {
                far_px = farthest_pixel(dists, n_pixels);

                px_off = far_px * n_channels;
                for (ch = 0; ch < n_channels; ch++) {
                    centers[k_off + ch] = data[px_off + ch];
                }

                dists[far_px] = 0;
            }
        }
    }

    *n_iters = iter;

    *sse = sum_sqr_errors(dists, n_pixels);

    update_data(data, centers, labels, n_pixels, n_channels, k_size);

    free(centers);
    free(labels);
    free(counts);
    free(dists);
}

/*
 * Function: initialize_centers
 * --------------------
 * Initialize the clusters centers with randomly selected pixels from the dataset.
 *
 *  byte_t *data       -- matrix of the pixels of the image
 *  double *centers    -- matrix of the clusters centers
 *  int    n_pixels    -- number of pixels of the image
 *  int    n_channels  -- number of color components of the image
 *  int    n_clusts    -- number of clusters
 */
void initialize_centers(byte_t *data, double *centers, int n_pixels, int n_channels, int n_clusts)
{
    int k, ch;
    int rnd;
    int k_off, rnd_off;

    for (k = 0; k < n_clusts; k++) {
        rnd = rand() % n_pixels;

        k_off = k * n_channels;
        rnd_off = rnd * n_channels;
        for (ch = 0; ch < n_channels; ch++) {
            centers[k_off + ch] = data[rnd_off + ch];
        }
    }
}

/*
 * Function: closest_cluster
 * --------------------
 * Finds the index of the cluster closer to a given pixel.
 *
 *  byte_t *pixel         -- array of the color values of a pixel
 *  double *centers       -- matrix of the clusters centers
 *  int    n_channels     -- number of color components of the image
 *  int    n_clusts       -- number of clusters
 *  double *clust_dist    -- outputs the squared distance from the closest cluster
 *
 *  return int            -- index of the closest cluster
 */
int closest_cluster(byte_t *pixel, double *centers, int n_channels, int n_clusts, double *clust_dist)
{
    int k;
    int min_k;
    double dist;
    double min_dist;

    min_dist = DBL_MAX;
    for (k = 0; k < n_clusts; k++) {
        dist = sqr_distance(pixel, &centers[k * n_channels], n_channels);
        if (dist < min_dist) {
            min_dist = dist;
            min_k = k;
        }
    }

    *clust_dist = min_dist;

    return min_k;
}

/*
 * Function: sqr_distance
 * --------------------
 * Computes their squared Euclidean distance between a pixel and a cluster center.
 *
 *  byte_t *pixel      -- array of the color values of a pixel
 *  double *center     -- array of the color values of a cluster center
 *  int    n_channels  -- number of color components of the image
 *
 *  return double      -- squared Euclidean distance
 */
double sqr_distance(byte_t *pixel, double *center, int n_channels)
{
    int ch;
    double diff;
    double dist = 0;

    for (ch = 0; ch < n_channels; ch++) {
        diff = (double)(pixel[ch] - center[ch]);
        dist += diff * diff;
    }

    return dist;
}

/*
 * Function: farthest_pixel
 * --------------------
 * Finds the index of the pixel which is farthest from the center of the cluster
 * to which it belongs.
 *
 *  double *dists     -- distances of each pixel from the center of the cluster they belong
 *  int    n_pixels   -- number of pixels of the image
 *
 *  return int        -- index of the pixel which is farthest from its cluster center
 */
int farthest_pixel(double *dists, int n_pixels)
{
    int px;
    int far_px;
    double max_dist;

    max_dist = 0;
    for (px = 0; px < n_pixels; px++) {
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
 *  double *dists     -- array of the distances of each pixel from its closest cluster center
 *  int    n_pixels   -- number of pixels of the image
 *
 *  return double     -- sum of squared rrrors
 */
double sum_sqr_errors(double *dists, int n_pixels)
{
    int px;
    double sse = 0;

    for (px = 0; px < n_pixels; px++) {
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
 *  byte_t *data       -- matrix of the pixels of the image
 *  double *centers    -- matrix of the clusters centers
 *  int    *labels     -- clusters to which each pixel belongs
 *  int    n_pixels    -- number of pixels of the image
 *  int    n_channels  -- number of color components of the image
 *  int    k_size      -- size of the clusters centers "array"
 */
void update_data(byte_t *data, double *centers, int *labels, int n_pixels, int n_channels, int k_size)
{
    int px, ch, j;
    int px_off, k_off;
    int min_k;

    for (j = 0; j < k_size; j++) {
        centers[j] = round(centers[j]);
    }

    for (px = 0; px < n_pixels; px++) {
        min_k = labels[px];

        px_off = px * n_channels;
        k_off = min_k * n_channels;
        for (ch = 0; ch < n_channels; ch++) {
            data[px_off + ch] = (byte_t)centers[k_off + ch];
        }
    }
}
