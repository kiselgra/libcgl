(use-modules (ice-9 optargs))
(use-modules (ice-9 receive))

(define *fragments* '())
(define* (shader-fragment name code #:key (uniforms '()))
  (set! *fragments* (cons (list name code uniforms) *fragments*)))

(define (find-fragment name)
  (let loop ((frags *fragments*))
    (if (null? frags)
        #f
        (if (string=? (car (car frags)) name)
            (car frags)
            (loop (cdr frags))))))

(define (handle-fragments code)
  (if code
      (let ((pos (string-contains code ",(use ")))
        (if pos
            (let* ((q1 (string-contains code "\"" pos))
                   (q2 (string-contains code "\"" (1+ q1)))
                   (end (string-contains code ")" q2))
                   (fragment-name (substring code (1+ q1) q2))
                   (pre (substring code 0 pos))
                   (post (substring code (1+ end)))
                   (fragment (find-fragment fragment-name)))
              (let ((new-code (string-append pre (cadr fragment) post))
                    (new-uniforms (caddr fragment)))
                (receive (newer-code newer-uniforms) (handle-fragments new-code)
                  (values newer-code (append new-uniforms newer-uniforms)))))
            (values code '())))
      (values #f '())))
      
(define (format-errors name code log headline)
  (if (not (null? log))
      (call-with-output-string
       (lambda (port)
	 (format port "    ~a for shader ~a:~%" headline name)
	 (let ((lines (string-split log #\newline))
	       (code-lines (string-split code #\newline)))
	   (for-each (lambda (line)
		       (if (not (string=? line ""))
			   (format port "    - ~a~%" line))
		       (when (or (string-contains line "warning")
				 (string-contains line "error"))
			 (let* ((a (1+ (string-contains line "(")))
				(b (string-contains line ")"))
				(sub (substring line a b))
				(num (string->number sub)))
			   (let loop ((rest code-lines) (i 1))
			     (if (= i num)
				 (format port "    ~a~%" (car rest))
				 (loop (cdr rest) (1+ i)))))))
		     lines))
	 (format port "~%")))
      ""))

;(define (show-errors name code log headline)
;  (format #f "~a~%~a~%" headline log))

(define (shader-error-output shader name vert-source frag-source geom-source tcs-source tes-source compute-source)
  (let ((str (format #f "INFO LOG FOR SHADER ~a~%" name)))
    (if vert-source     (set! str (string-append str (format-errors name vert-source (vertex-shader-info-log shader) "VERTEX LOG"))))
    (if frag-source     (set! str (string-append str (format-errors name frag-source (fragment-shader-info-log shader) "FRAGMENT LOG"))))
    (if geom-source     (set! str (string-append str (format-errors name geom-source (geometry-shader-info-log shader) "GEOMETRY LOG"))))
    (if tcs-source      (set! str (string-append str (format-errors name tcs-source (tesselation-control-shader-info-log shader) "TESSELATION CONTROL LOG"))))
    (if tes-source      (set! str (string-append str (format-errors name tes-source (tesselation-evaluation-shader-info-log shader) "TESSELATION EVALUATION LOG"))))
    (if compute-source  (set! str (string-append str (format-errors name compute-source (compute-shader-info-log shader) "COMPUTE LOG"))))
    (let ((link-log (shader-link-info-log shader)))
      (if (not (null? link-log))
	  (set! str (string-append str (format-errors name "" link-log "LINK LOG")))))
    (display str)(newline)
    (set! shader-error-texts (string-append shader-error-texts str))
    str))

(define make-shader-ll make-shader)
(define* (make-shader name #:key vertex-shader fragment-shader geometry-shader tess-control-shader tess-eval-shader compute-shader inputs uniforms)
  (let ((old-shader (find-shader name)))
    (if old-shader
	    (destroy-shader old-shader)))
  (receive (vert-source vert-uniforms) (handle-fragments vertex-shader)
    (receive (frag-source frag-uniforms) (handle-fragments fragment-shader)
      (receive (geom-source geom-uniforms) (handle-fragments geometry-shader)
        (receive (tcs-source tcs-uniforms) (handle-fragments tess-control-shader)
          (receive (tes-source tes-uniforms) (handle-fragments tess-eval-shader)
            (receive (compute-source compute-uniforms) (handle-fragments compute-shader)
              (let* ((addiditonal-uniforms (append vert-uniforms frag-uniforms geom-uniforms tcs-uniforms tes-uniforms))
                     (uniforms (append (if uniforms uniforms '()) addiditonal-uniforms))
               	     (shader (make-shader-ll name (if inputs (length inputs) 0))))
                (if vert-source (add-vertex-source shader vert-source))
                (if frag-source (add-fragment-source shader frag-source))
                (if geom-source (add-geometry-source shader geom-source))
                (if tcs-source (add-tesselation-control-source shader tcs-source))
                (if tes-source (add-tesselation-evaluation-source shader tes-source))
                (if compute-source (add-compute-source shader compute-source))
                (if inputs
                    (let rec ((rest inputs) (i 0))
                      (add-shader-input shader (car rest) i)
                      (if (not (null? (cdr rest)))
                          (rec (cdr rest) (1+ i)))))
                (for-each (lambda (u) 
                            ;(format #t "adding uniform ~a~%" u) 
                            (add-shader-uniform shader u)) 
                          uniforms)
                (if (not (compile-and-link-shader shader))
                    (shader-error-output shader name vert-source frag-source geom-source tcs-source tes-source compute-source))
                shader))))))))

;; shader reload
(define shader-files '())
(define (load-shader-file file)
  (set! shader-files (cons file shader-files))
  (primitive-load file))
(define shader-error-texts "")
(define shader-errors #f)
(define (execute-shader-reload)
  (set! shader-error-texts "")
  (set! shader-errors #f)
  (for-each primitive-load shader-files)
  (if (string<> "" shader-error-texts)
      (let ((logfile (string-append "/tmp/shader-errors:" (getlogin))))
	(set! shader-errors #t)
	(set! shader-error-texts (format #f "~
Some shaders did not compile.
-----------------------------
A complete log can be found at ~a.~%~%~a" logfile shader-error-texts))
	(with-output-to-file logfile
	  (lambda () (format #t shader-error-texts))))))

(define reload-shaders trigger-reload-of-shader-files)

(make-shader "cgl/shader-error-shader"
#:vertex-shader
"#version 150 core
in vec3 in_pos;
in vec2 in_tc;
out vec2 tc;
void main() {
    gl_Position = vec4(in_pos.xy, -0.8, 1.0);
    tc = in_tc;
}"
#:fragment-shader
"#version 420 core
in vec2 tc;
uniform layout(binding = 0) sampler2D tex;
out vec4 col;
void main() {
    col = vec4(texture(tex, tc).rgb, 1);
}")
