format PE console
entry start
include 'win32a.inc'

section '.code' code readable executable
        start:
                push helloStr
                call [printf]
                push n
                push scanfStr
                call [scanf]
                call [getch]
                mov eax, 0
                add eax, dword[n]

        loop1:
                push eax
                push print1Str
                call [printf]
                pop eax
                pop eax
                push eax
                push print2Str
                call [printf]
                pop eax
                pop eax

        loop1end:
                add eax,-1
                push eax
                push print3Str
                call [printf]
                pop eax
                call [getch]
                pop eax
                cmp eax, 0
                jne loop1

                push 0
                call [exit]

section '.data' data readable writeable
        helloStr: db 'How many crows enrolled to PI?',10,0
        scanfStr: db '%d',0
        n: dd 0
        print1Str: db '%d crows began their studies at PI.',10,0
        print2Str: db 'One got an IUP!',10,0
        print3Str: db 'Now there are %d crows!',10,0

section '.idata' import code readable
library msvcrt, 'msvcrt.dll'
import msvcrt, printf, 'printf',getch, '_getch', exit, 'exit', scanf, 'scanf'