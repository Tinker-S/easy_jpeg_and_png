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

    

#endif
