(load "yacc.lisp")
;(use-package :yacc)

(defparameter *int-types* (list "int" "GLint" "short" "GLshort" "GLsizei" "GLbyte"))
(defparameter *uint-types* (list "GLuint" "GLushort" "GLubyte" "GLenum" "GLboolean" "GLbitfield"))
(defparameter *float-types* (list "float" "GLfloat" "GLclampf"))
(defparameter *double-types* (list "double" "GLdouble" "GLclampd"))
(defparameter *void-types* (list "void" "GLvoid"))

(defun parameter-c-basetype (p)
  (cond ((find p *int-types* :test #'string=) '|int|)
	((find p *uint-types* :test #'string=) '|unsigned int|)
	((find p *float-types* :test #'string=) '|float|)
	((find p *double-types* :test #'string=) '|double|)
	((find p *void-types* :test #'string=) '|void|)
	(t 'invalid)))

(defun scm-to-c-function (basetype)
  (ecase basetype
    (|int| "scm_to_int")
    (|unsigned int| "scm_to_uint")
    ((|float| |double|) "scm_to_double"))) ; it's called like that, yeah.

(defun c-to-scm-function (basetype)
  (ecase basetype
    (|int| "scm_from_int")
    (|unsigned int| "scm_from_uint")
    ((|float| |double|) "scm_from_double")))

(defun pointer-type (list)
  (find "*" list :test #'string=))

(defun array-type (list)
  (find-if  (lambda (s) (find #\[ s :test #'char=)) list))


(defun basetype (list)
  (if (listp list)
      (loop for type in list
	 do (if (not (eq (parameter-c-basetype type) 'invalid))
		(return-from basetype type)))
      (if (not (eq (parameter-c-basetype list) 'invalid))
	  list
	  nil)))
     

(defun c-lexer (&optional (stream *standard-input*))
  (let ((accum-string "")
	(accum-number "")
	(type-words (list "int" "float" "void" "const" "unsigned"
			  "GLenum" "GLint" "GLfloat" "GLushort" "GLsizei" "GLvoid" "GLclampf" "GLboolean" "GLbitfield" "GLclampd" "GLuint" "GLubyte" "GLbyte" "GLdouble" "GLshort" "GLprogramcallbackMESA")))
    (flet ((paste (a b) (format nil "~a~a" a b))
	   (emit (putback) (cond ((> (length accum-string) 0)
				  (if putback (unread-char putback stream))
				  ;(format t "EMIT ~a~%" accum-string)
				  (if (find accum-string type-words :test #'string=)
				      (return-from c-lexer (values 'type-part accum-string))
				      (return-from c-lexer (values 'id accum-string))))
				 ((> (length accum-number) 0)
				  (if putback (unread-char putback stream))
				  ;(format t "EMIT ~a~%" accum-number)
				  (return-from c-lexer (values 'number (parse-integer accum-number))))
				 (t nil))))
      (macrolet ((skip-until (condition-and-action with-accum &body body)
		   `(do ((curr (read-char stream nil nil) (read-char stream nil nil))
			 (prev #\0 curr)
			 ,@(if with-accum `((accum "")) nil))
			,condition-and-action
		      (progn ,@(if with-accum `((setf accum (paste accum curr))))
			     ,@body))))
	(loop (let ((c (read-char stream nil nil)))
		;(format t "current input char is ~a.~%" c)
		(cond ((null c) (return-from c-lexer (values nil nil)))
		      ((alpha-char-p c) (if (> (length accum-number) 0)
					    (setf accum-number (paste accum-number c))
					    (setf accum-string (paste accum-string c))))
		      ((digit-char-p c) (if (> (length accum-string) 0)
					    (setf accum-string (paste accum-string c))
					    (setf accum-number (paste accum-number c))))
		      ((member c '(#\newline #\tab #\space)) (emit nil))
		      ((char= c #\") (skip-until ((and (char= curr #\") (char/= prev #\\))
						  (return-from c-lexer (values 'string accum)) t)))
		      ((char= c #\() (emit c) (return-from c-lexer (values 'left-paren "(")))
		      ((char= c #\)) (emit c) (return-from c-lexer (values 'right-paren ")")))
		      ((char= c #\;) (emit c) (return-from c-lexer (values 'semicolon ";")))
		      ((char= c #\,) (emit c) (return-from c-lexer (values 'comma ",")))
		      ((char= c #\#) (skip-until ((and (char= curr #\newline) (char/= prev #\\))) nil)) ; just skip
		      ((char= c #\/) (let ((next (read-char stream nil nil)))
				       (cond ((char= next #\/)
					      (skip-until ((and (char= curr #\newline) (char/= prev #\\))) nil)) ;just skip
					     ((char= next #\*)
					      (skip-until ((and (char= curr #\/) (char= prev #\*))) nil)))))
		      ((char= c #\*) (emit c) (return-from c-lexer (values 'type-part "*")))
		      ((char= c #\[) (emit c) (skip-until ((char= curr #\]) (return-from c-lexer (values 'array (format nil "[~a]" accum)))) t))
		      )))))))

(defclass decl () ((type :initarg :type) (name :initarg :name)))
(defclass func () ((ret-type :initarg :ret-type) (name :initarg :name) (args :initarg :args)))

(defvar *functions* nil)

(yacc:define-parser *c-header-parser*
  (:start-symbol global-scope)
  (:terminals (id number left-paren right-paren string semicolon comma type-part array))

  (global-scope
   function-declaration
   (function-declaration global-scope))

  (function-declaration
   (type id left-paren right-paren semicolon
	 (lambda (ty i l r s)
	   (declare (ignore r l s))
	   (pushnew (make-instance 'func :ret-type ty :name i :args nil) *functions*)))
   (type id left-paren arglist right-paren semicolon
	 (lambda (ty i l a r s)
	   (declare (ignore r l s))
	   (pushnew (make-instance 'func :ret-type ty :name i :args a) *functions*))))
   
 (type
  (type-part type (lambda (ty seq) (cons ty seq)))
  (type-part (lambda (ty) (list ty))))

 (decl
  (type id array (lambda (ty i a) (format t "FOUDN ARR ~a ~a ~a~%" ty i a) (make-instance 'decl :type (append ty (list a)) :name i)))
  (type id (lambda (ty i) (make-instance 'decl :type ty :name i)))
  (type (lambda (ty) (make-instance 'decl :type ty :name ""))))

 (arglist
  (decl comma arglist (lambda (d c a) (declare (ignore c)) (cons d a)))
  (decl (lambda (d) (list d))))

)

(defun test ()
  (yacc:parse-with-lexer #'c-lexer *c-header-parser*))

(defun read-file (filename)
  (with-open-file (stream filename :direction :input)
    (let ((*standard-input* stream))
      (yacc:parse-with-lexer #'c-lexer *c-header-parser*))))
;    (mapcar (lambda (f)
;	      (with-slots (ret-type name args) f
;		(format t "function ~a   |   R: ~a   |   A: ~a~%" name ret-type args)))
;	    *functions*)))

(defun lisp-name (name)
  (reduce (lambda (seq chr)
	    (if (and (characterp seq) (upper-case-p seq)) ; possible at first call
		(setf seq (char-downcase seq)))
	    (if (upper-case-p chr)
		(concatenate 'string (string seq) "-" (string (char-downcase chr)))
		(concatenate 'string (string seq) (string chr))))
	  name))

(defun gl-name-to-lisp (name)
  (if (not (string= (subseq name 0 2) "gl"))
      (error "~a is not a gl name!" name)
      (concatenate 'string "gl:" (lisp-name (subseq name 2)))))

(defun print-wrapper (fun &optional (stream *standard-output*))
  (with-slots (name ret-type args) fun
    (let* ((arglist (loop for arg in args for i from 0
		       collect (with-slots (type name) arg
				 (if (string= name "")
				     (list type (format nil "arg_~a" i))
				     (list type name)))))
	   (arglist-without-void (remove-if (lambda (arg)
					      (eq (parameter-c-basetype (basetype (first arg)))
						  '|void|))
					    arglist))
	   (flat-arglist (mapcar #'second arglist-without-void))) ;(reduce #'append arglist)))
      ;; check if we better skip this function
      (when (not (basetype ret-type))
	  (format stream "/* skipping wrapper for ~a because of invalid type ~a */~%~%" name ret-type)
	  (return-from print-wrapper nil))
      (when (or (pointer-type ret-type) (array-type ret-type)) 
	  (format stream "/* skipping wrapper for ~a because ~a is a pointer type */~%~%" name ret-type)
	  (return-from print-wrapper nil))
      (loop for (type pname) in arglist
	 do (let ((bt (basetype type))
		  (pt (or (pointer-type type) (array-type type))))
	      (when (not bt)
		(format stream "/* skipping wrapper for ~a because of invalid type ~a */~%~%" name type)
		(return-from print-wrapper nil))
	      (when pt
		(format stream "/* skipping wrapper for ~a because ~a is a pointer type */~%~%" name type)
		(return-from print-wrapper nil))))
      ;; first the header line
      (format stream "SCM_DEFINE(s_~a, \"~a\", ~a, 0, 0, " name (gl-name-to-lisp name) (length args))
      (format stream "(~{SCM ~a~^, ~})" flat-arglist)
      (format stream ", \"\") {~%")
      ;; convert arg list items from scheme
      (dolist (arg arglist)
	(destructuring-bind (type name) arg
	  (let ((base-t (parameter-c-basetype (basetype type))))
	    (if (not (eq base-t '|void|))
		(format stream "	~a c_~a = ~a(~a);~%" base-t name (scm-to-c-function base-t) name)))))
      ;; function call may have a return value
      (let ((ret-basetype (parameter-c-basetype (basetype ret-type))))
	;; start of line, regardless
	(format stream "	")
	(if (not (eq ret-basetype '|void|))
	    (format stream "~a c_result = " ret-basetype))
	;; call the function
	(format stream "~a(~{c_~a~^, ~});~%" name flat-arglist)
	(format stream "	SCM s_result = ~a;~%" (if (eq ret-basetype '|void|)
						  "SCM_BOOL_T"
						  (format nil "~a(c_result)" (c-to-scm-function ret-basetype))))
	(format stream "	return s_result;~%"))
      ;; closing
      (format stream "}~%~%"))))
    

(defun print-wrappers (list &optional (stream *standard-output*))
  (loop for function in list do (print-wrapper function stream)))

(defun generate (from to)
  (macrolet ((line (&optional (str "")) `(format stream "~a~%" ,str)))
    (setf *functions* nil)
    (read-file from)
    (with-open-file (stream to :direction :output :if-exists :supersede)
      (line "/* generated by cgl's gl wrapper generator. licensed under the terms of the GNU GPL (see the cgl license file). */")
      (line)
      (line "#include <libguile.h>")
      (line "#include <GL/gl.h>")
      (line)
      (print-wrappers *functions* stream)
      (line "/* we the actual setup code */")
      (line)
      (line "void register_gl_functions() {")
      (line "	#ifndef GUILE_MAGIC_SNARFER")
      (line (format nil "	#include \"~a.x\"" to))
      (line "	#endif")
      (line "}")
      (line)
      (line "/* we're done now :) */"))))

(let ((cmdline (rest sb-ext:*posix-argv*)))
  (setf *invoke-debugger-hook*
	(lambda (condition hook)
	  (declare (ignore hook))
	  ;; Uncomment to get backtraces on errors
	  ;; (sb-debug:backtrace 20)
	  (format *error-output* "Error: ~A~%" condition)
	  (quit)))
  (if (/= (length cmdline) 2)
      (error "call like: parse <input> <output>"))
  (let ((infile (first cmdline))
	(outfile (second cmdline)))
    (generate infile outfile)))
