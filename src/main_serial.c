/*
 * File: main_serial.c
 * --------------------
 * Contains the main function for the serial implementation of the
 * k-means color-based segmentation. It defines the flow of the program. It
 * handles arguments and optional parameters as well as the printing of results
 * and usage message.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

#include "image_io.h"
#include "segmentation.h"

#define DEFAULT_N_CLUSTS 4
#define DEFAULT_MAX_ITERS 150
#define DEFAULT_OUT_PATH "result.jpg"

void print_usage(char *pgr_name);
void print_details(int n_pixels, int n_channels, int n_clusts, int n_iters, double sse, double exec_time);

int main(int argc, char **argv)
{
    char *in_path = NULL;
    char *out_path = DEFAULT_OUT_PATH;
    byte_t *data;
    int width, height;
    int n_pixels, n_channels;
    int n_clusts = DEFAULT_N_CLUSTS;
    int n_iters = DEFAULT_MAX_ITERS;
    int seed = time(NULL);
    double sse, start_time, exec_time;

    // PARSING ARGUMENTS AND OPTIONAL PARAMETERS

    char optchar;
    while ((optchar = getopt(argc, argv, "k:m:o:s:h")) != -1) {
        switch (optchar) {
            case 'k':
                n_clusts = strtol(optarg, NULL, 10);
                break;
            case 'm':
                n_iters = strtol(optarg, NULL, 10);
                break;
            case 'o':
                out_path = optarg;
                break;
            case 's':
                seed = strtol(optarg, NULL, 10);
                break;
            case 'h':
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }

    in_path = argv[optind];

    // VALIDATING INPUT PARAMETERS

    if (in_path == NULL) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (n_clusts < 2) {
        fprintf(stderr, "INPUT ERROR: << Invalid number of clusters >> \n");
        exit(EXIT_FAILURE);
    }

    if (n_iters < 1) {
        fprintf(stderr, "INPUT ERROR: << Invalid maximum number of iterations >> \n");
        exit(EXIT_FAILURE);
    }

    srand(seed);

    // SCANNING INPUT IMAGE

    data = img_load(in_path, &width, &height, &n_channels);
    n_pixels = width * height;

    // EXECUTING KMEANS SEGMENTATION

    start_time = omp_get_wtime();
    kmeans_segm(data, n_pixels, n_channels, n_clusts, &n_iters, &sse);
    exec_time = omp_get_wtime() - start_time;

    // SAVING AND PRINTING RESULTS

    img_save(out_path, data, width, height, n_channels);
    print_details(n_pixels, n_channels, n_clusts, n_iters, sse, exec_time);

    free(data);

    return EXIT_SUCCESS;
}

void print_usage(char *pgr_name)
{
    char *usage = "PROGRAM USAGE \n\n"
        "   %s [-h] [-k num_clusters] [-m max_iters] [-o output_img] \n"
        "                [-o output_img] [-s seed] input_image \n\n"
        "   The input image filepath is the only mandatory argument and \n"
        "   must be specified last, after all the optional parameters. \n"
        "   Valid input image formats are JPEG, PNG, BMP, GIF, TGA, PSD, \n"
        "   PIC, HDR and PNM. The program performs a color-based segmentation\n"
        "   of the input image using a the k-means clustering algorithm. \n\n"
        "OPTIONAL PARAMETERS \n\n"
        "   -k num_clusters : number of clusters to use for the segmentation of \n"
        "                     the image. Must be bigger than 1. Default is %d. \n"
        "   -m max_iters    : maximum number of iterations that the clustering \n"
        "                     algorithm can perform before being forced to stop. \n"
        "                     Must be bigger that 0. Default is %d. \n"
        "   -o output_image : filepath of the output image. Valid output image \n"
        "                     formats are JPEG, PNG, BMP and TGA. If not specified, \n"
        "                     the resulting image will be saved in the current \n"
        "                     directory using JPEG format. \n"
        "   -s seed         : seed to use for the random selection of the initial \n"
        "                     centers. The clustering algorithm will always use  \n"
        "                     the same set of initial centers when a certain \n"
        "                     seed is specified. \n"
        "   -h              : print usage information. \n";

    fprintf(stderr, usage, pgr_name, DEFAULT_N_CLUSTS, DEFAULT_MAX_ITERS);
}

void print_details(int n_pixels, int n_channels, int n_clusts, int n_iters, double sse, double exec_time)
{
    char *details = "EXECUTION DETAILS\n"
        "-------------------------------------------------------\n"
        "  Number of pixels      : %d\n"
        "  Number of channels    : %d\n"
        "  Number of clusters    : %d\n"
        "  Number of iterations  : %d\n"
        "  Sum of Squared Errors : %f\n"
        "  Execution time        : %f\n";

    fprintf(stdout, details, n_pixels, n_channels, n_clusts, n_iters, sse, exec_time);
}
