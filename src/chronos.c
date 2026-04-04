/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "chronos.h"

#include "magick/colorspace.h"
#include "magick/common.h"
#include "magick/constitute.h"
#include "magick/error.h"
#include "magick/image.h"
#include "magick/magick.h"
#include "magick/pixel_cache.h"
#include "magick/resize.h"
#include "wand/magick_wand.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Clamp GraphicsMagick's Quantum value from signed to unisgned limit
 * @param v -> the double value
 */
static Quantum
chronos_clamp_quantum(double v)
{
    if (v < 0.0) return 0;
    if (v > 65535.0) return 65535;
    return (Quantum)v;
}

/**
 * @brief Process the Image in GraphicsMagick context
 * @param *in_filename -> The input's filename
 * @param *out_filename -> The output's filename
 */
static void
chronos_process_image(const char *in_filename, const char *out_filename,
    Chronos_Process_Function fn)
{
    Image *image = NULL;
    Image *result = NULL;
    ImageInfo *image_info = NULL;
    ExceptionInfo exception;

    InitializeMagick(NULL);
    GetExceptionInfo(&exception);

    image_info = CloneImageInfo(image_info);
    strcpy(image_info->filename, in_filename);

    image = ReadImage(image_info, &exception);
    if (exception.severity != UndefinedException) CatchException(&exception);

    if (!image) exit(1);

    result = fn(image, &exception);

    strcpy(result->filename, out_filename);
    WriteImage(image_info, result);

    DestroyImage(image);
    if (result != image) DestroyImage(result);
    DestroyImageInfo(image_info);
    DestroyExceptionInfo(&exception);
    DestroyMagick();
    return;
}

CHRONOS_DEF void
chronos_rgb_to_grayscale(const char *in_filename, const char *out_filename)
{
    MagickWand *wand;
    MagickBool status;

    InitializeMagick(NULL);

    wand = NewMagickWand();

    status = MagickReadImage(wand, in_filename);
    if (status == MagickFalse) {
        printf("[CHRONOS-ERROR] Unable to read image\n");
        exit(1);
    }

    status = MagickSetImageColorspace(wand, GRAYColorspace);
    if (status == MagickFalse) {
        printf("[CHRONOS-ERROR] Unable to convert to Grayscale\n");
        exit(1);
    }

    status = MagickWriteImage(wand, out_filename);
    if (status == MagickFalse) {
        printf("[CHRONOS-ERROR] Unable to write image\n");
        exit(1);
    }

    printf("[CHRONOS-INFO] Successfully write %s\n", out_filename);
    DestroyMagickWand(wand);

    return;
}

CHRONOS_DEF void
chronos_rgb_to_ycocg(char **argv)
{
    Image *image;
    ImageInfo *image_info;
    ExceptionInfo exception;
    PixelPacket *pixels;

    long y;
    long x;
    long columns;
    long rows;

    InitializeMagick(*argv);
    image_info = CloneImageInfo((ImageInfo *)NULL);
    strcpy(image_info->filename, "um80.jpg");
    GetExceptionInfo(&exception);

    image = ReadImage(image_info, &exception);
    if (image == (Image *)NULL) {
        printf("[CHRONOS-ERROR] Unable to read um80.jpg\n");
        exit(1);
    }

    rows = image->rows;
    columns = image->columns;

    for (y = 0; y < rows; y++) {
        pixels = GetImagePixels(image, 0, y, columns, 1);
        if (pixels == (PixelPacket *)NULL) break;

        for (x = 0; x < columns; x++) {
            double r = pixels[x].red;
            double g = pixels[x].green;
            double b = pixels[x].blue;

            double co = r - b;
            double tmp = b + (co / 2.0);
            double cg = g - tmp;
            double Y = tmp + (cg / 2.0);

            double bias = 65535.0 / 2.0;

            pixels[x].red = chronos_clamp_quantum(Y);
            pixels[x].green = chronos_clamp_quantum(
                co + bias); // Offset for signed
            pixels[x].blue = chronos_clamp_quantum(
                cg + bias); // Offset for signed
        }
        SyncImagePixels(image);
    }

    strcpy(image->filename, "output_ycocg.jpg");
    WriteImage(image_info, image);
    printf("[CHRONOS-INFO] Successfully write output_ycocg.jpg\n");

    DestroyImage(image);
    DestroyImageInfo(image_info);
    DestroyExceptionInfo(&exception);
    DestroyMagick();

    return;
}

/**
 * @brief The operation to convert to PPM
 * @param *image -> The image object
 * @param *exception_info -> The exception's information
 */
static Image *
chronos_convert_to_ppm_op(Image *image, ExceptionInfo *exception_info)
{
    (void)exception_info;
    strcpy(image->magick, "PPM");
    return image;
}

CHRONOS_DEF void
chronos_convert_to_ppm(const char *in_filename, const char *out_filename)
{
    chronos_process_image(in_filename, out_filename, chronos_convert_to_ppm_op);
}

CHRONOS_DEF void
chronos_resize_image(const char *in_filename, const char *out_filename)
{
    Image *image;
    Image *resized_image;
    ExceptionInfo exception;

    InitializeMagick(NULL);
    GetExceptionInfo(&exception);

    ImageInfo *image_info = CloneImageInfo((ImageInfo *)NULL);
    strcpy(image_info->filename, in_filename);
    image = ReadImage(image_info, &exception);

    resized_image = ResizeImage(image, 600, 400, LanczosFilter, 1.0,
        &exception);

    strcpy(resized_image->filename, out_filename);
    WriteImage(image_info, resized_image);
    DestroyImage(image);
    DestroyImage(resized_image);
    DestroyImageInfo(image_info);
    DestroyExceptionInfo(&exception);
    DestroyMagick();
}

CHRONOS_DEF void chronos_scale_image(const char *in_filename,
    const char *out_filename);
