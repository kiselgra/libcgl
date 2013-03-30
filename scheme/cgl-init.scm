(define cgl:*features* '(guile))

(define (cgl:has-feature f)
  (if (member f cgl:*features*)
      #t
      #f))

(define (cgl:push-feature f)
  (set! cgl:*features* (cons f cgl:*features*)))
