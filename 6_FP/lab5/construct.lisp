(defclass polynom ()
 ((var-symbol :initarg :var :reader var)
  ;; Разреженный список термов в порядке убывания степени
  (term-list :initarg :terms :reader terms)))

(defun make-term (&key order coeff)
    (list order coeff))

(defun order (term) (first term))
(defun coeff (term) (second term))

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

(defun adjoin-term (term term-list)
  ;; Добавить term к списку term-list
  (if (zerop1 (coeff term))   ; если коэффициент нулевой,
      term-list               ; то отбрасываем терм,
      (cons term term-list))) ; иначе накапливаем



(defun glitch (list1 list2)
	(cond ((null list1) null)
        ((null list2) null)
        (let ((d (min (len list1) (len list2))))
        	(t
         	(let ((t1 (first tl1))
               	(t2 (first tl2)))
           	(cond ((> (order t1) (order t2))
                  	(adjoin-term t1
                               (add-terms (rest tl1) tl2)))
                	 ((< (order t1) (order t2))
                  	(adjoin-term t2
                               (add-terms tl1 (rest tl2))))
                 	(t
                  	;; степени совпадают - суммируем коэффициенты
                  	(adjoin-term  
                   	(make-term :coeff (add2 (coeff t1) (coeff t2))
                              	:order (order t1))
                   	(add-terms (rest tl1)
                              	(rest tl2))))))))))

(defun construct_polynom (list1 list2)
  ((make-instance 'polynom 
          :var 'x
          :terms (glitch list1 list2)))))

()