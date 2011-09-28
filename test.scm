(display "oh yeah!")(newline)

(define make-shader-ll make-shader)
(define (make-shader name vsrc fsrc inputs)
  (let ((shader (make-shader-ll name (length inputs))))
    (add-vertex-source shader vsrc)
    (add-fragment-source shader fsrc)
	(let rec ((rest inputs) (i 0))
      (add-shader-input shader (car rest) i)
      (if (not (null? (cdr rest)))
          (rec (cdr rest) (1+ i))))
    (compile-and-link-shader shader)
    shader))

(make-shader "line-shader"
             "
             #version 150 core
		     in vec3 in_pos;
		     uniform mat4 proj;
		     uniform mat4 moview;
		     void main() {
		     	gl_Position = proj * moview * vec4(in_pos,1);
		     }"
             "
             #version 150 core
             out vec4 out_col;
             uniform vec3 line_col;
             void main() {
             	out_col = vec4(line_col,1);
             }"
             (list "in_pos"))

(make-perspective-camera "cam-name" (list 0 0 5) (list 0 0 1) (list 0 1 0) 45 1 0.1 1000)

(texture-from-file "gnutex" "/home/kai/render-data/images/floor.png" 'tex2d)
