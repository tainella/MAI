(defclass polynom ()
 ((var-symbol :initarg :var :reader var)
  ;; Разреженный список термов в порядке убывания степени
  (term-list :initarg :terms :reader terms)))

(defun make-term (&key order coeff)
    (list order coeff))

(defun order (term) (first term))
(defun coeff (term) (second term))

(defmethod zerop1 ((p polynom))
  (null (terms p)))

(defmethod minusp1 ((p polynom))
   nil)

(defgeneric zerop1 (arg)
 (:method ((n number))   ; (= n 0)
  (zerop n)))

(defgeneric minusp1 (arg)
 (:method ((n number))   ; (< n 0)
  (minusp n)))

(defmethod print-object ((p polynom) stream)
  (format stream "[МЧ (~s) ~:{~:[~:[+~;-~]~d~[~2*~;~s~*~:;~s^~d~]~;~]~}]"
          (var p)
          (mapcar (lambda (term)
                    (list (zerop1 (coeff term))
                          (minusp1 (coeff term))
                          (if (minusp1 (coeff term))
                              (abs (coeff term))
                              (coeff term))
                          (order term)
                          (var p)
                          (order term)))
                  (terms p))))

(defun glitch (list1 list2 i)
	(cond ((null list1) null)
        ((null list2) null)
        (t 
          (make-instance 'polynom
                        :var (nth i list2)
                        :terms (braces list1 i)))))

(defun braces(list1 i)
	(let ((br (minus (* -1 (nth 0 list1)))))
	(loop for j upfrom 1 below i do
		(setq br (make-instance 'polynom
               	  :var br
               	  :terms (list (make-term 
               	  				:order 1
                                :coeff (minus (* -1 (nth j list1))))))))
    (list (make-term 
            :order 1
          	:coeff br))))
					  			 					
(defun minus(cof) ;;works
	(make-instance 'polynom
               :var 'x
               :terms (list (make-term :order 1
                                       :coeff 1)
                            (make-term :order 0
                                       :coeff cof))))

(defun pair(cof1 cof2) ;;works
	(make-instance 'polynom
                   :var (minus cof2)
                   :terms (list (make-term 
                   				:order 1
          					 	:coeff (minus cof1)))))

(defun my_polynom(list1 list2)
	(cond ((null list2) nil)
		((null list1) 
			(make-instance 'polynom
               				:var 'z
               				:terms (list (make-term 
                   						  :order 0
          					 			  :coeff (nth 0 list2)))))
        (t
	(let ((n (min (length list1) (1- (length list2)))))
		(make-instance 'polynom
               :var 'z
               :terms (nconc (list 
               				(make-term 
                   					:order 0
                                    :coeff(nth 0 list2)))
               			(loop for j upfrom 1 below (1+ n)
               				collect (make-term :order 0
                                       	   	   :coeff (glitch list1 list2 j)))))))))

(my_polynom '(1 2 3 4) '(6 7 8 9 10 11))
(my_polynom '(1 2 3 4) '())
(my_polynom '() '(1 2 3 4))
(my_polynom '(6 7 8 9 10 11) '(1 2 3 4))