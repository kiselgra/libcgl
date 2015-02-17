#include "impex.h"
#include "cgl-config.h"

#include <string.h>	// because of png++ bug.
#if LIBCGL_HAVE_LIBPNG == 1
#include <png++/png.hpp>
#endif

#if LIBCGL_HAVE_MAGICKWAND == 1
#include <wand/MagickWand.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*! \defgroup impex Import & Export
 *
 *  This file provieds a few methods to read in image data.
 *  We recommend that you use ImageMagick, which is defaulted to if it is found at configure time.
 *
 *  Furthermore a list of paths is managed. These are (in order) searched for images that are being looked up.
 */

/*! \file impex.h
 *  \ingroup impex
 *
 *  This file provieds a few methods to read in image data.
 *  We recommend that you use ImageMagick, which is defaulted to if it is found at configure time.
 *
 *  Furthermore a list of paths is managed. These are (in order) searched for images that are being looked up.
 */

using namespace std;

extern "C" {

    /*! \addtogroup impex
     *  @{
     */
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

	vec4f* load_png4f(const char *filename, unsigned int *w, unsigned int *h) {
		png::image<png::rgba_pixel> image(filename);
		*w = image.get_width(),
		*h = image.get_height();
		vec4f *pixels = (vec4f*)malloc(sizeof(vec4f)**w**h);
		for (size_t y = 0; y < *h; ++y)
			for (size_t x = 0; x < *w; ++x) {
				png::rgba_pixel px = image.get_pixel(x, *h-y-1);
				make_vec4f(pixels+y**w+x, px.red/255.0f, px.green/255.0f, px.blue/255.0f, px.alpha/255.0f);
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
    //! @}

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

#if LIBCGL_HAVE_MAGICKWAND == 1
	void magickwand_error(MagickWand *wand) {
		char *description;
		ExceptionType severity;

		description=MagickGetException(wand,&severity);
		fprintf(stderr, "%s %s %lu\n", GetMagickModule(), description, severity);
		MagickRelinquishMemory(description);
		exit(1);
	}
#endif

    /*! \addtogroup impex
     *  @{
     */

	vec3f *load_image3f(const char *filename, unsigned int *w, unsigned int *h) {
#if LIBCGL_HAVE_MAGICKWAND == 1
		MagickWandGenesis();
		MagickWand *img = NewMagickWand();
		int status = MagickReadImage(img, filename);
		if (status == MagickFalse) {
			magickwand_error(img);
		}
		MagickFlipImage(img);
		*w = MagickGetImageWidth(img);
		*h = MagickGetImageHeight(img);
		vec3f *pixels = (vec3f*)malloc(sizeof(vec3f)**w**h);
		MagickExportImagePixels(img, 0, 0, *w, *h, "RGB", FloatPixel, (void*)pixels);
		DestroyMagickWand(img);
		MagickWandTerminus();
		return pixels;
#else
		format f = guess_image_format(filename);
#if LIBCGL_HAVE_LIBPNG == 1
		if (f == f_png)
			return load_png3f(filename, w, h);
#endif
#endif
		fprintf(stderr, "Cannot guess image format of '%s', or format unsupported (maybe not compiled in?).\n", filename);
		exit(1);
	}


	vec4f *load_image4f(const char *filename, unsigned int *w, unsigned int *h) {
#if LIBCGL_HAVE_MAGICKWAND == 1
		MagickWandGenesis();
		MagickWand *img = NewMagickWand();
		int status = MagickReadImage(img, filename);
		if (status == MagickFalse) {
			magickwand_error(img);
		}
		MagickFlipImage(img);
		*w = MagickGetImageWidth(img);
		*h = MagickGetImageHeight(img);
		vec4f *pixels = (vec4f*)malloc(sizeof(vec4f)**w**h);
		MagickExportImagePixels(img, 0, 0, *w, *h, "RGBA", FloatPixel, (void*)pixels);
		DestroyMagickWand(img);
		MagickWandTerminus();
		return pixels;
#else
		format f = guess_image_format(filename);
#if LIBCGL_HAVE_LIBPNG == 1
		if (f == f_png) {
			fprintf(stderr, "WARNING: untested image loader used! (%s:%d)\n", __FILE__, __LINE__);
			return load_png4f(filename, w, h);
		}
#endif
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

    //! @}
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

// vim: foldmethod=marker: 
