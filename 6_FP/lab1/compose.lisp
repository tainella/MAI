(defun square (x) (* x x))

(defun cube (x) (* x x x))

(defun compose (F G)
(function (lambda (X) (funcall F (funcall G X)))) )
 
(funcall (compose #'square #'1+) 2)
(funcall (compose #'cube #'1-) 2)