#ifdef WITH_GUILE
// part of the logic is implemented in scheme (see scheme/shader.scm)

#define _GNU_SOURCE

#include "shader.h"
#include "mesh.h"		// just for shader-error texture
#include "texture.h"	// just for shader-error texture
#include "prepared.h"	// just for shader-error texture
#include "impex.h"		// just for shader-error texture

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <libguile.h>
void reload_shaders() {
	scm_c_eval_string("(execute-shader-reload \"\")");
	cgl_shader_reload_pending = false;
}

void reload_shader(const char *filename) {
	char *tmp = 0;
	int n = asprintf(&tmp, "(execute-shader-reload \"%s\")", filename);
	scm_c_eval_string(tmp);
	free(tmp);
}

static mesh_ref shader_error_quad = { -1 };
static texture_ref shader_error_tex = { -1 };
static shader_ref shader_error_shader = { -1 };
static char *shader_error_message = 0;

void make_shader_error_display(int w, int h) {
#if LIBCGL_HAVE_LIBCAIRO == 1
	shader_error_quad = make_quad_with_tc("shader error display", 0);
	tex_params_t p = default_fbo_tex_params();
	shader_error_tex =  make_empty_texture("shader error texture", w, h, GL_TEXTURE_2D, GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA, &p);
	shader_error_shader = find_shader("cgl/shader-error-shader");
#else
	fprintf(stderr, "Cairo support was not compiled in, this will not produce output!\n");
#endif
}

bool shader_errors_present() {
	return scm_is_true(scm_c_eval_string("shader-errors"));
}

#if LIBCGL_HAVE_LIBCAIRO == 1

#include <cairo/cairo.h>

static cairo_t *cairo = 0;
static cairo_surface_t *cairo_surface = 0;
static unsigned char *cairo_surface_data = 0;

//! literally taken from cgls/console.c.in
static cairo_t* create_cairo_context(int width, int height, int channels, cairo_surface_t **surf, unsigned char **buffer) {
	cairo_t *cr;

	/* create cairo-surface/context to act as OpenGL-texture source */
	*buffer = (unsigned char*)calloc(channels * width * height, sizeof(unsigned char));
	if (!*buffer) {
		printf("create_cairo_context() - Couldn't allocate surface-buffer\n");
		return 0;
	}

	*surf = cairo_image_surface_create_for_data(*buffer, CAIRO_FORMAT_ARGB32, width, height, channels * width);
	if (cairo_surface_status(*surf) != CAIRO_STATUS_SUCCESS) {
		free(*buffer);
		printf("create_cairo_context() - Couldn't create surface\n");
		return 0;
	}

	cr = cairo_create(*surf);
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		free(*buffer);
		printf("create_cairo_context() - Couldn't create context\n");
		return 0;
	}
	
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);
	return cr;
}

