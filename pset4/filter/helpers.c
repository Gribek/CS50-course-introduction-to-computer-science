#include "helpers.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct
{
    int x;
    int y;
}
box_pixel;

// Function prototypes
void average_rgb(RGBTRIPLE *pixel);
void swap_pixels(RGBTRIPLE *px, RGBTRIPLE *py);
void array_copy(int x, int y, RGBTRIPLE array[x][y], RGBTRIPLE array_copy[x][y]);
void select_box_pixels(int i, int j, box_pixel array[]);
int calc_final_rgb(int gx, int gy);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            average_rgb(&image[i][j]);
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0, k = width - 1; j < k; j++, k--)
        {
            swap_pixels(&image[i][j], &image[i][k]);
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    int red, green, blue, x, y;

    // allocate memory for image_copy
    RGBTRIPLE(*image_copy)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    // make a copy of image
    array_copy(height, width, image, image_copy);

    // array of box pixels
    box_pixel box_pixels[9];

    // current box_pixel
    box_pixel pixel;
    RGBTRIPLE pixel_rgb;

    // loop through all pixels in image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            //BOX BLUR
            // number of pixels counted
            int pixel_count = 0;

            // set amount of red, green and blue to zero
            red = 0, green = 0, blue = 0;

            // prepare array of box pixels
            select_box_pixels(i, j, box_pixels);

            // loop through all pixels in box
            for (int k = 0; k < 9; k++)
            {
                pixel = box_pixels[k];

                // make sure that pixel is not outside of the image
                if (!(pixel.x < 0 || pixel.y < 0 || pixel.x >= height || pixel.y >= width))
                {
                    // current pixel rgb values
                    pixel_rgb = image_copy[pixel.x][pixel.y];

                    // calculate total amount of red, green and blue
                    red += pixel_rgb.rgbtRed;
                    green += pixel_rgb.rgbtGreen;
                    blue += pixel_rgb.rgbtBlue;
                    pixel_count++;
                }
            }

            // replace rgb values for current pixel with average
            image[i][j].rgbtRed = round((float) red / pixel_count);
            image[i][j].rgbtGreen = round((float) green / pixel_count);
            image[i][j].rgbtBlue = round((float) blue / pixel_count);
        }
    }

    // free memory for image_copy
    free(image_copy);

    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // multipliers for nine pixels in the box
    int Gx_multipliers[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int Gy_multipliers[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

    // array of box pixels
    box_pixel box_pixels[9];

    // allocate memory for image copy
    RGBTRIPLE(*initial_image)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    // copy image
    array_copy(height, width, image, initial_image);

    // current box_pixel
    box_pixel pixel;
    RGBTRIPLE pixel_rgb;

    // final rgb values for pixel
    int red, green, blue;

    // loop through all pixels in image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Sobel operator
            // set Gx and Gy values to zero
            int gx_red = 0, gx_green = 0, gx_blue = 0, gy_red = 0, gy_green = 0, gy_blue = 0;

            // prepare array of box pixels
            select_box_pixels(i, j, box_pixels);

            // loop through all pixels in box
            for (int k = 0; k < 9; k++)
            {
                pixel = box_pixels[k];

                // make sure that pixel is not outside of the image
                if (!(pixel.x < 0 || pixel.y < 0 || pixel.x >= height || pixel.y >= width))
                {
                    // current pixel rgb values
                    pixel_rgb = initial_image[pixel.x][pixel.y];

                    // calculate gx rgb values
                    gx_red += pixel_rgb.rgbtRed * Gx_multipliers[k];
                    gx_green += pixel_rgb.rgbtGreen * Gx_multipliers[k];
                    gx_blue += pixel_rgb.rgbtBlue * Gx_multipliers[k];

                    // calculate gy rgb values
                    gy_red += pixel_rgb.rgbtRed * Gy_multipliers[k];
                    gy_green += pixel_rgb.rgbtGreen * Gy_multipliers[k];
                    gy_blue += pixel_rgb.rgbtBlue * Gy_multipliers[k];
                }
            }

            // calculate final rgb values for pixel
            red = calc_final_rgb(gx_red, gy_red);
            green = calc_final_rgb(gx_green, gy_green);
            blue = calc_final_rgb(gx_blue, gy_blue);

            // replace rgb values for current pixel
            image[i][j].rgbtRed = red;
            image[i][j].rgbtGreen = green;
            image[i][j].rgbtBlue = blue;
        }
    }

    // free memory for image copy
    free(initial_image);

    return;
}

// Calculate average rgb value and replace the initial values with it
void average_rgb(RGBTRIPLE *pixel)
{
    int red, green, blue, average;

    // read initial data
    red = (*pixel).rgbtRed;
    green = (*pixel).rgbtGreen;
    blue = (*pixel).rgbtBlue;

    // calculate average value for red, green and blue
    average = round(((float) red + green + blue) / 3);

    // replace initial values with average
    (*pixel).rgbtRed = average;
    (*pixel).rgbtGreen = average;
    (*pixel).rgbtBlue = average;
}

// swap two selected pixels
void swap_pixels(RGBTRIPLE *px, RGBTRIPLE *py)
{
    RGBTRIPLE temp = *px;
    *px = *py;
    *py = temp;
}

//copy array
void array_copy(int x, int y, RGBTRIPLE array[x][y], RGBTRIPLE array_copy[x][y])
{
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            array_copy[i][j] = array[i][j];
        }
    }
}

// Prepare box_pixel array for selected pixel
void select_box_pixels(int i, int j, box_pixel array[])
{
    // pixel counter
    int k = 0;
    // pixel coordinates
    int x = i - 1;
    int y = j - 1;

    // loop through 9 elements in pixel box
    for (int m = 0; m < 3; m++)
    {
        for (int n = 0; n < 3; n++)
        {
            array[k].x = x; 
            array[k].y = y;
            // next pixel on the right
            y++;
            // next pixel in the box
            k++;
        }
        // next row of pixels in the box
        x++;
        y = j - 1;
    }
    return;
}

// calculate rgb value from Gx and Gy
int calc_final_rgb(int gx, int gy)
{
    int rgb_value = round(sqrt(pow(gx, 2) + pow(gy, 2)));

    // chceck if value is not higher than 255
    if (rgb_value > 255)
    {
        rgb_value = 255;
    }
    return rgb_value;
}