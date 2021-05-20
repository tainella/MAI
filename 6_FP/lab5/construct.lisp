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

(defun adjoin-term (term term-list)
  ;; Добавить term к списку term-list
  (if (zerop1 (coeff term))   ; если коэффициент нулевой,
      term-list               ; то отбрасываем терм,
      (cons term term-list))) ; иначе накапливаем

(defun same-variable-p (v1 v2)
  ;; Переменные v1 и v2 - совпадающие символы
  (and (symbolp v1) (symbolp v2) (eq v1 v2)))

(defun glitch (list1 list2 i)
	(cond ((null list1) null)
        ((null list2) null)
        (t 
          (make-instance 'polynom
                        :var 'y
                        :terms
                        (nconc (list (make-term :order 0
          						   		 	    :coeff (nth i list2))
          					  (loop for j upfrom 0 below i
        						collect (make-term :order 0
          						   		 		   :coeff (razn list1 j)))))))))
					  			 					
(defun minus(cof) ;;works
	(make-instance 'polynom
               :var 'x
               :terms (list (make-term :order 1
                                       :coeff 1)
                            (make-term :order 0
                                       :coeff cof))))

(defun pair(cof1 cof2) ;;works
	(make-instance 'polynom
                   :var 'y
                   :terms (list (make-term 
                   				:order 0
          					 	:coeff (minus cof1)))))

;;(glitch '(1 2 3 4) '(7 7 7 7 7) 2)

;;(razn '(1 2 3 4) 4)


;;(pair 1 2)


;; :coeff (list((minus cof1) (minus cof2)))))))

          					 	