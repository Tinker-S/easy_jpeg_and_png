//
//  rrimage.c
//
//  Created by Tristan Sun on 12-10-23.
//  Copyright (c) 2012年 Personal. All rights reserved.
//

#include "rrimage.h"

METHODDEF(void) my_error_exit(j_common_ptr cinfo) {
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	longjmp(myerr->setjmp_buffer, 1);
}

void free_rrimage(rrimage *data) {
    if (!data) {
        return;
    }
    
    if (data->pixels) {
        free(data->pixels);
    }
    
    free(data);
}

int check_if_png(FILE *fp) {
	if (fp == NULL) {
		return 0;
	}
    
	char buf[PNG_MAGIC_SIZE];
    
	if (fread(buf, sizeof(char), PNG_MAGIC_SIZE, fp) != PNG_MAGIC_SIZE) {
		return 0;
	}

    // reset file pointer
    fseek(fp, 0L, SEEK_SET);
    
	return (!png_sig_cmp((void *)buf, (png_size_t) 0, PNG_MAGIC_SIZE));
}

rrimage* read_jpeg(FILE *in_file) {
    if (in_file == NULL) {
        return NULL;
    }
    
    rrimage *data = (rrimage *) malloc (sizeof(rrimage));
    
    struct jpeg_decompress_struct in;
	struct my_error_mgr in_err;
    
    in.err = jpeg_std_error(&in_err.pub);
	in_err.pub.error_exit = my_error_exit;
	if (setjmp(in_err.setjmp_buffer)) {
		jpeg_destroy_decompress(&in);
		return NULL;
	}
    
    jpeg_create_decompress(&in);
	jpeg_stdio_src(&in, in_file);
	jpeg_read_header(&in, TRUE);
    
	jpeg_start_decompress(&in);
    
    data->width = in.image_width;
    data->height = in.image_height;
    data->channels = in.output_components;
    
    int row_stride = data->width * data->channels;
    data->pixels = (unsigned char *) malloc (row_stride * data->height);

    JSAMPROW row_pointer[1];
    while (in.output_scanline < data->height) {
        row_pointer[0] = (&data->pixels[in.output_scanline * row_stride]);
        jpeg_read_scanlines(&in, row_pointer, 1);
    }
    
    jpeg_finish_decompress(&in);
	jpeg_destroy_decompress(&in);
    
    return data;
}

int write_jpeg(FILE *out_file, rrimage *data) {
    if (out_file == NULL || data == NULL || data->pixels == NULL) {
        return 0;
    }
    
    struct jpeg_compress_struct out;
	struct my_error_mgr out_err;
    
    out.err = jpeg_std_error(&out_err.pub);
	out_err.pub.error_exit = my_error_exit;
	if (setjmp(out_err.setjmp_buffer)) {
		jpeg_destroy_compress(&out);
		return 0;
	}
    
    jpeg_create_compress(&out);
	jpeg_stdio_dest(&out, out_file);
    
    out.image_width = data->width;
    out.image_height = data->height;
    out.input_components = data->channels;
    
    if (out.input_components == 1) {
        out.in_color_space = JCS_GRAYSCALE;
    } else {
        out.in_color_space = JCS_RGB;
    }
    
    jpeg_set_defaults(&out);
	jpeg_start_compress(&out, TRUE);
    
    JSAMPROW row_pointer[1];
    int row_stride = data->width * data->channels;
    
    while (out.next_scanline < out.image_height) {
        row_pointer[0] = &data->pixels[out.next_scanline * row_stride];
		jpeg_write_scanlines(&out, row_pointer, 1);
	}
    
    jpeg_finish_compress(&out);
	jpeg_destroy_compress(&out);
    
    return 1;
}

