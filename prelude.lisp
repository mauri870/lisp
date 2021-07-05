; Atoms
(def {nil} {})
(def {false} 0)
(def {true} 1)

(def {ok} ())
(def {lambda} \)

(def {PI} 3.14159265358979323846)

; List functions
(fun {unpack f l} {
    eval (join (list f) l)
})

; Pack List for Function
(fun {pack f & xs} {f xs})

; Curried and Uncurried calling
(def {curry} unpack)
(def {uncurry} pack)

; Perform Several things in Sequence
(fun {do & l} {
  if (== l nil)
    {nil}
    {last l}
})

; Open new scope
(fun {let b} {
  ((\ {_} b) ok)
})

(fun {flip f a b} {f b a})
(fun {ghost & xs} {eval xs})
(fun {comp f g x} {f (g x)})

; First, Second, or Third Item in List
(fun {fst l} { eval (head l) })
(fun {snd l} { eval (head (tail l)) })
(fun {trd l} { eval (head (tail (tail l))) })

; Nth item in List
(fun {nth n l} {
  if (== n 0)
    {fst l}
    {nth (- n 1) (tail l)}
})

; Last item in List
(fun {last l} {nth (- (len l) 1) l})

; Take N items
(fun {take n l} {
  if (== n 0)
    {nil}
    {join (head l) (take (- n 1) (tail l))}
})

; Drop N items
(fun {drop n l} {
  if (== n 0)
    {l}
    {drop (- n 1) (tail l)}
})

; Split at N
(fun {split n l} {list (take n l) (drop n l)})

; Element of List
(fun {elem x l} {
  if (== l nil)
    {false}
    {if (== x (fst l)) {true} {elem x (tail l)}}
})

; Apply Function to List
(fun {map f l} {
  if (== l nil)
    {nil}
    {join (list (f (fst l))) (map f (tail l))}
})

; Apply Filter to List
(fun {filter f l} {
  if (== l nil)
    {nil}
    {join (if (f (fst l)) {head l} {nil}) (filter f (tail l))}
})

; Fold Left
(fun {foldl f z l} {
  if (== l nil)
    {z}
    {foldl f (f z (fst l)) (tail l)}
})

; Element on List, based on foldl
; (fun {elem x l} {
;   foldl (\ {acc v} { if (== acc 1) {acc} {== x v} }) 0 l
; })

; Len of list
; We already have a len builtin tho
; (fun {len l} {foldl (\ {acc _} {+ acc 1}) 0 l })

; Sum / product
(fun {sum l} {foldl + 0 l})
(fun {product l} {foldl * 1 l})

; Conditional Functions
(fun {select & cs} {
  if (== cs nil)
    {error "No Selection Found"}
    {if (fst (fst cs)) {snd (fst cs)} {unpack select (tail cs)}}
})

; Default Case
(def {otherwise} true)

(fun {case x & cs} {
  if (== cs nil)
    {error "No Case Found"}
    {if (== x (fst (fst cs))) {snd (fst cs)} {
      unpack case (join (list x) (tail cs))}}
})

; Test

(fun {assert got expected} {
  (== got expected)
})

(fun {test msg got expected} {
  (do
    (show 
      (join
        (if (assert got expected) { "OK!" } { "FAIL!" })
        " "
        msg)))
})
