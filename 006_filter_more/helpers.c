#include "helpers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Struct for storing RGB values as int
typedef struct
{
    int rgbintBlue;
    int rgbintGreen;
    int rgbintRed;
}
RGBKERNEL;

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE pixel = image[i][j];
            RGBTRIPLE *ppixel = &image[i][j];
            BYTE average_color = round((pixel.rgbtBlue + pixel.rgbtGreen + pixel.rgbtRed) / 3.0);

            // Update rgb values for a pixel with the average of their values
            ppixel->rgbtBlue = average_color;
            ppixel->rgbtGreen = average_color;
            ppixel->rgbtRed = average_color;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    int i, j, k;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width - 1; j++)
        {
            for (k = 0; k < width - j - 1; k++)
            {
                RGBTRIPLE temp_left = image[i][k];
                RGBTRIPLE *ppixel_left = &image[i][k];
                RGBTRIPLE *ppixel_right = &image[i][k + 1];
                *ppixel_left = image[i][k + 1];
                *ppixel_right = temp_left;
            }
        }
    }
    return;
}


void average_colour(int height, int width, int pixel_height, int pixel_width, RGBTRIPLE image[height][width],
                    RGBTRIPLE image_copy[height][width])
{
    int blue_values = 0;
    int green_values = 0;
    int red_values = 0;

    int i, j;
    float pixel_count = 0.0;

    // Loop through pixels within one row and column of a given pixel
    for (i = pixel_height - 1; i < pixel_height + 2; i++)
    {
        // Ignore pixels past the edge of the image
        if (i < 0 || i >= height)
        {
            continue;
        }

        for (j = pixel_width - 1; j < pixel_width + 2; j++)
        {
            // Ignore pixels past the edge of the image
            if (j < 0 || j >= width)
            {
                continue;
            }

            RGBTRIPLE pixel = image[i][j];

            blue_values += pixel.rgbtBlue;
            green_values += pixel.rgbtGreen;
            red_values += pixel.rgbtRed;

            pixel_count++;
        }
    }

    RGBTRIPLE *ppixel = &image_copy[pixel_height][pixel_width];

    // Add RGB values to the pixels of the image copy
    if (blue_values != 0)
    {
        ppixel->rgbtBlue = round(blue_values / pixel_count);
    }

    if (green_values != 0)
    {
        ppixel->rgbtGreen = round(green_values / pixel_count);
    }

    if (red_values != 0)
    {
        ppixel->rgbtRed = round(red_values / pixel_count);
    }

    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{

    // Image copy with same size as the image
    RGBTRIPLE(*image_copy)[width] = malloc(sizeof(RGBTRIPLE[height][width]));
    if (image_copy == NULL)
    {
        printf("Could not create copy of image.\n");
        return;
    }

    // Update average RGB values for the image copy pixels
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            average_colour(height, width, i, j, image, image_copy);
        }
    }

    // Copy pixel RGB values from the image copy to the image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE *ppixel = &image[i][j];
            *ppixel = image_copy[i][j];
        }
    }

    free(image_copy);

    return;
}

// Calculate RGB values of a pixel for the Gx or Gy kernel
RGBKERNEL kernel_calc(int height, int width, int pixel_height, int pixel_width, RGBTRIPLE image[height][width], char axis)
{
    int blue_values = 0;
    int green_values = 0;
    int red_values = 0;

    // Declare if primary and secondary loops use height or width
    int for1 = 0, for2 = 0;
    switch (axis)
    {
        case 'x':
            for1 = pixel_height;
            for2 = pixel_width;
            break;

        case 'y':
            for1 = pixel_width;
            for2 = pixel_height;
            break;
    }

    int value;
    for (int i = for1 - 1; i < for1 + 2; i++)
    {
        // set value depending on looped pixel position
        if (for1 != i)
        {
            value = -2;
        }
        else
        {
            value = -4;
        }

        for (int j = for2 - 1; j < for2 + 2; j++)
        {
            // update value depending on looped pixel position
            if (for1 != i)
            {
                value += 1;
            }
            else
            {
                value += 2;
            }

            RGBTRIPLE pixel;

            // set pixel location based on primary and secondary loop
            if (axis == 'x')
            {
                pixel = image[i][j];
                if (i < 0 || i >= height || j < 0 || j >= width)
                {
                    continue;
                }
            }

            // set pixel location based on primary and secondary loop
            if (axis == 'y')
            {
                pixel = image[j][i];
                if (i < 0 || i >= width || j < 0 || j >= height)
                {
                    continue;
                }
            }

            // Append pixel RGB values using kernel value
            blue_values += pixel.rgbtBlue * value;
            green_values += pixel.rgbtGreen * value;
            red_values += pixel.rgbtRed * value;
        }
    }

    RGBKERNEL kernel;

    // Set and return kernel RGB values
    kernel.rgbintBlue = blue_values;
    kernel.rgbintGreen = green_values;
    kernel.rgbintRed = red_values;

    return kernel;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    char x_axis = 'x';
    char y_axis = 'y';

    // Image copy with same size as the image
    RGBTRIPLE(*image_copy)[width] = malloc(sizeof(RGBTRIPLE[height][width]));
    if (image_copy == NULL)
    {
        printf("Could not create copy of image.\n");
        return;
    }

    // Calculate Gx and Gy kernel values per pixel
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Gx kernel
            RGBKERNEL *gx = malloc(sizeof(RGBKERNEL));
            if (gx == NULL)
            {
                printf("Could not create Gx kernel.\n");
                return;
            }

            // Gy kernel
            RGBKERNEL *gy = malloc(sizeof(RGBKERNEL));
            if (gy == NULL)
            {
                printf("Could not create Gy kernel.\n");
                return;
            }

            // RGB values of pixel from the Gx and Gy kernels
            *gx = kernel_calc(height, width, i, j, image, x_axis);
            *gy = kernel_calc(height, width, i, j, image, y_axis);

            // Combine Gx and Gy kernels
            int combined_blue = round(sqrt(pow(gx->rgbintBlue, 2) + pow(gy->rgbintBlue, 2)));
            int combined_green = round(sqrt(pow(gx->rgbintGreen, 2) + pow(gy->rgbintGreen, 2)));
            int combined_red = round(sqrt(pow(gx->rgbintRed, 2) + pow(gy->rgbintRed, 2)));

            RGBTRIPLE *ppixel_copy = &image_copy[i][j];

            if (combined_blue > 255)
            {
                combined_blue = 255;
            }

            if (combined_green > 255)
            {
                combined_green = 255;
            }

            if (combined_red > 255)
            {
                combined_red = 255;
            }

            // Add values to pixel the image copy
            ppixel_copy->rgbtBlue = combined_blue;
            ppixel_copy->rgbtGreen = combined_green;
            ppixel_copy->rgbtRed = combined_red;

            // Free kernels
            free(gx);
            free(gy);
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Copy pixel RGB values from the image copy to the image
            RGBTRIPLE *ppixel = &image[i][j];
            *ppixel = image_copy[i][j];
        }
    }

    // Free image copy
    free(image_copy);

    return;
}
