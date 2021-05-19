(defgeneric construct (arg1 arg2)
 (:method ((n1 list) (n2 list))
  (+ n1 n2)))