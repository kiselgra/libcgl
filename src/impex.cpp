#include "impex.h"

#include <png++/png.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

extern "C" {

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
				image[h-y-1][x] = png::rgba_pixel(v->x*255.0f, v->y*255.0f, v->z*255.0f, v->w*255.0f);
			}
		image.write(filename);
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

	char* find_file(const char *basename)
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
}

