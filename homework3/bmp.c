#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "image_server.h"

// calculate the number of bytes of memory needed to serialize the bitmap
// that is, to write a valid bmp file to memory
size_t bmp_calculate_size(bitmap_t *bmp) {
    size_t file_header_size = sizeof(BITMAPFILEHEADER);
    size_t info_header_size = sizeof(BITMAPINFOHEADER);
    size_t data_size = 3 * bmp->width * bmp->height;
    size_t total_size = file_header_size + info_header_size + data_size;
    return total_size;
}

// write the bmp file to memory at data, which must be at least
// bmp_calculate_size large.
void bmp_serialize(bitmap_t *bmp, uint8_t *data) {
    BITMAPFILEHEADER file_header = {0};
    BITMAPINFOHEADER info_header = {0};

    file_header.bfType = 0x4d42;
    file_header.bfSize = bmp_calculate_size(bmp);
    file_header.bfReserved1 = 0;
    file_header.bfReserved2 = 0;
    file_header.bfOffBits = sizeof(file_header) + sizeof(info_header);

    //info_header.biSize = sizeof(info_header);
    info_header.biSize = 40;
    info_header.biWidth = bmp->width;
    info_header.biHeight = bmp->height;
    info_header.biPlanes = 1;
    //info_header.biBitCount = 3 * sizeof(uint8_t) * 8;
    info_header.biBitCount = 24;
    info_header.biCompression = 0;
    //info_header.biCompression = BI_RGB;
    info_header.biSizeImage = 0;
    //info_header.biSizeImage = bmp->width * bmp->height * 3;
    info_header.biXPelsPerMeter = 2835;
    info_header.biYPelsPerMeter = 2835;
    info_header.biClrUsed = 0;
    info_header.biClrImportant = 0;

    uint8_t *data_out = data;

    memcpy(data_out, &file_header, sizeof(file_header));
    data_out += sizeof(file_header);

    memcpy(data_out, &info_header, sizeof(info_header));
    data_out += sizeof(info_header);

    for (int i = 0; i < bmp->height; i++) {
        memcpy(data_out, &bmp->data[bmp->width * (bmp->height - (i + 1))], 3 * bmp->width);
        data_out += 3 * bmp->width;
    }
}
