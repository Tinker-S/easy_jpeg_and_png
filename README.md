easy_jpeg_and_png
=================

a encryption of libjpeg and libpng with easy using API

Methods:
void free_rrimage(rrimage *);

unsigned char get_gray(rrimage *, int, int);
unsigned char get_r(rrimage *, int, int);
unsigned char get_g(rrimage *, int, int);
unsigned char get_b(rrimage *, int, int);

void set_gray(unsigned char gray, rrimage *, int, int);
void set_r(unsigned char r, rrimage *, int, int);
void set_g(unsigned char g, rrimage *, int, int);
void set_b(unsigned char b, rrimage *, int, int);

int check_if_png(FILE *);
rrimage* read_jpeg(FILE *);
int write_jpeg(FILE *, rrimage *);
rrimage* read_png(FILE *);
int write_png(FILE *, rrimage *);
