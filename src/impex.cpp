#include "impex.h"

#include <png++/png.hpp>

#include <stdio.h>
#include <stdlib.h>

extern "C" {

vec3f* load_png3f(const char *filename, unsigned int *w, unsigned int *h) {
	png::image<png::rgb_pixel> image(filename);
	*w = image.get_width(),
	*h = image.get_height();
	vec3f *pixels = (vec3f*)malloc(sizeof(vec3f)**w**h);
	for (size_t y = 0; y < *h; ++y)
		for (size_t x = 0; x < *w; ++x) {
			png::rgb_pixel px = image.get_pixel(x, y);
			make_vec3f(pixels+y**w+x, px.red/255.0f, px.green/255.0f, px.blue/255.0f);
		}
	return pixels;
}

void save_png3f(const vec3f *data, unsigned int w, unsigned int h, const char *filename) {
	png::image<png::rgb_pixel> image(w, h);
	for (size_t y = 0; y < h; ++y)
		for (size_t x = 0; x < w; ++x) {
			const vec3f *v = data+y*w+x;
			image[y][x] = png::rgb_pixel(v->x*255.0f, v->y*255.0f, v->z*255.0f);
		}
	image.write(filename);
}

void save_png4f(const vec4f *data, unsigned int w, unsigned int h, const char *filename) {
	png::image<png::rgba_pixel> image(w, h);
	for (size_t y = 0; y < h; ++y)
		for (size_t x = 0; x < w; ++x) {
			const vec4f *v = data+y*w+x;
			image[y][x] = png::rgba_pixel(v->x*255.0f, v->y*255.0f, v->z*255.0f, v->w*255.0f);
		}
	image.write(filename);
}

}

/*
#include <wand/MagickWand.h>
#include <wand/magick-property.h>

int test_im(char *in, char *out)
{
#define ThrowWandException(wand) \
	{ \
		char \
		*description; \
		\
		ExceptionType \
		severity; \
		\
		description=MagickGetException(wand,&severity); \
		(void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description); \
		description=(char *) MagickRelinquishMemory(description); \
		exit(-1); \
	}

	MagickBooleanType
		status;

	MagickWand
		*magick_wand;

	MagickWandGenesis();
	magick_wand=NewMagickWand();
	status=MagickReadImage(magick_wand,in);
	if (status == MagickFalse)
		ThrowWandException(magick_wand);
	double x,y;
	MagickGetResolution(magick_wand, &x, &y);
	printf("X: %e Y: %e\n", x, y);

	MagickResetIterator(magick_wand);
	while (MagickNextImage(magick_wand) != MagickFalse)
		MagickResizeImage(magick_wand,106,80,LanczosFilter,1.0);
	status=MagickWriteImages(magick_wand,out,MagickTrue);
	if (status == MagickFalse)
		ThrowWandException(magick_wand);
	magick_wand=DestroyMagickWand(magick_wand);
	MagickWandTerminus();
	return(0);
}

*/
