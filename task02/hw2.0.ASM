; ������� 10
; ����������� ���������, ������� ������ ���������� ������ A[N],
; ��������� �� ��������� ������� A ����� ������ B �� ��������,
; ��������� � �������, � ������� ���. ������ ��� ������� �����
; ���������� ��� ����������, ��� � ����������� �� ������ ������������.
; ������� ������� ������ �� ������� ��������� �������:
;   1)���� � ����� �������� �������� ��� ������������.
;   2)���������� ������� �� �������� �������� ��� ���������
;   3)������������ ����� ��� ���������, ��� � ��������������� ��������
; ������ B �� ��������� A � �������� �������

format PE console
entry start

include 'win32a.inc'

section '.data' data readable writable

        ;������ ��� ���������� � �����, ��� ������ ������� ������ � �� �������������� ��� ������
        strArrayALength      db      'Please write the array length: ',0
        strReadElem          db      'Please write the array element a[%d] = ', 0
        strWriteA            db      'Array A: ',0
        strWriteB            db      'Array B: ',0
        strWriteElem         db      '{%d} ',0
        strReadInt           db      '%d',0
        endl                 db      10,0

        ;������ ��� ������ ��������� �� ������ � ���������� ���������
        strFailInput         db      'Your input is wrong. Start again',10,0
        strWriteFinish       db      'Press anything to exit...', 10, 0

        ;���������� ��� ���������, ��������� ��������, ����� ��������, ����� ��������
        i                dd ?
        tmp              dd ?
        tmpStack         dd ?
        arrayALength     dd 0
        arrayA           rd 100
        arrayB           rd 100
;**************************************************************************************************
section '.code' code readable executable
start:
        ;����� ���������� �������� �������
        call ReadA

        ;����� �������� ������ ������� �� ������ ����������� - ����������� � �������� �������
        call MakeB

        ;����� ��������� � ������ ������� �������, ������� ������
        push strWriteA
        call [printf]
        add esp, 4

        ;����� ������, ����������� ����������� ������ ������
        call WriteA

        ;����� ��������� � ������ ������� �������, ������� ������
        push strWriteB
        call [printf]
        add esp, 4

        ;����� ������, ����������� ����������� ������ ������
        call WriteB

        ;������� � ���������� ���������
        jmp Finish
;*************************************************��������� ���������� ������� �������
ReadA:
        ;����� ���������� � ����� �������, ������� ������
        push strArrayALength
        call [printf]
        add esp, 4

        ;���������� �������� ������� �������, ������� ������
        push arrayALength
        push strReadInt
        call [scanf]
        add esp, 8

        ;��������, ��� ��� ����� ������������� ����������
        cmp eax, 1
        jne FailInput

        ;�������������� �������� �������, �������� ����, ��� ��� ������ ����.
        ;���� �� - ������� ������, ���� ��� - �������� ������ �����
        mov eax, [arrayALength]
        cmp eax, 0
        jg  getArray
        call FailInput

;�������� �������: �������� �������, ������ � ������ ������� ���������� �������
getArray:
        xor ecx, ecx
        mov ebx, arrayA

;� ����� ���������� ������� ����������
getLoop:
        ;���� �������-������� ����� �� ������� �������, ������ � ����� �����
        mov [tmp], ebx
        cmp ecx, [arrayALength]
        jge endInput

        ;����� ��������� ��� ���� i-�� �������� �������, ������� ������
        mov [i], ecx
        push ecx
        push strReadElem
        call [printf]
        add esp, 8

        ;���������� ��������� ��������, ������� ������
        push ebx
        push strReadInt
        call [scanf]
        add esp, 8

        ;���� ��������� ������� ������������� ���� - ������ � ������ �����
        cmp eax, 1
        jne FailInput

        ;����������� ��������, ������� �� ����� ��������
        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getLoop

endInput:
        ret
;*************************************************��������� �������� ������� �������
MakeB:
        ;������ ���������� ������� � �������
        mov eax, arrayB

        ;�������� ���������� �� ����� � ������ ������� ������
        mov edi, arrayA
        mov esi, [arrayALength]
        add esi, -1
        imul esi, 4
        add edi, esi
        mov esi, arrayA
        add esi, -4
getLoop2:
        ;� ����� ��������� ����� ������ ���������� ������� � �������� �������,
        ;���������� � �������� �� ��������� - ���������, ���� ��� �� ����������
        mov ebx, [edi]
        mov [eax], ebx
        add eax, 4
        add edi, -4
        cmp esi, edi
        jne getLoop2

        ret
;*************************************************��������� ������ ������� �������
WriteA:
        ;���������� ���������� ��������, �������� ������, ������ � ������ ������ �� ������
        mov [tmpStack], esp
        xor ecx, ecx
        mov ebx, arrayA
makeLoop:
        ;� �����, ���� ������� ������� �� ���������� � ������ �������
        mov [tmp], ebx
        cmp ecx, [arrayALength]
        je endOutput
        mov [i], ecx

        ;����� �������� �� ������� �������� � ��������������� ������
        push dword [ebx]
        push strWriteElem
        call [printf]

        ;���������� ��������
        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp makeLoop
endOutput:
        ;� ����� ������ ���������� �������� � �������, ������ ������� ������ � ������ �����
        mov esp, [tmpStack]
        push endl
        call [printf]
        add esp, 4
        ret
;*************************************************��������� ������ ������� �������
WriteB:
        ;��������� ���������� � ��� ��������, ��� ������ ���� ������ �� ������ ������
        mov [tmpStack], esp
        xor ecx, ecx
        mov ebx, arrayB
makeLoop2:
        mov [tmp], ebx
        cmp ecx, [arrayALength]
        je endOutput2
        mov [i], ecx

        push dword [ebx]
        push strWriteElem
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp makeLoop2
endOutput2:
        mov esp, [tmpStack]
        push endl
        call [printf]
        add esp, 4
        ret
;*************************************************������ �����
FailInput:
        ;����� ��������� �� ������, ������� ������, ����� ���������� ������
        push strFailInput
        call [printf]
        add esp, 4
        jmp Finish
;*************************************************���������� ������ ���������
Finish:
        ;����� ��������� � ���������� ������, ����� getch ��� �������� ������� �������,
        ;����� ���������� ��������
        push strWriteFinish
        call [printf]
        add esp, 4
        call [getch]

        push 0
        call [ExitProcess]
;**************************************************************************************************
;����������� �������
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess'
  include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch'