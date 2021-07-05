(show "Prelude Test Cases")

; atoms
(test "atoms nil" nil {})
(test "atoms false" false 0)
(test "atoms true" true 1)
(test "atoms ok" ok ())

; lambda
(test "lambda" ((lambda {x} {^ x 2}) 4) 16)

; join
(test "join string with string" (join "abc" "123") "abc123")

; len
(test "len of string" (len "abc") 3)
(test "len of empty string" (len "") 0)
(test "len of list" (len {1 2 3 4}) 4)
(test "len of empty list" (len {}) 0)

; head
(test "head of string" (head "abcd") "a")
(test "head of empty string" (head "") "")
(test "head of list" (head {1 2 3 4}) {1})
(test "head of empty list" (head {}) {})

; tail
(test "tail of string" (tail "abcd") "bcd")
(test "tail of empty string" (tail "") "")
(test "tail of list" (tail {1 2 3 4}) {2 3 4})
(test "tail of empty list" (tail {}) {})

; join
(test "join lists" (join {1 2} {3 4}) {1 2 3 4})
(test "join empty lists" (join {} {1 2}) {1 2})
(test "join strings" (join "ab" "cd") "abcd")
(test "join empty strings" (join "" "") "")
(test "join more than two strings" (join "a" "b" "c" "d") "abcd")
(test "join more than two lists" (join "" "") "")

; sqrt
(test "sqrt of number" (sqrt 3.14) 1.772004514666935)

(show "End of Prelude Test Cases")