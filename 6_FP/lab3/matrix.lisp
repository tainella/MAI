(defun print-matrix (matrix &optional (chars 3) stream)
  (let ((*print-right-margin* (+ 6 (* (1+ chars)
                                      (array-dimension matrix 1)))))
    (pprint matrix stream)
    (values)))

(defun find_max(a ii jj)
	(let max (aref a 0 0))
	(loop for i upfrom 0 below ii do
      (loop for j upfrom 0 below jj do
      		(if (< max (aref a i j))
      			setf max (aref a i j)))))

(defun max_matrix (a) 
	(let b make-array ’((array-dimensions a) (array-dimensions a 0)))
	(loop with n = (array-dimension a)
      for i upfrom 0 below n do
      (loop with m = (array-dimension a 0)
      	for j upfrom 0 below m do
            (psetf (aref b i j) (find_max a j i)))))

(defvar a (make-array ’(4 3) 
            :initial-contents
            ’((6 7 8) 
              (1 2 3)
              (4 5 0)
              (3 2 9))))
(print-matrix a)
(max_matrix a)
