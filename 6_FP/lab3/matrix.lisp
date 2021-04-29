(defun print-matrix (matrix &optional (chars 3) stream)
  (let ((*print-right-margin* (+ 6 (* (1+ chars)
                                      (array-dimension matrix 1)))))
    (pprint matrix stream)
    (values)))

(defun find_max(a ii jj)
	(let ((max (aref a 0 jj)))
		(loop for i from 0 to (- ii 1) do
      		(loop with m = (array-dimension a 1)
      			for j from jj to (- m 1) do
      				(if (< max (aref a i j))
      				(setf max (aref a i j)))))
    max))

(defun max_matrix (a)
	(let ((b a))
		(loop with n = (array-dimension a 0)
      	for i upfrom 0 below n do
      		(loop with m = (array-dimension a 1)
      		for j upfrom 0 below m do
            	(setf (aref b i j) (find_max a i j))))
    b))

(defvar a (make-array ’(4 3) 
            :initial-contents
            ’((3 7 6) 
              (1 9 3)
              (4 5 0)
              (3 2 9))))
(print-matrix a)
(max_matrix a)

