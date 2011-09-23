(display "oh yeah!")(newline)

(define vsrc "
		#version 150 core
		in vec3 in_pos;
		uniform mat4 proj;
		uniform mat4 moview;
		void main() {
			gl_Position = proj * moview * vec4(in_pos,1);
		}
		")
(define fsrc "
		#version 150 core
		out vec4 out_col;
		uniform vec3 line_col;
		void main() {
			out_col = vec4(line_col,1);
		}
		")
(display "----->")(display
(let ((shader (make-shader "line-shader" 1)))
 (display shader)(display "  ")
  (add-vertex-source shader vsrc)
  (add-fragment-source shader fsrc)
  (add-shader-input shader "in_pos" 0)
  (compile-and-link-shader shader))
	)(newline)