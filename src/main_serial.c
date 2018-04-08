#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

#include "image_io.h"
#include "kmeans.h"

#define DEFAULT_N_CLUSTS 4
#define DEFAULT_MAX_ITERS 500
#define DEFAULT_OUT_PATH "result.jpg"

void print_usage(char *pgr_name);

void print_details(int n_pixels, int n_channels, int n_clusts,
                   double exec_time, double sse, int n_iters);

int main(int argc, char **argv)
{
    char *in_path = NULL;
    char *out_path = DEFAULT_OUT_PATH;
    int width, height, n_pixels, n_channels;
    int n_clusts = DEFAULT_N_CLUSTS;
    int max_iters = DEFAULT_MAX_ITERS;
    int seed = time(NULL), n_iters;
    double start_time, exec_time, sse;
    byte_t *data;

    char optchar;
    while ((optchar = getopt(argc, argv, "k:m:o:s:h")) != -1) {
        switch (optchar) {
            case 'k':
                n_clusts = strtol(optarg, NULL, 10);
                break;
            case 'm':
                max_iters = strtol(optarg, NULL, 10);
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

    if (in_path == NULL) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (n_clusts < 2) {
        fprintf(stderr, "INPUT ERROR: << Invalid number of clusters >> \n");
        exit(EXIT_FAILURE);
    }

    if (max_iters < 1) {
        fprintf(stderr, "INPUT ERROR: << Invalid maximum number of iterations >> \n");
        exit(EXIT_FAILURE);
    }

    srand(seed);

    data = img_load(in_path, &width, &height, &n_channels);
    n_pixels = width * height;

    start_time = omp_get_wtime();
    kmeans(data, n_pixels, n_channels, n_clusts, max_iters, &sse, &n_iters);
    exec_time = omp_get_wtime() - start_time;

    img_save(out_path, data, width, height, n_channels);

    print_details(n_pixels, n_channels, n_clusts, exec_time, sse, n_iters);

    free(data);

    return EXIT_SUCCESS;
}

void print_details(int n_pixels, int n_channels, int n_clusts,
                   double exec_time, double sse, int n_iters)
{
    char *details = "EXECUTION DETAILS\n"
        "-------------------------------------------------------\n"
        "  Number of pixels        : %d\n"
        "  Number of channels      : %d\n"
        "  Number of clusters      : %d\n"
        "  Execution time          : %f\n"
        "  Sum of Squared Errors   : %f\n"
        "  Number of iterations    : %d\n";

    fprintf(stdout, details, n_pixels, n_channels, n_clusts, exec_time, sse, n_iters);
}

void print_usage(char *pgr_name)
{
    char *usage = "USAGE \n\n"
        "   %s [-h] [-k num_clusters] [-m max_iters] [-o output_img] \n"
        "            [-o output_img] [-s seed] input_image \n\n"
        "   The input image filepath is the only mandatory argument and \n"
        "   must be specified last, after all the optional parameters. \n"
        "   Valid input image formats are JPEG, PNG, BMP, GIF, TGA, PSD, \n"
        "   PIC, HDR and PNM. \n\n"
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
