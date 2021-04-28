(defun interleave (list1 list2) 
 (if (null list2)
      list1
      (if (null list1)
      	   list2
      	   (cons (first list1) (interleave list2 (rest list1))))))

(interleave '(a (b c) e f g) '(1 2 (3 4)))
(interleave '(a a a) '())