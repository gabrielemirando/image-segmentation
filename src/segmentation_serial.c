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

#include "image_io.h"
#include "segmentation.h"

void init_centers(byte_t *data, double *centers, int n_px, int n_ch, int n_clus);
int assign_pixels(byte_t *data, double *centers, int *labels, double *dists, int n_px, int n_ch, int n_clus);
void update_centers(byte_t *data, double *centers, int *labels, double *dists, int n_px, int n_ch, int n_clus);
double compute_sse(double *dists, int n_px);
void update_data(byte_t *data, double *centers, int *labels, int n_px, int n_ch);

/*
 * Function:  kmeans_segm
 * --------------------
 * Performs color-based segmentation on the image data using the k-means
 * clustering algorithm.
 *
 *  byte_t *data    --  matrix containing the color values of the pixels of the image,
 *                      outputs the color values of the pixel after the segmentation
 *  int    n_px     --  number of pixels of the image
 *  int    n_ch     --  number of color components of the image
 *  int    n_clus   --  number of clusters to use for the segmentation
 *  int    *n_iters --  number of maximum iterations for k-means algorithm,
 *                      outputs the numbers of iterations that took to complete
 *  double *sse     --  outputs the of sum of squared errors of k-means algorithm
 */
void kmeans_segm(byte_t *data, int n_px, int n_ch, int n_clus, int *n_iters, double *sse)
{
    int iter, max_iters;
    int changes;
    int *labels;
    double *centers;
    double *dists;

    max_iters = *n_iters;

    labels = malloc(n_px * sizeof(int));
    centers = malloc(n_clus * n_ch * sizeof(double));
    dists = malloc(n_px * sizeof(double));

    init_centers(data, centers, n_px, n_ch, n_clus);

    for (iter = 0; iter < max_iters; iter++) {
        changes = assign_pixels(data, centers, labels, dists, n_px, n_ch, n_clus);

        if (!changes) {
            break;
        }

        update_centers(data, centers, labels, dists, n_px, n_ch, n_clus);
    }

    *n_iters = iter;

    *sse = compute_sse(dists, n_px);

    update_data(data, centers, labels, n_px, n_ch);

    free(centers);
    free(labels);
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
void init_centers(byte_t *data, double *centers, int n_px, int n_ch, int n_clus)
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
 * Function:  assign_pixels
 * --------------------
 * Assign each pixel to the closest cluster.
 *
 *  byte_t *data    --  matrix containing the color values of the pixels of the image
 *  double *centers --  matrix of the clusters centers
 *  int    *labels  --  indexes of the clusters to which each pixel belongs
 *  double *dists   --  distances of each pixel from their cluster center
 *  int    n_px     --  number of pixels of the image
 *  int    n_ch     --  number of color components of the image
 *  int    n_clus   --  number of clusters to use for the segmentation
 *
 *  return int      --  a value different from 0 if there were changes in the
 *                      cluster assignments of the pixels
 */
int assign_pixels(byte_t *data, double *centers, int *labels, double *dists, int n_px, int n_ch, int n_clus)
{
    int px, ch, k;
    int min_k, changes = 0;
    double dist, min_dist;

    for (px = 0; px < n_px; px++) {
        min_dist = DBL_MAX;

        for (k = 0; k < n_clus; k++) {
            dist = 0;

            for (ch = 0; ch < n_ch; ch++) {
                dist += pow((double)(data[px * n_ch + ch] - centers[k * n_ch + ch]), 2);
            }

            if (dist < min_dist) {
                min_dist = dist;
                min_k = k;
            }
        }

        dists[px] = min_dist;

        if (labels[px] != min_k) {
            labels[px] = min_k;
            changes = 1;
        }
    }

    return changes;
}

/*
 * Function:  update_centers
 * --------------------
 * Update the the values of the clusters centers, by computing the mean of the
 * pixel objects belonging to each cluster.
 *
 *  byte_t *data    --  matrix containing the color values of the pixels of the image
 *  double *centers --  matrix of the clusters centers
 *  int    *labels  --  indexes of the clusters to which each pixel belongs
 *  double *dists   --  distances of each pixel from their cluster center
 *  int    n_px     --  number of pixels of the image
 *  int    n_ch     --  number of color components of the image
 *  int    n_clus   --  number of clusters to use for the segmentation
 */
void update_centers(byte_t *data, double *centers, int *labels, double *dists, int n_px, int n_ch, int n_clus)
{
    int px, ch, k;
    int *counts;
    int min_k, far_px;
    double max_dist;

    counts = malloc(n_clus * sizeof(int));

    // Resetting centers and initializing clusters counters
    for (k = 0; k < n_clus; k++) {
        for (ch = 0; ch < n_ch; ch++) {
            centers[k * n_ch + ch] = 0;
        }
        counts[k] = 0;
    }

    // Computing partial sums of the centers and updating clusters counters
    for (px = 0; px < n_px; px++) {
        min_k = labels[px];
        for (ch = 0; ch < n_ch; ch++) {
            centers[min_k * n_ch + ch] += data[px * n_ch + ch];
        }
        counts[min_k]++;
    }

    // Dividing to obtain the centers mean
    for (k = 0; k < n_clus; k++) {
        if (counts[k]) {
            for (ch = 0; ch < n_ch; ch++) {
                centers[k * n_ch + ch] /= counts[k];
            }
        } else {
            max_dist = 0;
            for (px = 0; px < n_px; px++) {
                if (dists[px] > max_dist) {
                    max_dist = dists[px];
                    far_px = px;
                }
            }

            for (ch = 0; ch < n_ch; ch++) {
                centers[k * n_ch + ch] = data[far_px * n_ch + ch];
            }

            dists[far_px] = 0;
        }
    }

    free(counts);
}

/*
 * Function: compute_sse
 * --------------------
 * Computes the Sum of Squared Errors of the final cluster configuration.
 *
 *  double *dists  -- array of the distances of each pixel from its cluster center
 *  int    n_px    -- number of pixels of the image
 *
 *  return double  -- sum of squared rrrors
 */
double compute_sse(double *dists, int n_px)
{
    int px;
    double sse = 0;

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
 */
void update_data(byte_t *data, double *centers, int *labels, int n_px, int n_ch)
{
    int px, ch, min_k;

    for (px = 0; px < n_px; px++) {
        min_k = labels[px];

        for (ch = 0; ch < n_ch; ch++) {
            data[px * n_ch + ch] = (byte_t)round(centers[min_k * n_ch + ch]);
        }
    }
}
