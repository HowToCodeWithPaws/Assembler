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

                push n2
                push scanfStr
                call [scanf]

                mov edx, 0
                add edx, dword[n2]
                mov eax, 0
                add eax, dword[n1]

                mov ecx,0
                add ecx,eax
                add ecx, edx

                push ecx
                push edx
                push eax
                push printStr
                call [printf]
                call [getch]
                push 0
                call [exit]

section '.data' data readable writeable
        helloStr: db 'Print two numbers you want to add on separate lines',10,0
        scanfStr: db '%d',0
        n1: dd 0
        n2: dd 0
        printStr: db '%d + %d = %d',10,0

section '.idata' import code readable
library msvcrt, 'msvcrt.dll'
import msvcrt, printf, 'printf',getch, '_getch', exit, 'exit', scanf, 'scanf'