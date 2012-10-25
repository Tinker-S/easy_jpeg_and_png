//
//  rrimage.h
//
//  Created by Tristan Sun on 12-10-23.
//  Copyright (c) 2012年 Personal. All rights reserved.
//

#ifndef rrimage_h
#define rrimage_h

#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <jpeglib.h>
#include <setjmp.h>

#define PNG_MAGIC_SIZE 4

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int channels;
    unsigned char *pixels;
}rrimage;

typedef struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
}* my_error_ptr;

int clamp(int);

void free_rrimage(rrimage *);

unsigned char get_gray(rrimage *, int, int);

unsigned char get_r(rrimage *, int, int);

unsigned char get_g(rrimage *, int, int);

unsigned char get_b(rrimage *, int, int);

void set_gray(unsigned char gray, rrimage *, int, int);

void set_r(int r, rrimage *, int, int);

void set_g(int g, rrimage *, int, int);

void set_b(int b, rrimage *, int, int);

int check_if_png(char *);

rrimage* read_jpeg(char *);

int write_jpeg(char *, rrimage *);

rrimage* read_png(char *);

int write_png(char *, rrimage *);

#endif
