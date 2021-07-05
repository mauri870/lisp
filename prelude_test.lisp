(show "Prelude Test Cases")

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