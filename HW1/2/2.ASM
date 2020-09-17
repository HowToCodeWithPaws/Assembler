format PE console
entry start
include 'win32a.inc'

section '.code' code readable executable
        start:
                push helloStr
                call [printf]
                call [getch]
                add esp, 4

                push n
                push scanfStr
                call [scanf]
                add esp, 8

                push dword[n]
                push printStr
                call [printf]
                 call [getch]
                add esp, 8

                push 0
                call [exit]

section '.data' data readable writeable
        helloStr: db 'Hi! Tell me your number!',10,0
        scanfStr: db '%d',0
        n: dd 0
        printStr: db 'Behold, magic! Your number is %d!',10,0

section '.idata' import code readable
library msvcrt, 'msvcrt.dll'
import msvcrt, printf, 'printf',getch, '_getch', exit, 'exit', scanf, 'scanf'