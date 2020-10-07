; Вариант 10
; Разработать программу, которая вводит одномерный массив A[N],
; формирует из элементов массива A новый массив B по правилам,
; указанным в таблице, и выводит его. Память под массивы может
; выделяться как статически, так и динамически по выбору разработчика.
; Разбить решение задачи на функции следующим образом:
;   1)Ввод и вывод массивов оформить как подпрограммы.
;   2)Выполнение задания по варианту оформить как процедуру
;   3)Организовать вывод как исходного, так и сформированного массивов
; Массив B из элементов A в обратном порядке

format PE console
entry start

include 'win32a.inc'

section '.data' data readable writable

        ;Строки для информации о вводе, для чтения входных данных и их форматирования при выводе
        strArrayALength      db      'Please write the array length: ',0
        strReadElem          db      'Please write the array element a[%d] = ', 0
        strWriteA            db      'Array A: ',0
        strWriteB            db      'Array B: ',0
        strWriteElem         db      '{%d} ',0
        strReadInt           db      '%d',0
        endl                 db      10,0

        ;Строки для вывода сообщения об ошибке и финального сообщения
        strFailInput         db      'Your input is wrong. Start again',10,0
        strWriteFinish       db      'Press anything to exit...', 10, 0

        ;Переменные для итератора, временных объектов, длины массивов, самих массивов
        i                dd ?
        tmp              dd ?
        tmpStack         dd ?
        arrayALength     dd 0
        arrayA           rd 100
        arrayB           rd 100
;**************************************************************************************************
section '.code' code readable executable
start:
        ;Вызов считывания входного массива
        call ReadA

        ;Вызов создания нового массива на основе полученного - изначальный в обратном порядке
        call MakeB

        ;Вывод сообщение о выводе первого массива, очистка мусора
        push strWriteA
        call [printf]
        add esp, 4

        ;Вызов метода, печатающего поэлементно первый массив
        call WriteA

        ;Вывод сообщения о выводе второго массива, очистка мусора
        push strWriteB
        call [printf]
        add esp, 4

        ;Вызов метода, печатающего поэлементно второй массив
        call WriteB

        ;Переход к завершению программы
        jmp Finish
;*************************************************процедура считывания первого массива
ReadA:
        ;Вывод информации о вводе размера, очистка мусора
        push strArrayALength
        call [printf]
        add esp, 4

        ;Считывание значения размера массива, очистка мусора
        push arrayALength
        push strReadInt
        call [scanf]
        add esp, 8

        ;Проверка, что тип ввода соответствует требуемому
        cmp eax, 1
        jne FailInput

        ;Перекладывание значения размера, проверка того, что оно больше нуля.
        ;Если да - создаем массив, если нет - вызываем ошибку ввода
        mov eax, [arrayALength]
        cmp eax, 0
        jg  getArray
        call FailInput

;Создание массива: обнуляем регистр, кладем в другой регистр переменную массива
getArray:
        xor ecx, ecx
        mov ebx, arrayA

;В цикле заполнение массива элементами
getLoop:
        ;Если регистр-счетчик дошел до размера массива, прыжок к концу цикла
        mov [tmp], ebx
        cmp ecx, [arrayALength]
        jge endInput

        ;Вывод сообщения про ввод i-го элемента массива, очистка мусора
        mov [i], ecx
        push ecx
        push strReadElem
        call [printf]
        add esp, 8

        ;Считывание вводимого элемента, очистка мусора
        push ebx
        push strReadInt
        call [scanf]
        add esp, 8

        ;Если введенный элемент неправильного типа - прыжок к ошибке ввода
        cmp eax, 1
        jne FailInput

        ;Прибавление счетчика, переход на новую итерацию
        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getLoop

endInput:
        ret
;*************************************************процедура создания второго массива
MakeB:
        ;Кладем переменную массива в регистр
        mov eax, arrayB

        ;Создание указателей на конец и начало первого массив
        mov edi, arrayA
        mov esi, [arrayALength]
        add esi, -1
        imul esi, 4
        add edi, esi
        mov esi, arrayA
        add esi, -4
getLoop2:
        ;В цикле заполняем новый массив элементами старого в обратном порядке,
        ;прибавляем и вычитаем из регистров - счетчиков, пока они не сравняются
        mov ebx, [edi]
        mov [eax], ebx
        add eax, 4
        add edi, -4
        cmp esi, edi
        jne getLoop2

        ret
;*************************************************процедура вывода первого массива
WriteA:
        ;Перемещаем содержимое регистра, обнуляем другой, кладем в третий ссылку на массив
        mov [tmpStack], esp
        xor ecx, ecx
        mov ebx, arrayA
makeLoop:
        ;В цикле, пока регистр счетчик не сравняется с длиной массива
        mov [tmp], ebx
        cmp ecx, [arrayALength]
        je endOutput
        mov [i], ecx

        ;Вывод элемента на позиции счетчика с форматированием строки
        push dword [ebx]
        push strWriteElem
        call [printf]

        ;Обновление счетчика
        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp makeLoop
endOutput:
        ;В конце вывода возвращаем значение в регистр, делаем перенос строки и чистим мусор
        mov esp, [tmpStack]
        push endl
        call [printf]
        add esp, 4
        ret
;*************************************************процедура вывода второго массива
WriteB:
        ;Идентична предыдущей с той разницей, что теперь даем ссылку на второй массив
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
;*************************************************ошибка ввода
FailInput:
        ;Вывод сообщения об ошибке, очистка мусора, вызов завершения работы
        push strFailInput
        call [printf]
        add esp, 4
        jmp Finish
;*************************************************завершение работы программы
Finish:
        ;Вывод сообщения о завершении работы, вызов getch для ожидания нажатия клавиши,
        ;вызов завершения процесса
        push strWriteFinish
        call [printf]
        add esp, 4
        call [getch]

        push 0
        call [ExitProcess]
;**************************************************************************************************
;Загружаемые ресурсы
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