format PE console
entry start
include 'win32a.inc'

section '.code' code readable executable
        start:
                push helloStr
                call [printf]

                push n1
                push scanfStr
                call [scanf]

                cmp dword[n1], 1
                je talking
                jmp exiting


talking:
                push qStr
                call [printf]
                push n2
                push scanfStr
                call [scanf]

                cmp dword[n2], 1
                je unsettling
                cmp dword[n2], 2
                je normal
                jmp exiting

unsettling:
               push quoteStr1
               call [printf]
               jmp exiting

normal:         push quoteStr2
                call [printf]
                jmp exiting
exiting:
                push byeStr
                call [printf]
                call [getch]
                push 0
                call [exit]

section '.data' data readable writeable
        helloStr: db 'Hi! Do you want to see a motivational quote? 1 for yes, 2 for no',10,0
        qStr: db 'Do you want it to be unsettling? 1 for yes, 2 for no', 10, 0
        quoteStr1: db 'Do it today. Tomorrow might never come.', 10, 0
        quoteStr2: db 'Push yourself, because no one else is going to do it for you.', 10, 0
        byeStr: db 'see you soon!',10,0
        scanfStr: db '%d',0
        n1: dd 0
        n2: dd 0

section '.idata' import code readable
library msvcrt, 'msvcrt.dll'
import msvcrt, printf, 'printf',getch, '_getch', exit, 'exit', scanf, 'scanf'