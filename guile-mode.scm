(use-modules (system foreign))

(define cgl (dynamic-link "libcgl"))
(define mcm (dynamic-link "libmcm.so"))

;; actual startup
(define startup-cgl
  (let ((startup_cgl (pointer->procedure void (dynamic-func "startup_cgl" cgl) (list '* int int int '* int int int))))
    (lambda (title res-x res-y verbose)
      (startup_cgl (string->pointer title) 3 3 0 %null-pointer res-x res-y (if verbose 1 0)))))

;; the glut functions

(define register-display-function 
  (let ((register_display_function (pointer->procedure void (dynamic-func "register_display_function" cgl) (list '*))))
    (lambda (f) (register_display_function (procedure->pointer void f (list))))))

(define register-idle-function 
  (let ((register_idle_function (pointer->procedure void (dynamic-func "register_idle_function" cgl) (list '*))))
    (lambda (f) (register_idle_function (procedure->pointer void f (list))))))

(define register-keyboard-function 
  (let ((register_keyboard_function (pointer->procedure void (dynamic-func "register_keyboard_function" cgl) (list '*))))
    (lambda (f) (register_keyboard_function (procedure->pointer void f (list uint8 int int))))))

(define register-mouse-function
  (let ((register_mouse_function (pointer->procedure void (dynamic-func "register_mouse_function" cgl) (list '*))))
    (lambda (f) (register_mouse_function (procedure->pointer void f (list int int int int))))))

(define register-motion-function
  (let ((register_motion_function (pointer->procedure void (dynamic-func "register_mouse_motion_function" cgl) (list '*))))
    (lambda (f) (register_motion_function (procedure->pointer void f (list int int))))))

(define register-motion-function
  (let ((register_motion_function (pointer->procedure void (dynamic-func "register_mouse_motion_function" cgl) (list '*))))
    (lambda (f) (register_motion_function (procedure->pointer void f (list int int))))))

(define enter-glut-mainloop
  (let ((enter_loop (pointer->procedure void (dynamic-func "enter_glut_main_loop" cgl) (list))))
    (lambda () (enter_loop))))
