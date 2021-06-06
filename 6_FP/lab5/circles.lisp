(defclass circle ()
 ((center :initarg :center :reader center)   ; центр - экземпляр cart или polar
  (radius :initarg :radius :reader radius))) ; радиус - число

(defclass polar ()
 ((radius :initarg :radius :accessor radius) 	; длина >=0
  (angle  :initarg :angle  :accessor angle)))	; угол (-π;π]

(defclass cart ()                ; имя класса и надклассы
 ((x :initarg :x :reader cart-x)   ; дескриптор слота x
  (y :initarg :y :reader cart-y))) ; дескриптор слота y

(defmethod cart-x ((p polar))
  (* (radius p) (cos (angle p))))

(defmethod cart-y ((p polar))
  (* (radius p) (sin (angle p))))

(defgeneric to-cart (arg)
 (:documentation "Преобразование аргумента в декартову систему.")
 (:method ((c cart))
  c)
 (:method ((p polar))
  (make-instance 'cart
                 :x (cart-x p)
                 :y (cart-y p))) )

(defun dist (city-1 city-2)
  (let ((x1 (cart-x city-1))
        (y1 (cart-y city-1)) 
        (x2 (cart-x city-2))
        (y2 (cart-y city-2)))
       (if (And x1 y1 x2 y2) (sqrt (+ (* (- x1 x2) (- x1 x2)) (* (- y1 y2) (- y1 y2)))) Nil)))

(defun interposition (circle1 circle2)
	(let ((distance (dist (slot-value circle1 'center) (slot-value circle2 'center)))
		  (sumr (+ (slot-value circle2 'radius) (slot-value circle1 'radius))))
		(if (= distance sumr)
		  (string (intern "касаются"))
		  (if (> distance sumr)
  			  (string (intern "не пересекаются"))
  			  (string (intern "пересекаются"))))))

;;тестирование
(setq c1 (make-instance 'circle
	:radius 1
	:center (make-instance 'cart
                 :x 2
                 :y 3)))
                 
(setq c2 (make-instance 'circle
	:radius 1
	:center (make-instance 'cart
                 :x 4
                 :y 3)))
                 
(interposition c1 c2)

(setq c3 (make-instance 'circle
	:radius 2
	:center (make-instance 'polar
                 :radius 1
                 :angle 1)))
                 
(setq c4 (make-instance 'circle
	:radius 5
	:center (make-instance 'polar
                 :radius 1
                 :angle 1)))
                 
(interposition c3 c4)