(defun print-matrix (matrix &optional (chars 3) stream)
  (let ((*print-right-margin* (+ 6 (* (1+ chars)
                                      (array-dimension matrix 1)))))
    (pprint matrix stream)
    (values)))

(defun copy-array (array &key
                   (element-type (array-element-type array))
                   (fill-pointer (and (array-has-fill-pointer-p array)
                                      (fill-pointer array)))
                   (adjustable (adjustable-array-p array)))
  (let* ((dimensions (array-dimensions array))
         (new-array (make-array dimensions
                                :element-type element-type
                                :adjustable adjustable
                                :fill-pointer fill-pointer)))
    (dotimes (i (array-total-size array))
      (setf (row-major-aref new-array i)
            (row-major-aref array i)))
    new-array))

(defun find_max(a ii jj)
	(let ((max (aref a 0 jj)))
		(loop for i upfrom 0 below (+ 1 ii) do
      		(loop with m = (array-dimension a 1)
      			for j upfrom jj below m do
      				(if (< max (aref a i j))
      				(setf max (aref a i j)))))
    max))

(defun max_matrix (a)
	(let ((b (make-array ((array-dimension a 0) (array-dimension a 1)) : initial-element 0)))
		(loop with n = (array-dimension a 0)
      	for i upfrom 0 below n do
      		(loop with m = (array-dimension a 1)
      		for j upfrom 0 below m do
            	(setf (aref b i j) (find_max a i j))))
    b))

(defvar a (make-array ’(5 4) 
            :initial-contents
            ’((3 4 6 5) 
              (1 3 3 2)
              (4 8 0 2)
              (3 2 9 3)
              (4 6 2 1))))
(print-matrix a)
(print-matrix (max_matrix a))
(print-matrix a)

