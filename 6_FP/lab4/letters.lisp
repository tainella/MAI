(defun russian-upper-case-p (char)
  (position char "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"))

(defun russian-char-downcase (char)
  (let ((i (russian-upper-case-p char)))
    (if i 
        (char "абвгдеёжзийклмнопрстуфхцчшщъыьэюя" i)
        (char-downcase char))))    

(defun russian-string-downcase (string)
  ;; Преобразовать и латинские, и русские буквы строки в строчные
  (map 'string #'russian-char-downcase string))

;;(defun whitespace-char-p (char)
  ;;(member char '(#\Space #\Tab #\Newline)))
  
(defun word-list (string)
  ;; Разбить строки на слова, разделённые знаками whitespace
  ;; A la (split-seq-if #'whitespace-char-p string)
  (loop with len = (length string)
        for left = 0 then (1+ right)
        for right = (or (position-if #'whitespace-char-p string
                                     :start left)
                        len)
        unless (= right left)	; исключить пустые слова
          collect (subseq string left right)
        while (< right len)))

(defun is_two-char-word (word)
	(let ((answer 0))
		(loop with len = (length word)
			for j upfrom 0 below len do
				(let ((a (char word j)))
					(if (find a word :start (+ 1 j) :end len)
					(setf answer 1))))
	answer))

(defun remove-two-char-words(text)
	(dolist (sentence text)
      collect (dolist (word (word-list sentence))
      		(let ((predloz ()))
      		(let ((string (string-right-trim ",.;:?!" (russian-string-downcase word))))
      				(when (< 0 (length string))
      					(when (= 0 (is_two-char-word (russian-string-downcase string)))
      						(push string predloz))))))))

;;(remove-two-char-words '("Оно скрылось за деревьями." "Мы прошли, не заметив его."))
(remove-two-char-words '("Ono skrulos za derevyami." "Mu proshli, ne zametiv ego."))

