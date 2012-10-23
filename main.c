//
//  main.cpp
//  ImageProcess
//
//  Created by Tristan Sun on 12-10-23.
//  Copyright (c) 2012年 Personal. All rights reserved.
//

#include "rrimage.h"


void test_write_jpeg(FILE *out_file) {
    rrimage *data = (rrimage *) malloc(sizeof(rrimage));
    data->width = 3200;
    data->height = 2400;
    data->channels = 3;
    data->pixels = (unsigned char *) malloc(sizeof(unsigned char) * data->width * data->height * data->channels);
    
    int row_stride = data->width * data->channels;
    
    for (int i = 0; i < 2400; i++) {
        for (int j = 0; j < 3200; j++) {
            data->pixels[i * row_stride + j * 3] = 192;
            data->pixels[i * row_stride + j * 3 + 1] = 205;
            data->pixels[i * row_stride + j * 3 + 2] = 33;
        }
    }
    
    write_jpeg(out_file, data);
    
    free_rrimage(data);
}

int main(int argc, const char * argv[])
{
//    char *infilename = "/Users/tristan/Desktop/hello.jpg";
//    char *outfilename = "/Users/tristan/Desktop/out.jpg";
    char *infilename = "/Users/tristan/Desktop/hello.png";
    char *outfilename = "/Users/tristan/Desktop/out.png";
    
    FILE *in_file, *out_file;
    if ((in_file = fopen(infilename, "rb")) == NULL) {
        printf("error read in image file...\n");
        return 0;
    }
    if ((out_file = fopen(outfilename, "wb")) == NULL) {
        printf("error read out image file...\n");
        return 0;
    }
    
//    test_write_jpeg(out_file);
    
/*
    rrimage *data = read_jpeg(in_file);
    
    for (int i = 0; i < data->height; i++) {
        for (int j = 0; j < data->width; j++) {
            unsigned char r = get_r(data, i, j);
            unsigned char g = get_g(data, i, j);
            unsigned char b = get_b(data, i, j);
            
            set_r(r + 20, data, i, j);
            set_g(g, data, i, j);
            set_b(b, data, i, j);
        }
    }

    write_jpeg(out_file, data);
    
    free_rrimage(data);
*/
    
    rrimage *data = read_png(in_file);
    
    for (int i = 0; i < data->height; i++) {
        for (int j = 0; j < data->width; j++) {
            unsigned char r = get_r(data, i, j);
            unsigned char g = get_g(data, i, j);
            unsigned char b = get_b(data, i, j);
            
            set_r(r, data, i, j);
            set_g(g, data, i, j);
            set_b(0, data, i, j);
        }
    }
    
    printf("write result = %d\n", write_png(out_file, data));
    
    free_rrimage(data);
    
    printf("success\n");
    
    return 0;
}

