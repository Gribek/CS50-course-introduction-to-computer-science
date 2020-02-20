#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // check number of provided command line arguments
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    // open memory card file
    FILE *memory_card = fopen(argv[1], "r");
    if (memory_card == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 1;
    }

    // alocate memory for 512 bytes of data
    BYTE *card_data = malloc(sizeof(BYTE) * 512);

    // number of jpeg files found
    int jpeg_count = 0;

    // name of the jpeg file
    char jpeg_name[8];

    // set file pointer to null
    FILE *img = NULL;

    // read first 512 bytes of memory card data
    int amount_of_data = fread(card_data, sizeof(BYTE), 512, memory_card);

    while (amount_of_data == 512)
    {
        // check if this is start of new JPEG
        if (card_data[0] == 0xff && card_data[1] == 0xd8 && card_data[2] == 0xff && (card_data[3] & 0xf0) == 0xe0)
        {
            // create name for new jpeg
            sprintf(jpeg_name, "%03i.jpg", jpeg_count);

            // increment number of jpeg files found
            jpeg_count++;

            // check if this is a first jpeg found
            if (jpeg_count == 1)
            {
                img = fopen(jpeg_name, "w");
            }
            else
            {
                fclose(img);
                img = fopen(jpeg_name, "w");
            }
            if (img == NULL)
            {
                printf("Could not create new jpeg file");
                return 1;
            }
            // write data to img file
            fwrite(card_data, sizeof(BYTE), 512, img);
        }
        else
        {
            // check if first file has been found
            if (jpeg_count > 0)
            {
                // write data to img file
                fwrite(card_data, sizeof(BYTE), 512, img);
            }
        }
        // read next 512 bytes of memory card data
        amount_of_data = fread(card_data, sizeof(BYTE), 512, memory_card);
    }

    // write last part of data
    fwrite(card_data, sizeof(BYTE), amount_of_data, img);

    // check if any jpeg has been found
    if (jpeg_count > 0)
    {
        // close last jpeg file
        fclose(img);
    }

    // close memory card file
    fclose(memory_card);

    // free memory for 512 bytes of card data
    free(card_data);

    return 0;
}
