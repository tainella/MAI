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

(defun same-variable-p (v1 v2)
  ;; Переменные v1 и v2 - совпадающие символы
  (and (symbolp v1) (symbolp v2) (eq v1 v2)))

(defun add-terms (tl1 tl2)
  ;; Объединить списки термов tl1 и tl2,
  ;; упорядочивая по убыванию степеней
  (cond ((null tl1) tl2)
        ((null tl2) tl1)
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
                              (rest tl2)))))))))

(defun glitch (list1 list2)
	(cond ((null list1) null)
        ((null list2) null)
        (t 
        	(let ((n (min (length list1) (length list2))))
        	(loop for i upfrom 0 below n
        		collect (list (make-term :order 1
        								  :coeff 1)
        					   (make-term :order 0
        								  :coeff (* (nth i list1) -1))))))))
(defun make-d-poly (list n)
	(make-instance 'polynom 
          :var 'x
          :terms (loop for i upfrom 0 below n
        			collect (make-term :order 0
        							   :coeff (nth i list)))))
          
(defmethod mul2 ((p1 polynom) (p2 polynom))
  (if (same-variable-p (var p1) (var p2))
      (make-instance 'polynom
                     :var (var p1)
                     :terms (mul-terms (terms p1)
                                       (terms p2)))
      (error "Многочлены от разных переменных: ~s и ~s"
             p1 p2)))

(defun mul-terms (tl1 tl2)
  ;; Скрестить каждый терм из списка tl1 с каждым из списка tl2
  (if (null tl1)
      ()
      (add-terms (mul-term-by-all-terms (first tl1) tl2)
                 (mul-terms (rest tl1) tl2))))

(defun mul-term-by-all-terms (t1 term-list)
  ;; Скрестить терм t1 с каждым из списка term-list
  (if (null term-list)
      ()
      (let ((t2 (first term-list)))
        ;; Коэффициенты перемножаем, а степени суммируем
        (adjoin-term (make-term :coeff (mul2 (coeff t1) (coeff t2))
                                :order (+ (order t1) (order t2)))
                     (mul-term-by-all-terms t1 (rest term-list))))))

(defun construct_polynom (list1 list2)
  (mul2 (make-d-poly list2 (min (length list1) (length list2))) 
  		(make-instance 'polynom 
          					:var 'x
          					:terms (glitch list1 list2))))

(construct_polynom '(1 2 3 4) '(7 7 7 7 7))
;;(glitch '(1 2 3 4) '(7 7 7 7 7))
;;(make-d-poly '(7 7 7 7 7) 4)
