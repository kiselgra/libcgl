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

(define (vec-ref v i)
  (if (>= i (length v))
      (error "vector ~a does not have an element of index ~a." v i)
      (list-ref v i)))
(define (vec-x v) (vec-ref v 0))
(define (vec-y v) (vec-ref v 1))
(define (vec-z v) (vec-ref v 2))
(define (vec-w v) (vec-ref v 3))
(define (vec-r v) (vec-ref v 0))
(define (vec-g v) (vec-ref v 1))
(define (vec-b v) (vec-ref v 2))
(define (vec-a v) (vec-ref v 3))
