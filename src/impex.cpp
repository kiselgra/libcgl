#include "impex.h"
#include "cgl-config.h"

#if LIBCGL_HAVE_LIBPNG == 1
#include <png++/png.hpp>
#endif

#if LIBCGL_HAVE_LIBJPEG == 1
#include <jpeglib.h>
#include <jerror.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

extern "C" {

#if LIBCGL_HAVE_LIBPNG == 1
	vec3f* load_png3f(const char *filename, unsigned int *w, unsigned int *h) {
		png::image<png::rgb_pixel> image(filename);
		*w = image.get_width(),
		*h = image.get_height();
		vec3f *pixels = (vec3f*)malloc(sizeof(vec3f)**w**h);
		for (size_t y = 0; y < *h; ++y)
			for (size_t x = 0; x < *w; ++x) {
				png::rgb_pixel px = image.get_pixel(x, *h-y-1);
				make_vec3f(pixels+y**w+x, px.red/255.0f, px.green/255.0f, px.blue/255.0f);
			}
		return pixels;
	}

	unsigned char* load_png3ub(const char *filename, unsigned int *w, unsigned int *h) {
		png::image<png::rgb_pixel> image(filename);
		*w = image.get_width(),
		*h = image.get_height();
		unsigned char *pixels = (unsigned char*)malloc(3*sizeof(unsigned char)**w**h);
		for (size_t y = 0; y < *h; ++y)
			for (size_t x = 0; x < *w; ++x) {
				png::rgb_pixel px = image.get_pixel(x, *h-y-1);
				int i = 3*(y**w+x);
				pixels[i+0] = px.red;
				pixels[i+1] = px.green;
				pixels[i+2] = px.blue;
			}
		return pixels;
	}

	void save_png1f(const float *data, unsigned int w, unsigned int h, const char *filename) {
		png::image<png::rgb_pixel> image(w, h);
		for (size_t y = 0; y < h; ++y)
			for (size_t x = 0; x < w; ++x) {
				float val = data[y*w+x] * 255.0f;
				image[h-y-1][x] = png::rgb_pixel(val, val, val);
			}
		image.write(filename);
	}

	void save_png3f(const vec3f *data, unsigned int w, unsigned int h, const char *filename) {
		png::image<png::rgb_pixel> image(w, h);
		for (size_t y = 0; y < h; ++y)
			for (size_t x = 0; x < w; ++x) {
				const vec3f *v = data+y*w+x;
				image[h-y-1][x] = png::rgb_pixel(v->x*255.0f, v->y*255.0f, v->z*255.0f);
			}
		image.write(filename);
	}

	void save_png4f(const vec4f *data, unsigned int w, unsigned int h, const char *filename) {
		png::image<png::rgba_pixel> image(w, h);
		for (size_t y = 0; y < h; ++y)
			for (size_t x = 0; x < w; ++x) {
				const vec4f *v = data+y*w+x;
// 				image[h-y-1][x] = png::rgba_pixel(v->x*255.0f, v->y*255.0f, v->z*255.0f, v->w*255.0f);
				image[h-y-1][x] = png::rgba_pixel(v->x*255.0f, v->y*255.0f, v->z*255.0f, 255);
			}
		image.write(filename);
	}
#endif

#if LIBCGL_HAVE_LIBJPEG == 1
	unsigned char *load_jpeg_data(const char *filename, int *bytes_per_pixel, unsigned int *w, unsigned int *h) {
		FILE *fd;
		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr jerr;
		unsigned char *line;

		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&cinfo);
		fd = fopen(filename, "rb");
		if (fd == 0)
			return 0;

		jpeg_stdio_src(&cinfo, fd);
		jpeg_read_header(&cinfo, true);

		if (cinfo.out_color_space == JCS_GRAYSCALE)
			*bytes_per_pixel = 1;
		else if (cinfo.out_color_space == JCS_RGB)
			*bytes_per_pixel = 3;
// 		else if (cinfo.out_color_space == JCS_EXT_RGBA)
// 			*bytes_per_pixel = 4;
		else return 0;

		unsigned char *data = (unsigned char*)malloc(*bytes_per_pixel * cinfo.output_width * cinfo.output_height);
		*w = cinfo.output_width;
		*h = cinfo.output_height;

// 		unsigned char *data = (unsigned char*)malloc(*bytes_per_pixel * cinfo.image_width * cinfo.image_height);
// 		*w = cinfo.image_width;
// 		*h = cinfo.image_height;

		jpeg_start_decompress(&cinfo);

		while (cinfo.output_scanline < cinfo.output_height)
		{
			line = data + *bytes_per_pixel * cinfo.output_scanline;
			jpeg_read_scanlines(&cinfo, &line, 1);
		}
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		return data;
	}
	
	vec3f* load_jpeg3f(const char *filename, unsigned int *w, unsigned int *h) {
		int bpp;
		unsigned char *data = load_jpeg_data(filename, &bpp, w, h);
		vec3f *pixels = (vec3f*)malloc(sizeof(vec3f)**w**h);
		if (data) {
			for (int y = 0; y < *h; ++y)
				for (int x = 0; x < *w; ++x)
					if (bpp == 1)
						pixels[y**w+x].x = pixels[y**w+x].y = pixels[y**w+x].z = data[y**w+x] / 255.0;
					else {
						pixels[y**w+x].x = data[3*(y**w+x)] / 255.0;
						pixels[y**w+x].y = data[3*(y**w+x)+1] / 255.0;
						pixels[y**w+x].z = data[3*(y**w+x)+2] / 255.0;
					}
		}
		else {
			fprintf(stderr, "Could not read '%s': aborting.\n", filename);
			exit(1);
		}
		return pixels;
	}
