#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

#include "get_image_for_rom.h"

#pragma pack(push,1)
typedef struct{
    uint8_t signature[2];
    uint32_t filesize;
    uint32_t reserved;
    uint32_t fileoffset_to_pixelarray;
} fileheader;

typedef struct{
    uint32_t dibheadersize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitsperpixel;
    uint32_t compression;
    uint32_t imagesize;
    uint32_t ypixelpermeter;
    uint32_t xpixelpermeter;
    uint32_t numcolorspallette;
    uint32_t mostimpcolor;
} bitmapinfoheader;

typedef struct {
    fileheader fileheader;
    bitmapinfoheader bitmapinfoheader;
} bitmap;

#pragma pack(pop)

int main (int argc , char** argv)
{
    char* rom = argv[1];
    char* bootrom = argv[2];
    char* output = argv[3];

    uint32_t pixels[160*144];
    uint8_t cgbFlag = 0;

    if (argc == 4)
    {
        if (get_image_for_rom(rom, bootrom, pixels, &cgbFlag))
        {
            return -1;
        }

        FILE* fp = fopen(output,"wb");
        if (fp)
        {
            bitmap* pbitmap  = malloc(sizeof(bitmap));
            pbitmap->fileheader.signature[0] = 0x42;
            pbitmap->fileheader.signature[1] = 0x4D;

            pbitmap->fileheader.filesize = sizeof(pixels) + sizeof(bitmap);
            pbitmap->fileheader.fileoffset_to_pixelarray = sizeof(bitmap);
            pbitmap->bitmapinfoheader.dibheadersize = sizeof(bitmapinfoheader);
            pbitmap->bitmapinfoheader.width = 160;
            pbitmap->bitmapinfoheader.height = -144;
            pbitmap->bitmapinfoheader.planes = 1;
            pbitmap->bitmapinfoheader.bitsperpixel = 32;
            pbitmap->bitmapinfoheader.compression = 0;
            pbitmap->bitmapinfoheader.imagesize = sizeof(pixels);
            pbitmap->bitmapinfoheader.ypixelpermeter = 0;
            pbitmap->bitmapinfoheader.xpixelpermeter = 0;
            pbitmap->bitmapinfoheader.numcolorspallette = 0;
            fwrite(pbitmap, 1, sizeof(bitmap),fp);
            fwrite(pixels, 1, sizeof(pixels),fp);
            fclose(fp);
            free(pbitmap);
        }
    }
    else
    {
        fprintf(stderr, "%s: [rom] [bootrom] [output]\n", argv[0]);
    }
}
