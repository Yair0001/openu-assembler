mcr hello
    ; this should create an error
    mov HELLO,, HI
    sub HI, HELLO
endmcr

hello

PI: .data 3,1,4
mov PI,r1

; this should create an error
CAKE: .string "-----"-----"