#endif

	// rather stupid check on file "extensions"
	enum format { f_png, f_jpeg, f_unknown };
	format guess_image_format(const char *filename) {
		int l = strlen(filename);
		char *str = strdup(filename);
		char *e3 = str + l - 3;
		char *e4 = str + l - 4;
		for (int i = 0; i < 4; ++i) e4[i] = tolower(e4[i]);
		format f = f_unknown;
		if (strcmp(e4, "jpeg") == 0) f = f_jpeg;
		if (strcmp(e3, "jpg") == 0) f = f_jpeg;
		if (strcmp(e3, "png") == 0) f = f_png;
		free(str);
		return f;
	}
	vec3f *load_image3f(const char *filename, unsigned int *w, unsigned int *h) {
		format f = guess_image_format(filename);
#if LIBCGL_HAVE_LIBPNG == 1
		if (f == f_png)
			return load_png3f(filename, w, h);
#endif
#if LIBCGL_HAVE_LIBJPEG == 1
		if (f == f_jpeg) 
			return load_jpeg3f(filename, w, h);
#endif
		fprintf(stderr, "Cannot guess image format of '%s', or format unsupported (maybe not compiled in?).\n", filename);
		exit(1);
	}

	std::list<std::string> image_paths;
	struct initial_paths
	{
		initial_paths()
		{
			image_paths.push_back(".");
		}
	} initialize_paths;
	
	bool file_exists(const char *name)
	{
		struct stat st;
		int stat_res = stat(name, &st);
		if (stat_res == 0)
			return true;
		else
			return false;
	}

	char* find_file_default_version(const char *basename)
	{
		if (basename[0] == '/') {
			if (file_exists(basename))
				return strdup(basename);
		}
		else
			for (std::list<std::string>::const_iterator it = image_paths.begin(); it != image_paths.end(); ++it)
			{
				string p = *it;
				if (p[p.length()-1] != '/')
					p += "/";
				p += basename;
				if (file_exists(p.c_str()))
					return strdup(p.c_str());
			}
		return 0;
	}

	void append_image_path(const char *path) {
		image_paths.push_back(path);
	}

	void prepend_image_path(const char *path) {
		image_paths.push_front(path);
	}

	void pop_image_path_front() {
		image_paths.pop_front();
	}

	void pop_image_path_back() {
		image_paths.pop_back();
	}

	static char* (*file_lookup_function)(const char*) = find_file_default_version;
	void register_file_lookup_function(char*(*func)(const char*)) {
		file_lookup_function = func;
	}

	char* find_file(const char *basename) {
		return file_lookup_function(basename);
	}
}

#ifdef WITH_GUILE
#include <libguile.h>
#include <stdio.h>
#include "scheme.h"

extern "C" {
	SCM_DEFINE(s_append_image_path, "append-image-path", 1, 0, 0, (SCM p), "") {
		char *path = scm_to_locale_string(p);
		append_image_path(path);
		free(path);
		return p;
	}

	SCM_DEFINE(s_prepend_image_path, "prepend-image-path", 1, 0, 0, (SCM p), "") {
		char *path = scm_to_locale_string(p);
		prepend_image_path(path);
		free(path);
		return p;
	}

	void register_scheme_functions_for_impex() {
	#ifndef SCM_MAGIC_SNARFER
	#include "impex.x"
	#endif
	}


}

#endif

