(use-modules (srfi srfi-1))

(define vec-add (lambda args (apply map + args)))
(define vec-sub (lambda args (apply map - args)))
(define vec-mul (lambda args (apply map * args)))
(define vec-div (lambda args (apply map / args)))
(define (vec-dot a b) (reduce + 0 (map * a b)))

(define (vec-mul-by-scalar v s) (vec-mul v (make-list (length v) s)))
(define (vec-div-by-scalar v s) (vec-div v (make-list (length v) s)))

(define (vec-length v) (sqrt (vec-dot v v)))

(define make-vec list)
