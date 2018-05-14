#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <omp.h>

#include "image_io.h"
#include "segmentation.h"

void init_centers(byte_t *data, double *centers, int n_pixels, int n_channels,
                  int n_clusts);

int closest_clust(byte_t *pixel, double *centers, int n_channels, int n_clusts,
                  double *clust_dist);

double sqr_dist(byte_t *pixel, double *center, int n_channels);

int farthest_pixel(double *dists, int n_pixels);

double sum_sqr_error(double *dists, int n_pixels);

void update_data(byte_t *data, double *centers, int *labels, int n_pixels,
                 int n_channels, int k_size);

void kmeans_segm_omp(byte_t *data, int n_pixels, int n_channels, int n_clusts,
                int *n_iters, double *sse, int n_threads)
{
    int px, ch, k, j, th;
    int px_off, k_off, k_size;
    int iter, max_iters, changes;
    int min_k, far_px;
    int *counts, *labels;
    int thread_num, thcen_off, thcount_off;
    double *centers, *tmp_centers, *dists;

    k_size = n_clusts * n_channels;
    max_iters = *n_iters;

    centers = malloc(k_size * sizeof(double));
    tmp_centers = malloc(n_threads * k_size * sizeof(double));
    counts = malloc(n_threads * n_clusts * sizeof(int));
    labels = malloc(n_pixels * sizeof(int));
    dists = malloc(n_pixels * sizeof(double));

    omp_set_dynamic(0);
    omp_set_num_threads(n_threads);

    init_centers(data, centers, n_pixels, n_channels, n_clusts);

    for (iter = 0; iter < max_iters; iter++) {
        changes = 0;

        #pragma omp parallel private(thread_num, thcen_off, thcount_off, j, k)
        {
            thread_num = omp_get_thread_num();
            thcen_off = thread_num * k_size;
            thcount_off = thread_num * n_clusts;

            for (j = 0; j < k_size; j++) {
                tmp_centers[thcen_off + j] = 0;
            }

            for (k = 0; k < n_clusts; k++) {
                counts[thcount_off + k] = 0;
            }

            #pragma omp for schedule(static) private(px, px_off, min_k, k_off, ch)
            for (px = 0; px < n_pixels; px++) {
                px_off = px * n_channels;

                min_k = closest_clust(&data[px_off], centers, n_channels, n_clusts,
                                      &dists[px]);

                if (labels[px] != min_k) {
                    labels[px] = min_k;
                    changes = 1;
                }

                k_off = thcen_off + min_k * n_channels;
                for (ch = 0; ch < n_channels; ch++) {
                    tmp_centers[k_off + ch] += data[px_off + ch];
                }

                counts[thcount_off + min_k]++;
            }
        }

        if (!changes) {
            break;
        }

        for (th = 1; th < n_threads; th++) {
            thcen_off = th * k_size;
            thcount_off = th * n_clusts;

            for (j = 0; j < k_size; j++) {
                tmp_centers[j] += tmp_centers[thcen_off + j];
            }

            for (k = 0; k < n_clusts; k++) {
                counts[k] += counts[thcount_off + k];
            }
        }

        for (k = 0; k < n_clusts; k++) {
            k_off = k * n_channels;

            if (counts[k]) {
                for (ch = 0; ch < n_channels; ch++) {
                    centers[k_off + ch] = tmp_centers[k_off + ch] / counts[k];
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

    *sse = sum_sqr_error(dists, n_pixels);

    update_data(data, centers, labels, n_pixels, n_channels, k_size);

    free(centers);
    free(tmp_centers);
    free(labels);
    free(counts);
    free(dists);
}

void init_centers(byte_t *data, double *centers, int n_pixels, int n_channels,
                  int n_clusts)
{
    int k, ch, rnd;
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

int closest_clust(byte_t *pixel, double *centers, int n_channels, int n_clusts,
                  double *clust_dist)
{
    int k, min_k;
    double dist, min_dist;

    min_dist = DBL_MAX;
    for (k = 0; k < n_clusts; k++) {
        dist = sqr_dist(pixel, &centers[k * n_channels], n_channels);
        if (dist < min_dist) {
            min_dist = dist;
            min_k = k;
        }
    }

    *clust_dist = min_dist;

    return min_k;
}

double sqr_dist(byte_t *pixel, double *center, int n_channels)
{
    int ch;
    double diff, dist = 0;

    for (ch = 0; ch < n_channels; ch++) {
        diff = (double)(pixel[ch] - center[ch]);
        dist += diff * diff;
    }

    return dist;
}

int farthest_pixel(double *dists, int n_pixels)
{
    int px, far_px;
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

double sum_sqr_error(double *dists, int n_pixels)
{
    int px;
    double sse = 0;

    #pragma omp parallel for private(px) reduction(+:sse)
    for (px = 0; px < n_pixels; px++) {
        sse += dists[px];
    }

    return sse;
}

void update_data(byte_t *data, double *centers, int *labels, int n_pixels,
                 int n_channels, int k_size)
{
    int px, ch, j;
    int px_off, k_off;
    int min_k;

    for (j = 0; j < k_size; j++) {
        centers[j] = round(centers[j]);
    }

    #pragma omp parallel for schedule(static) private(min_k, px, ch, px_off, k_off)
    for (px = 0; px < n_pixels; px++) {
        min_k = labels[px];

        px_off = px * n_channels;
        k_off = min_k * n_channels;
        for (ch = 0; ch < n_channels; ch++) {
            data[px_off + ch] = (byte_t)centers[k_off + ch];
        }
    }
}
