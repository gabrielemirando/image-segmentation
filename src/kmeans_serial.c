#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <omp.h>

#include "img_io.h"
#include "kmeans.h"

double *rand_centers(byte_t *data, int n_pixels, int n_channels, int n_clusts)
{
    double *centers;
    int rnd, *old_rnds;
    int k, sub, ch;
    int rnd_buf, k_buf;
    int i, duplicate;

    centers = malloc(n_clusts * n_channels * sizeof(double));
    old_rnds = malloc(n_clusts * sizeof(int));

    sub = n_pixels - n_clusts;

    for (k = 0; k < n_clusts; k++) {
        rnd = rand() % (sub + 1);

        duplicate = 0;
        for (i = 0; i < k; i++) {
            if (rnd == old_rnds[i]) {
                duplicate = 1;
                break;
            }
        }

        if (duplicate) {
            rnd = sub;
        }

        k_buf = k * n_channels;
        rnd_buf = rnd * n_channels;
        for (ch = 0; ch < n_channels; ch++) {
            centers[k_buf + ch] = data[rnd_buf + ch];
        }

        old_rnds[k] = rnd;

        sub++;
    }

    free(old_rnds);

    return centers;
}

void kmeans(byte_t *data, int n_pixels, int n_channels, int n_clusts,
            int max_iters, double *sse, int *iters)
{
    int px, ch, k, i;
    int min_k, far_px, px_buf, k_buf, k_size;
    int iter, changes, *labels, *counts;
    double min_dist, *dists;
    double *centers, tmp;

    centers = rand_centers(data, n_pixels, n_channels, n_clusts);

    dists = malloc(n_pixels * sizeof(double));
    labels = malloc(n_pixels * sizeof(int));
    counts = malloc(n_clusts * sizeof(int));

    k_size = n_clusts * n_channels;

    for (iter = 0; iter < max_iters; iter++) {
        changes = 0;

        for (px = 0; px < n_pixels; px++) {
            px_buf = px * n_channels;

            min_k = closest_clust(&data[px_buf], centers, n_clusts, n_channels, &min_dist);

            if (labels[px] != min_k) {
                labels[px] = min_k;
                changes = 1;
            }

            dists[px] = min_dist;
        }

        if (!changes) {
            break;
        }

        for (i = 0; i < k_size; i++) {
            centers[i] = 0;
        }

        for (k = 0; k < n_clusts; k++) {
            counts[k] = 0;
        }

        for (px = 0; px < n_pixels; px++) {
            min_k = labels[px];

            px_buf = px * n_channels;
            k_buf = min_k * n_channels;
            for (ch = 0; ch < n_channels; ch++) {
                centers[k_buf + ch] += data[px_buf + ch];
            }

            counts[min_k]++;
        }

        for (k = 0; k < n_clusts; k++) {
            k_buf = k * n_channels;

            if (counts[k]) {
                for (ch = 0; ch < n_channels; ch++) {
                    centers[k_buf + ch] /= counts[k];
                }
            } else {
                far_px = farthest_pixel(dists, n_pixels);

                px_buf = far_px * n_channels;
                for (ch = 0; ch < n_channels; ch++) {
                    centers[k_buf + ch] = data[px_buf + ch];
                }

                dists[far_px] = 0;
            }
        }
    }

    *iters = iter;

    tmp = 0;
    for (px = 0; px < n_pixels; px++) {
        tmp += dists[px];
    }
    *sse = tmp;

    for (i = 0; i < k_size; i++) {
        centers[i] = round(centers[i]);
    }

    for (px = 0; px < n_pixels; px++) {
        min_k = labels[px];
        px_buf = px * n_channels;
        k_buf = min_k * n_channels;
        for (ch = 0; ch < n_channels; ch++) {
            data[px_buf + ch] = (byte_t)centers[k_buf + ch];
        }
    }

    free(centers);
    free(labels);
    free(counts);
    free(dists);
}

void kmeans_omp(byte_t *data, int n_pixels, int n_channels, int n_clusts,
                int max_iters, double *sse, int *iters, int *n_threads)
{
    int px, ch, k, i;
    int min_k, far_px, px_buf, k_buf, k_size;
    int iter, changes, *labels, *counts;
    int n_threads_req, n_threads_obt;
    double min_dist, *dists;
    double *centers, tmp;

    n_threads_req = *n_threads;

    omp_set_num_threads(n_threads_req);

    centers = rand_centers(data, n_pixels, n_channels, n_clusts);

    dists = malloc(n_pixels * sizeof(double));
    labels = malloc(n_pixels * sizeof(int));
    counts = malloc(n_clusts * sizeof(int));

    k_size = n_clusts * n_channels;

    for (iter = 0; iter < max_iters; iter++) {
        changes = 0;

        #pragma omp parallel for schedule(static) private(min_k, min_dist, px, px_buf)
        for (px = 0; px < n_pixels; px++) {
            px_buf = px * n_channels;

            min_k = closest_clust(&data[px_buf], centers, n_clusts, n_channels, &min_dist);

            if (labels[px] != min_k) {
                labels[px] = min_k;
                changes = 1;
            }

            dists[px] = min_dist;
        }

        if (!changes) {
            break;
        }

        for (i = 0; i < k_size; i++) {
            centers[i] = 0;
        }

        for (k = 0; k < n_clusts; k++) {
            counts[k] = 0;
        }

        for (px = 0; px < n_pixels; px++) {
            min_k = labels[px];

            px_buf = px * n_channels;
            k_buf = min_k * n_channels;
            for (ch = 0; ch < n_channels; ch++) {
                centers[k_buf + ch] += data[px_buf + ch];
            }

            counts[min_k]++;
        }

        for (k = 0; k < n_clusts; k++) {
            k_buf = k * n_channels;

            if (counts[k]) {
                for (ch = 0; ch < n_channels; ch++) {
                    centers[k_buf + ch] /= counts[k];
                }
            } else {
                far_px = farthest_pixel(dists, n_pixels);

                px_buf = far_px * n_channels;
                for (ch = 0; ch < n_channels; ch++) {
                    centers[k_buf + ch] = data[px_buf + ch];
                }

                dists[far_px] = 0;
            }
        }
    }

    *iters = iter;

    tmp = 0;

    #pragma omp parallel for private(px) reduction(+:tmp)
    for (px = 0; px < n_pixels; px++) {
        tmp += dists[px];
    }

    *sse = tmp;

    for (i = 0; i < k_size; i++) {
        centers[i] = round(centers[i]);
    }

    #pragma omp parallel for schedule(static) private(min_k, px, ch, px_buf, k_buf)
    for (px = 0; px < n_pixels; px++) {
        min_k = labels[px];
        px_buf = px * n_channels;
        k_buf = min_k * n_channels;
        for (ch = 0; ch < n_channels; ch++) {
            data[px_buf + ch] = (byte_t)centers[k_buf + ch];
        }
    }

    free(centers);
    free(labels);
    free(counts);
    free(dists);
}

int closest_clust(byte_t *pixel, double *centers, int n_clusts, int n_channels, double *clust_dist)
{
    int k, min_k;
    double dist, min_dist;

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

double sqr_distance(byte_t *pixel, double *center, int n_channels)
{
    int ch;
    double diff, dist = 0;

    for (ch = 0; ch < n_channels; ch++) {
        diff = (double)(pixel[ch] - center[ch]);
        dist += diff * diff;
    }

    return dist;
}
