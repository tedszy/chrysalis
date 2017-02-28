
;;; all-purpose join strings 
(defun join (out list-of-strings delimiter)
  "You can use t for the output stream. And you can use \"~%\" 
   for the delimiter to get items separated by newlines."
  (let ((fmt (format nil "~a~a~a" "~{~a~^" delimiter "~}")))
    (format out fmt list-of-strings)))

(defun join-newline (out list-of-strings)
  (join out list-of-strings "~%"))

(defparameter *width* 49)
(defparameter *columns* 43)
(defparameter *rows* 6)
(defparameter *red* "red")
(defparameter *green* "green!50!blue")
(defparameter *blue* "blue!20!black")

(defparameter *doc-head* 
  (list "\\documentclass[article,11pt,a4paper]{memoir}"
	"\\setlrmarginsandblock{0.15in}{0.15in}{*}"
	"\\setulmarginsandblock{0.75in}{0.75in}{*}"
	"\\fixthelayout"
	"\\usepackage{tikz,fp}"
	"\\usetikzlibrary{calc}"
	"\\usepackage{pgfplots}"
	"\\pgfplotsset{compat=1.12}"
	"\\begin{document}"))

(defparameter *doc-tail*
  (list "\\vfill"
	"\\end{document}"))

(defparameter *tikz-figure-before* 
  (list	"\\begin{figure}"
	"\\begin{tikzpicture}[scale=0.45]"
	(format nil "\\draw[step=1,very thin,black!30](0,0) grid (~a, ~a);" *columns* *rows*))

(defun combination->lattice-steps (n width)
  "Returns a list of 1s and 0s representing steps across a lattice.
   (1 = up, 0 = right.) Width is 49 for the Amir problem."
  (let ((result nil))
    (loop 
       repeat width
       for q = n then (ash q -1)
       do (push (logand q 1) result) 
       finally (return result))))

(defun combination->path (n width)
  "Translates and integer into a sequence of coordinates on a lattice."
  (let ((x 0) (y 0) (path (list '(0 0))))
    (loop 
       for step in (combination->lattice-steps n width)
       if (= step 0) do (incf x)
       else do (incf y)
       do (push (list x y) path)
       finally (return path))))

(defun make-tikz-path (n width)
  "Formats a path (sequence of coordinates) as a TikZ path string."
  (join nil 
	(mapcar #'(lambda (u)
		    (format nil "(~a,~a)" (first u) (second u)))
		(combination->path n width))
	"--"))

(defun tikz-figure-after (caption)
  (list "\\end{tikzpicture}"
	(format nil "\\caption{~a}" caption)
	"\\end{figure}"))

(defun tikz-figure-data (color-path-pairs)
  (loop 
     for (color path) in color-path-pairs
     collect (format nil "\\draw[very thick,~a] ~a;" color path)))

(defun render-amir (filename)
  (let ((amir-set (with-open-file (in filename :direction :input)
		    (loop
		       for line = (read-line in nil)
		       while line
		       collect (parse-integer line :radix 2)))))
    (with-output-to-string (out)
      (join-newline out *doc-head*)
      (join-newline out *tikz-figure-before*)
      (join-newline out (tikz-figure-data
			 (loop 
			    for n in amir-set
			    if (= (mod n 3) 0) collect (list *red* (make-tikz-path n 49))
			    else if (= (mod n 3) 1) collect (list *green* (make-tikz-path n 49))
			    else collect (list *blue* (make-tikz-path n 49)))))
      (join-newline out (tikz-figure-after "Visualization of 241-element Amir set."))
      (join-newline out  *doc-tail*))))

(defun draw-amir-set (filename)
  (with-open-file (out "amir.tex" :direction :output :if-exists :supersede)
    (format out (render-amir filename))))