rrimage* read_png(FILE *in_file) {
    if (in_file == NULL) {
        return NULL;
    }
    
    rrimage *data = (rrimage *) malloc(sizeof(rrimage));
    
    png_structp in_png_ptr;
	png_infop in_info_ptr;
    
    in_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (in_png_ptr == NULL) {
		return NULL;
	}
    
    in_info_ptr = png_create_info_struct(in_png_ptr);
	if (in_info_ptr == NULL) {
		png_destroy_read_struct(&in_png_ptr, NULL, NULL);
		return NULL;
	}
    
	if (setjmp(png_jmpbuf(in_png_ptr))) {
		png_destroy_read_struct(&in_png_ptr, &in_info_ptr, NULL);
		return NULL;
	}
    
    png_init_io(in_png_ptr, in_file);
	png_read_info(in_png_ptr, in_info_ptr);
    
    data->width = png_get_image_width(in_png_ptr, in_info_ptr);
    data->height = png_get_image_height(in_png_ptr, in_info_ptr);
    int color_type = png_get_color_type(in_png_ptr, in_info_ptr);
    int bit_depth = png_get_bit_depth(in_png_ptr, in_info_ptr);
    
    // strip alpha
    png_set_strip_alpha(in_png_ptr);
    // expand images of all color-type and bit-depth to 3x8 bit RGB images
    if (bit_depth == 16) {
        png_set_strip_16(in_png_ptr);
    }
    if (bit_depth < 8) {
        png_set_expand(in_png_ptr);
    }
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(in_png_ptr);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(in_png_ptr);
    }
    
    png_read_update_info(in_png_ptr, in_info_ptr);
    
    color_type = png_get_color_type(in_png_ptr, in_info_ptr);
    int row_stride = 0;
    png_bytep row_pointers[1];
    
    if (color_type != PNG_COLOR_TYPE_RGB) {
        return NULL;
    }
    
    data->channels = 3;
    row_stride = data->channels * data->width;
    data->pixels = (unsigned char *) malloc(sizeof(unsigned char) * row_stride * data->height);    
    
    for (int i = 0; i < data->height; i++) {
        row_pointers[0] = &data->pixels[i * row_stride];
        png_read_rows(in_png_ptr, row_pointers, NULL, 1);
    }
    
    png_destroy_read_struct(&in_png_ptr, &in_info_ptr, NULL);
    
    return data;
}

int write_png(FILE *out_file, rrimage *data) {
    if (out_file == NULL || data == NULL || data->pixels == NULL) {
        return 0;
    }
    
    png_structp out_png_ptr;
	png_infop out_info_ptr;
    
	out_png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
                                          NULL);
	if (out_png_ptr == NULL) {
		return 0;
	}
    
	out_info_ptr = png_create_info_struct(out_png_ptr);
	if (out_info_ptr == NULL) {
		return 0;
	}
    
	if (setjmp(png_jmpbuf(out_png_ptr))) {
		png_destroy_write_struct(&out_png_ptr, &out_info_ptr);
		return 0;
	}
    
	png_init_io(out_png_ptr, out_file);
	png_set_IHDR(out_png_ptr, out_info_ptr, data->width, data->height, 8, 
                 data->channels == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_GRAY, 
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
    // write header
	if (setjmp(png_jmpbuf(out_png_ptr))) {
		png_destroy_write_struct(&out_png_ptr, &out_info_ptr);
		return 0;
	}
	png_write_info(out_png_ptr, out_info_ptr);
    
    // write data line by line
	if (setjmp(png_jmpbuf(out_png_ptr))) {
		png_destroy_write_struct(&out_png_ptr, &out_info_ptr);
		return 0;
	}
    png_bytep row_pointer[1];
	for (int i = 0; i < data->height; i++) {
        row_pointer[0] = &data->pixels[i * data->width * data->channels];
        png_write_rows(out_png_ptr, row_pointer, 1);
	}
    
    // write end
	if (setjmp(png_jmpbuf(out_png_ptr))) {
		png_destroy_write_struct(&out_png_ptr, &out_info_ptr);
		return 0;
	}
	png_write_end(out_png_ptr, NULL);
    
	png_destroy_write_struct(&out_png_ptr, &out_info_ptr);
    
    return 1;
}

unsigned char get_gray(rrimage *data, int row, int col) {
    if (data == NULL || data->channels != 1) {
        return 0;
    }
    
    return data->pixels[row * data->width + col];
}

unsigned char get_r(rrimage *data, int row, int col) {
    if (data == NULL || data->channels != 3) {
        return 0;
    }
    
    return data->pixels[row * data->width * 3 + col * 3];
}

unsigned char get_g(rrimage *data, int row, int col) {
    if (data == NULL || data->channels != 3) {
        return 0;
    }
    
    return data->pixels[row * data->width * 3 + col * 3 + 1];
}

unsigned char get_b(rrimage *data, int row, int col) {
    if (data == NULL || data->channels != 3) {
        return 0;
    }
    
    return data->pixels[row * data->width * 3 + col * 3 + 2];
}

void set_grey(unsigned char gray, rrimage *data, int row, int col) {
    if (data == NULL || data->channels != 1) {
        return;
    }
    
    data->pixels[row * data->width + col] = gray;
}

void set_r(unsigned char r, rrimage *data, int row, int col) {
    if (data == NULL || data->channels != 3) {
        return;
    }
    
    data->pixels[row * data->width * 3 + col * 3] = r;
}

void set_g(unsigned char g, rrimage *data, int row, int col) {
    if (data == NULL || data->channels != 3) {
        return;
    }
    
    data->pixels[row * data->width * 3 + col * 3 + 1] = g;
}

void set_b(unsigned char b, rrimage *data, int row, int col) {
    if (data == NULL || data->channels != 3) {
        return;
    }
    
    data->pixels[row * data->width * 3 + col * 3 + 2] = b;
}