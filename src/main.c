/*
 * Image segmentation using k-means algorithm
 * --------------------------------------------------
 * Author: Gabriele Mirando
 *
 * About:  The goal of the project is to achieve image segmentation using the
 *         standard kmeans clustering algorithm and to speed up the execution
 *         exploiting different multithreading programming paradigms.
 *
 * Help:   External libraries have been used for handling the i/o of images.
 *          - stb_image (v2.19): to load the values of each pixel of the images.
 *          - stb_image_write (v1.09): to create images from pixel values.
 *         These libraries are available at https://github.com/nothings/stb.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

#define OMP "omp"
#define CUDA "cuda"
#define SERIAL "serial"

#define DEFAULT_MAX_ITERS 150
#define DEFAULT_THREADS 1

#define DEBUG_SEED 2

#include "img_io.h"
#include "kmeans.h"
#include "utils.h"

void print_help(char *pgr_name);

void print_details(int n_pixels, int n_channels, int n_clusts, char *pgr_paradigm,
                   int n_threads, double exec_time, double sse, int n_iters);

int main(int argc, char **argv)
{
    char *in_path = NULL;
    char *out_path = NULL;
    char *pgr_paradigm = SERIAL;
    int width, height, n_pixels;
    int n_channels, n_clusts = -1;
    int n_threads = DEFAULT_THREADS;
    int max_iters = DEFAULT_MAX_ITERS;
    double start_time, exec_time, sse;
    int n_iters, debug_mode;

    byte_t *data;

    char optchar;
    while ((optchar = getopt(argc, argv, "i:o:k:m:p:t:d")) != -1) {
        switch (optchar) {
            case 'i':
                in_path = optarg;
                break;
            case 'o':
                out_path = optarg;
                break;
            case 'k':
                n_clusts = strtol(optarg, NULL, 10);
                break;
            case 'm':
                max_iters = strtol(optarg, NULL, 10);
                break;
            case 'p':
                pgr_paradigm = optarg;
                break;
            case 't':
                n_threads = strtol(optarg, NULL, 10);
                break;
            case 'd':
                debug_mode = 1;
                break;
            default:
                print_help(argv[0]);
                break;
        }
    }

    if (in_path == NULL || out_path == NULL || n_clusts < 2 || n_threads < 1 || max_iters < 1) {
        print_help(argv[0]);
    }

    if (debug_mode) {
        srand(DEBUG_SEED);
    } else {
        srand(time(NULL));
    }

    data = img_load(in_path, &width, &height, &n_channels);

    n_pixels = width * height;

    start_time = omp_get_wtime();

    if (strings_equal(pgr_paradigm, SERIAL)) {
        kmeans(data, n_pixels, n_channels, n_clusts, max_iters, &sse, &n_iters);
    } else if (strings_equal(pgr_paradigm, OMP)) {
        kmeans_omp(data, n_pixels, n_channels, n_clusts, max_iters, &sse, &n_iters, &n_threads);
    } else if (strings_equal(pgr_paradigm, CUDA)) {

    }

    exec_time = omp_get_wtime() - start_time;

    img_save(out_path, data, width, height, n_channels);

    print_details(n_pixels, n_channels, n_clusts, pgr_paradigm,
                  n_threads, exec_time, sse, n_iters);

    free(data);

    return EXIT_SUCCESS;
}

void print_details(int n_pixels, int n_channels, int n_clusts, char *pgr_paradigm,
                   int n_threads, double exec_time, double sse, int n_iters)
{
    char *details = "EXECUTION DETAILS\n"
        "-------------------------------------------------------\n"
        "  Number of pixels        : %d\n"
        "  Number of channels      : %d\n"
        "  Number of clusters      : %d\n"
        "  Programming paradigm    : %s\n"
        "  Number of threads (omp) : %d\n"
        "  Execution time          : %f\n"
        "  Sum of Squared Errors   : %f\n"
        "  Number of iterations    : %d\n";

    fprintf(stdout, details,
            n_pixels, n_channels, n_clusts,
            pgr_paradigm, n_threads,
            exec_time, sse, n_iters);
}

void print_help(char *pgr_name)
{
    char *help = "MAIN USAGE: %s [-i input_img] [-o output_img] [-k num_clusters] \n"
        "-------------------------------------------------------\n"
        "     -i input_img    : [MANDATORY] input image file. Valid input image formats \n"
        "                       are JPEG, PNG, BMP, GIF, TGA, PSD, PIC, HDR, PNM.  \n"
        "     -o output_img   : [MANDATORY] output image file. Valid output image \n"
        "                       formats are JPEG, PNG, BMP, TGA.  \n"
        "     -k num_clusters : [MANDATORY] number of clusters. Must be bigger than 1. \n"
        "     -m max_iters    : maximum number of iterations that the clustering \n"
        "                       algorithm can perform before being forced to stop. \n"
        "                       Default is %d. Must be bigger that 0.\n"
        "     -p pgr_paradigm : programming paradigm. Default is '%s', but '%s' and \n"
        "                       '%s' are also available. \n"
        "     -t num_threads  : number of threads. This argument will be taken in \n"
        "                       consideration only if OMP was specified as programming \n"
        "                       paradigm. Default is %d. Must be bigger than 0. \n"
        "     -d              : debug flag. If present the kmeans algorithm \n"
        "                       will always use the same set of initial centers. \n"
        "     -h              : print this help information. \n";

    fprintf(stderr, help,
            pgr_name,
            DEFAULT_MAX_ITERS,
            SERIAL, OMP, CUDA,
            DEFAULT_THREADS);

    exit(EXIT_FAILURE);
}