void regenerate_error_texture(char *text) {
	int max_line_len = 256;
	char *line = malloc(max_line_len+1);
	int w = texture_width(shader_error_tex);
	int h = texture_height(shader_error_tex);
	if (!cairo)
		cairo = create_cairo_context(w, h, 4, &cairo_surface, &cairo_surface_data);

	cairo_set_source_rgb(cairo, 0, 0, 0);

	cairo_set_font_size(cairo, 12);
	char *font_name = scm_to_locale_string(scm_c_eval_string("shader-error-font-name"));
	cairo_select_font_face(cairo, font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	free(font_name);
	cairo_font_extents_t fe;
	cairo_font_extents(cairo, &fe);

	int len = strlen(text);
	int start_i = 0;
	int line_nr = 0;
	int xoffset = 20,
		yoffset = 20;
	int max_line_width = 0;
	for (int i = 0; i < len; ++i) {
		if (text[i] == '\n') {
			int line_len = i - start_i;
			if (line_len > max_line_len) {
				free(line);
				line = malloc(line_len+1);
			}
			strncpy(line, text+start_i, line_len);
			line[line_len] = '\0';
			cairo_move_to(cairo, xoffset, yoffset);
			cairo_show_text(cairo, line);
			yoffset += fe.height;
			cairo_text_extents_t te;
			cairo_text_extents(cairo, line, &te);
			if (te.width > max_line_width)
				max_line_width = te.width;
			if (yoffset > h - fe.height - 20) {
				if (max_line_width > w/2-20 || xoffset > w/2) {
					cairo_move_to(cairo, w-40, h-10);
					cairo_set_source_rgb(cairo, 0, 0, .7);
					cairo_show_text(cairo, "...");
					cairo_set_source_rgb(cairo, 0, 0, 0);
					break;
				}
				yoffset = 20;
				xoffset = 20+w/2;
			}
			start_i = i+1;
			++line_nr;
		}
	}
	
	bind_texture(shader_error_tex, 0);
	unsigned char *data = cairo_image_surface_get_data(cairo_surface);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	unbind_texture(shader_error_tex);

// 	save_texture_as_png(shader_error_tex, "tex.png");
}

#endif

void render_shader_error_message() {
#if LIBCGL_HAVE_LIBCAIRO == 1
	char *error_text = scm_to_locale_string(scm_c_eval_string("shader-error-texts"));
	if (!shader_error_message || strcmp(shader_error_message, error_text) != 0) {
		regenerate_error_texture(error_text);
		free(shader_error_message);
		shader_error_message = error_text;
	}
	else
		free(error_text);

// 	texture_ref tex = find_texture("gbuffer/normal");
	texture_ref tex = find_texture("shader error texture");
	glDisable(GL_DEPTH_TEST);
	bind_shader(shader_error_shader);
	bind_mesh_to_gl(shader_error_quad);
	bind_texture(tex, 0);
	draw_mesh(shader_error_quad);
	unbind_texture(tex);
	unbind_mesh_from_gl(shader_error_quad);
	unbind_shader(shader_error_shader);
	glEnable(GL_DEPTH_TEST);
#endif	
}

#include <sys/inotify.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define max_inotify_watches 512
static int inotify_fd = -1;
struct pollfd poll_fds[1];
static int inotify_watches[max_inotify_watches];
static char *inotify_watched_files[max_inotify_watches];
static int inotify_watches_N = 0;

static bool inotify_verbose = false;

void activate_automatic_shader_reload() {
	fprintf(stderr, "ino\n");
	if (inotify_fd == -1) {
		inotify_fd = inotify_init1(IN_NONBLOCK);
		poll_fds[0].fd = inotify_fd;
		poll_fds[0].events = POLLIN;
	}

	SCM file_list = scm_c_eval_string("shader-files");
	while (!scm_is_null(file_list)) {
		SCM car = scm_car(file_list);
		file_list = scm_cdr(file_list);
		char *file = scm_to_locale_string(car);
		if (inotify_watches_N >= max_inotify_watches) {
			fprintf(stderr, "Adding a watch on shader file '%s': too many inotify watches. Can be extended, see %s:%d.\n", file, __FILE__, __LINE__);
			exit(EXIT_FAILURE);
		}
		inotify_watches[inotify_watches_N] = inotify_add_watch(inotify_fd, file, inotify_verbose ? IN_ALL_EVENTS : (IN_MODIFY | IN_IGNORED));
		inotify_watched_files[inotify_watches_N] = file;	// that's why we don't free it.
		if (inotify_watches[inotify_watches_N] == -1) {
			fprintf(stderr, "Cannot watch '%s'\n", file);
			perror("inotify_add_watch");
			exit(EXIT_FAILURE);
		}
		inotify_watches_N++;
	}
}

/* right now we get a SELF_DELETE followed by an IGNORE which meas the watch is dropped. something with how vi saves the file? */

static void handle_inotify_events() {
	/* Some systems cannot read integer variables if they are not
	   properly aligned. On other systems, incorrect alignment may
	   decrease performance. Hence, the buffer used for reading from
	   the inotify file descriptor should have the same alignment as
	   struct inotify_event. */

	static char buf[4096]
		__attribute__ ((aligned(__alignof__(struct inotify_event))));
	const struct inotify_event *event;
	int i;
	ssize_t len;
	char *ptr;

	// Loop while events can be read from inotify file descriptor.
	while (1) {
		// Read events
		len = read(inotify_fd, buf, sizeof buf);
		if (len == -1 && errno != EAGAIN) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		// If the nonblocking read() found no events to read, then it returns
 		// -1 with errno set to EAGAIN. In that case, we exit the loop. 
		if (len <= 0)
			break;

		// Loop over all events in the buffer
		for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
			event = (const struct inotify_event *) ptr;
			char *file = 0;
			int index = 0;
			for (index = 0; index < inotify_watches_N; ++index)
				if (inotify_watches[index] == event->wd) {
					file = inotify_watched_files[index];
					break;
				}
			bool exists = file_exists(file);
			if (inotify_verbose) {
				printf("FILE:  %s\n", file);
				printf("EVENT: %d %d %d %d %s\n", event->wd, event->mask, event->cookie, event->len, event->name);
				if (exists) printf("EXIST: yes.\n");
				else        printf("EXIST: no.\n");

				if (event->mask & IN_ACCESS) printf("M: IN_ACCESS\n");
				if (event->mask & IN_ATTRIB) printf("M: IN_ATTRIB\n");
				if (event->mask & IN_CLOSE_WRITE) printf("M: IN_CLOSE_WRITE\n");
				if (event->mask & IN_CLOSE_NOWRITE) printf("M: IN_CLOSE_NOWRITE\n");
				if (event->mask & IN_CREATE) printf("M: IN_CREATE\n");
				if (event->mask & IN_DELETE) printf("M: IN_DELETE\n");
				if (event->mask & IN_DELETE_SELF) printf("M: IN_DELETE_SELF\n");
				if (event->mask & IN_MODIFY) printf("M: IN_MODIFY\n");
				if (event->mask & IN_MOVE_SELF) printf("M: IN_MOVE_SELF\n");
				if (event->mask & IN_MOVED_FROM) printf("M: IN_MOVED_FROM\n");
				if (event->mask & IN_MOVED_TO) printf("M: IN_MOVED_TO\n");
				if (event->mask & IN_OPEN) printf("M: IN_OPEN\n");
			}
			bool reload = false;
			if (event->mask & IN_MODIFY)
				reload = true;
			else if (event->mask & IN_IGNORED)
				if (exists) {
					reload = true;
					inotify_watches[index] = inotify_add_watch(inotify_fd, file, inotify_verbose ? IN_ALL_EVENTS : (IN_MODIFY | IN_IGNORED));
				}
				else {
					fprintf(stderr, "Missing logic in shader reload: File was (presumably) changed by copying over it (like vim :w does), but the file no longer exists. Maybe it is really gone. Maybe we should poll for a few frames.\n");
					exit(EXIT_FAILURE);
				}
			if (reload) {
				printf("reloading %s\n", file);
				reload_shader(file);
			}
		}
	}
}

void reload_modified_shader_files() {
	int poll_num = poll(poll_fds, 1, 0);
	if (poll_num == -1) {
		if (errno == EINTR)
			return;
		perror("poll for reload_modified_shader_files");
		exit(EXIT_FAILURE);
	}
	if (poll_num > 0) {
		printf("polled something!\n");
		handle_inotify_events();
	}
}

#endif
