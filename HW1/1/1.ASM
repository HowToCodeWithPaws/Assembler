format PE console
entry start
include 'win32a.inc'

section '.code' code readable executable
start:
     push helloStr
     call [printf]
     call [getch]
     push replyStr
     call [printf]
     call [getch]
     push 0
     call [exit]

section '.data' data readable writeable
helloStr: db '- Hello there!',10,0
replyStr: db '- General Kenobi',10,0

section '.idata' import code readable
library msvcrt, 'msvcrt.dll'
import msvcrt, printf, 'printf',getch, '_getch', exit, 'exit'