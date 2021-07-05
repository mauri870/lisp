; join
(test "join string with string" (join "abc" "123") "abc123")

; len
(test "len of string" (len "abc") 3)
(test "len of empty string" (len "") 0)
(test "len of list" (len {1 2 3 4}) 4)
(test "len of empty list" (len {}) 0)