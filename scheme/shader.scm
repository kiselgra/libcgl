(use-modules (ice-9 optargs))

(define make-shader-ll make-shader)
(define* (make-shader name #:key vertex-shader fragment-shader geometry-shader inputs uniforms)
  ;(format #t "make shader ~a ~a ~a~%" name inputs uniforms)
  (let ((old-shader (find-shader name)))
    (if old-shader
	    (destroy-shader old-shader)))
  (let* ((uniforms (if uniforms uniforms '()))
		 (shader (make-shader-ll name (length inputs) (length uniforms))))
    (add-vertex-source shader vertex-shader)
    (add-fragment-source shader fragment-shader)
	(if geometry-shader (add-geometry-source shader geometry-shader))
	(let rec ((rest inputs) (i 0))
      (add-shader-input shader (car rest) i)
      (if (not (null? (cdr rest)))
          (rec (cdr rest) (1+ i))))
	(for-each (lambda (u) 
                ;(format #t "adding uniform ~a~%" u) 
                (add-shader-uniform shader u)) 
              uniforms)
    (if (not (compile-and-link-shader shader))
	    (begin
		  (format #t "info log for shader ~a~%" name)
		  (format #t "vertex info log:~%~a~%---------~%" (vertex-shader-info-log shader))
		  (format #t "fragment info log:~%~a~%---------~%" (fragment-shader-info-log shader))
		  (if geometry-shader (format #t "geometry info log:~%~a~%---------~%" (geometry-shader-info-log shader)))
		  (format #t "linker info log:~%~a~%---------~%" (shader-link-info-log shader))))
    shader))

