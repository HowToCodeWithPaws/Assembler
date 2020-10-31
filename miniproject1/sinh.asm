format PE console
entry start
include 'win32a.inc'

section '.code' code readable executable
  sinh:
    finit
    fld1
    fstp Qword[delta];слово на 64 бит

    ; заходим в расчет собственно гиперболического синуса
    mov ecx, 0
    sinh_loop:
    inc ecx
    ; домножаем delta на x / i
    fld Qword[x]
    mov [i], ecx
    fidiv dword[i]
    fmul Qword[delta]
    fstp Qword[delta]

    ; пропускаем четные степени i, потому что синус раскладывается
    ; в сумму нечетных степеней
    mov eax, ecx
    and eax, 1
    cmp eax, 0
    je sinh_loop

    ; прибавляем delta к res
    fld Qword[delta]
    fadd Qword[res]
    fstp Qword[res]

    ; сравниваем то, какой процент разница между последними составляет от результата 0%.
    ; в условии 0.001, а не 0, но тогда точность будет на самом деле меньше, чем мы хотим
    ; abs(delta / res) <= 0 (или 0.001 но тогда будет нехорошая точность)
    fldz ;делаем 64-битное слово для fraction
    fld Qword[delta]
    fdiv Qword[res]
    fabs
    fcompp
    fstsw ax
    sahf
    jbe sinh_end

    ; на всякий случай делаем лимит на количество итераций,
    ; чтобы точно избежать бесконечных циклов
    cmp ecx, 69420
    jl sinh_loop
    sinh_end:
    ret

  start:
    ; создаем float64 в scanf, считываем аргумент для шинуса
    push beginStr
    call [printf]
    add esp, 4
    push x
    push scanfFormat
    call [scanf]
    add esp, 8
    cmp eax, 1
    jne start_wrongInput ; проверяем на правильность ввода. Если неправильный - сообщаем и завершаемся

    start_scanfEnd:
    call sinh ; вызываем расчет

    ; printf работает только с float64, а push - нет, выводим результат
    push dword[res+4]
    push dword[res]
    push dword[x+4]
    push dword[x]
    push answerStr
    call [printf]
    add esp, 20

    ; завершаем программу
    start_exit:
    push exitStr
    call [printf]
    call [getch]
    call [exit]

    ; завершаем программу при некорректном вводе
    start_wrongInput:
    push errorStr
    call [printf]
    add esp, 4
    jmp start_exit

section '.data' data readable writable
  scanfFormat: db '%lf',0
  answerStr: db 'sinh(%g) = %.15g',10,0 ; выводим 15 знаков, потому что у даблов точность 15 десятичных знаков
  beginStr: db 'Which number will be our next victim for sinh function? ',0
  errorStr: db 'Wrong input - not a float',10,0
  exitStr: db 'My work here is done',10,0
  i: dd 0
  x: dq 0
  res: dq 0
  delta: dq 0

section '.idata' import code readable
  library msvcrt, 'msvcrt.dll'
  import msvcrt, printf, 'printf', scanf, 'scanf', exit, '_exit', getch, '_getch'
