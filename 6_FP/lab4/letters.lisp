(defun russian-upper-case-p (char)
  (position char "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"))

(defun russian-char-downcase (char)
  (let ((i (russian-upper-case-p char)))
    (if i 
        (char "абвгдеёжзийклмнопрстуфхцчшщъыьэюя" i)
        (char-downcase char))))    

(defun russian-string-downcase (string)
  ;; Преобразовать и латинские, и русские буквы строки в строчные
  (map 'string #'russian-char-downcase string)

(defun whitespace-char-p (char)
  (member char '(#\Space #\Tab #\Newline)))
  
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

defun collect-word-counts (text)
  ;; Собрать все слова текста и подсчитать число их вхождений
  ;; Value: Хэш-таблица: слово -> счётчик
  (let ((ht (make-hash-table :test #'equal)))
    (dolist (sentence text)
      (dolist (word (word-list sentence))
        ;; Отбросить знаки пунктуации справа от слов и к нижнему регистру
        (let ((string (string-right-trim ",.;:?!" (russian-string-downcase word))))
          (when (< 0 (length string))              ; только непустые слова
            (incf (gethash string ht 0))))))
    ht))
