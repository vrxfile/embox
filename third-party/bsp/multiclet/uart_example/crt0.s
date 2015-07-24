.include "aliases.inc"
.include "R1_pcf.inc"

; Константы системные:
.global PSW.ONIRQS      ; Признак обработки немаскируемых прерываний
.equiv  PSW.ONIRQS,       0x0001
.global INTR.PRGE       ; Немаскируемое программное исключение
.equiv  INTR.PRGE,        0x0008

.alias divsl divsl.crt
.alias divl divl.crt

; Глобальные имена:
.global irq.desc.tbl    ; Таблица векторов прерываний.
.global sizeof.IDT.item ; Размер элемента таблицы векторов прерываний.
.equiv  sizeof.IDT.item,  sizeof.ptr
.global __libc_main     ; Ф-ция инициализации стандартной библиотеки Си.
.global main            ; Главная ф-ция задачи.
.global divsl           ; Деление знаковых чисел.
.global divl            ; Деление беззнаковых чисел.
.global movb            ; Копирование участком памяти байтами.
.global movl            ; Копирование участком памяти словами.
.global movq            ; Копирование участком памяти двойными словами.
;Функции
.global asm_getl_PSW 		; Функция getl #PSW из ассемблера 
.global asm_setl_PSW 		; Функция setl #PSW из ассемблера
.global asm_getl_MSKR 		; Функция getl #MSKR из ассемблера 
.global asm_setl_MSKR 		; Функция setl #MSKR из ассемблера
.global asm_getl_INTR 		; Функция getl #INTR из ассемблера 
.global asm_setl_INTR 		; Функция setl #INTR из ассемблера
.global asm_getl_INTNUM  	; Функция getl #INTNUM из ассемблера 
.global asm_getl_PLLCR 		; Функция getl #PLLCR из ассемблера 
.global asm_setl_PLLCR 		; Функция setl #PLLCR из ассемблера
.global asm_getl_PLLSTR		; Функция getl #PLLSTR из ассемблера 
.global asm_setl_PLLSTR		; Функция setl #PLLSTR из ассемблера
.global asm_getl_PLLTMCR 	; Функция getl #PLLTMCR из ассемблера 
.global asm_setl_PLLTMCR 	; Функция setl #PLLTMCR из ассемблера
.global asm_set_PLL         ; функция настройки PLL

;Пользовательские прерывания
.global  set_irq_vector ; Функция для установки пользовательского прерывания

; Локальные имена:
.local master.isr       ; Первичный обработчик прерываний.
.local tsk.retval       ; Код завершения задачи.

;===============================================================================
; Interrupt Descriptor Table (IDT): Таблица векторов прерываний
;===============================================================================
.data
.align 4

  irq.desc.tbl:       
    .long \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0
	;0, \ ;//ENMI.isr,
	;0 ;//RESERVED.isr,
      ; 0, \ ;//PERE.isr,
      ; 0, \ ;//PPGE.isr,
      ; 0, \;//RESERVED.isr,
      ; 0, \;//RESERVED.isr,
      ; 0, \;//WKUP.isr,
      ; 0, \;//DTC0.isr,
      ; 0, \;//ST0.isr, 
	  ; 0, \;//ST1.isr, 
	  ; 0, \;//ST2.isr,
      ; 0, \;//ST3.isr,
      ; 0, \;//SW0.isr,
	  ; 0, \;//SW1.isr,
      ; 0, \;//SW2.isr,
      ; 0, \;//SW3.isr,
      ; 0, \;//ADC0.isr,
      ; 0, \;//DAC0.isr,
      ; 0, \;//ADC1.isr,
      ; 0, \;//DAC1.isr,
      ; 0, \;//UART0.isr,
	  ; 0, \;//UART1.isr,
	  ; 0, \;//UART2.isr,
	  ; 0, \;//UART3.isr,
      ; 0, \;//I2C0.isr,
	  ; 0, \;//I2C1.isr,
      ; 0, \;//SPI0.isr,
	  ; 0, \;//SPI1.isr,
      ; 0, \;//I2S0.isr,
      ; 0, \;//GPTIM0.isr,
	  ; 0, \;//GPTIM1.isr,
	  ; 0, \;//GPTIM2.isr,
	  ; 0, \;//GPTIM3.isr,
      ; 0, \;//WDT.isr,
      ; 0, \;//RTC.isr,
      ; 0, \;//GPIOA.isr,
	  ; 0, \;//GPIOB.isr,
	  ; 0, \;//GPIOC.isr, 
	  ; 0, \;//GPIOD.isr,
	  ; 0, \;//GPIOE.isr,
	  ; 0, \;//GPIOF.isr,
      ; 0, \;//ETHERNET0.isr,
      ; 0, \;//USB0.isr,
      ; 0, \;//USB0_EPI.isr,
      ; 0, \;//USB0_EPO.isr,
      ; 0, \;//PWM0.isr,
      ; 0 ;//STAT.isr

;===============================================================================
; Interrupt System Timer(IST): Таблица векторов прерываний периферийного таймера
;===============================================================================



/////////////////////////////////////////

;===============================================================================
; Начальный код
;-------------------------------------------------------------------------------
; - инициализирует стеки
; - утанавливает адрес первичного обработчика прерываний
; - разрешает обработку прерываний
; - уходит на функцию __libc_main определенную в стандартной библиотеке
;   которая в свою очередь вызывает ф-цию main программы.
; - записывает возвращаемое значение в переменную tsk.retval
; - инициирует немаскируемое программное прерывание.
;===============================================================================
.data
  tsk.retval: .long               ; Результат выполнения задачи.
  .alias sizeof.tsk.retval sizeof.long
  //psw_reset_val: .long

.text

__libc_main:  
 wdt_off:
     getl 0x00003333
     wrl @1, 0xC00E0004 ; disable watch dog timer
     jmp start;
 complete 

start:

    ; Инициализация стека и указателей стека:
    ;     0           #SP      #BP = sizeof.DM
    ;     |           |        |
    ;     v           v        v
    ; DM |...........|....|....|
    ;                      ^^^^
    ;                 ^^^^ tsk.retval
    ;                 #start.stop
    ; #SP -- адрес возврата из программы;
    ; #BP -- за верхнюю границу DM.
    jmp   main
    getl  sizeof.DM
    getl  0
    getl  start.stop
    subl  @3, sizeof.tsk.retval
    subl  @1, sizeof.ptr
    wrl   @4, @2                  ; __libc_main::retval = 0
    wrl   @4, @2                  ; __libc_main::retaddr = start.stop
    setl  #BP, @7                 ; BP = sizeof (DM)
    setl  #SP, @4                 ; SP = BP - sizeof.tsk.retval - sizeof.ptr
    
    ; Инициализация обработки прерываний:

    setl  #IHOOKADDR, master.isr  
	
	setl #INTR, 0xFFFFFFFF ;обнулим прерывания
	setl #MSKR, 0 ;обнулим маску
	
    getq  #PSW
    or    @1, PSW.ONIRQS
    setq  #PSW, @1

  complete                        ; => __libc_main
  

  start.stop:                     ; <= __libc_main

    ;jmp   start

    ; Копируем возвращаемое значение во статическую переменную.

    rdl   #SP, sizeof.ptr
    wrl   @1, tsk.retval

    ; Освобождаем память в стеке.

    getl  #SP
    addl  @1, sizeof.ptr + sizeof.tsk.retval
    setl  #SP, @1                 ; SP = BP = sizeof.DM

    ; Устанавливаем немаскируемое программное прерывание

    ;setq  #FORCE, INTR.PRGE

  complete

;===============================================================================
; __libc_main
;-------------------------------------------------------------------------------
; Заглушка для ф-ции которая должна предоставляться стандартной библиотекой Си.
; Выполняет инициализацию окружения выполнения программы на Си и вызывает
; фцнкцию main.
;===============================================================================
  .alias __libc_main.ret.value  #SP,sizeof.ptr


 ;===============================================================================
; Interrupt Vector Setting (IVS): Установка веторов прерываний
; Реализация функции, которая устанавливает пользовательский обработчик прерываний
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; void set_irq_vector(int function, int number_irq);
;===============================================================================
 set_irq_vector:
	rdl #SP + 4 ;function name
	rdl #SP + 8 ;irq number
    getl irq.desc.tbl
    mull @2, sizeof.IDT.item
    addl @1, @2
	wrl @5, @1 ;set irq vector
	;rdl #SP
    ;jmp @1
    jmp [#SP]
 complete
 
 
;===============================================================================
; Interrupt Service Routine (ISR): Обработчик прерываний
;===============================================================================
; Нереентерабельная обработка прерываний.

.data

  .local master.isr.retaddr
  master.isr.retaddr: .long   ; Адрес возврата из прерывания.
  test_irq_psw: .long ;Тестим прерывания

.text

  master.isr:

    ; Формируем переход на обработчик прерывания
    ; по адресу из таблицы прерываний.
    ; Стек не трогается т.к. в нем могут передаваться аргументы для
    ; программного прерывания, за исключением того, что добавляем в стек
    ; адрес возврата на окончание первичного обработчика прерывания.
	; Кроме того проверяем задан ли вектор прерывания.
  
    getl  #SP
    getl  master.isr.stop
    getl  #INTNUMR
    getl  #IRETADDR
    getl  irq.desc.tbl
    mull @3, sizeof.IDT.item
    addl @1, @2
    rdl   @1
	je @1, master.isr.stop
	jne @2, @2
    subl  @10, sizeof.ptr
    wrl   @10, @1
    setl  #SP, @2

    ; Сохраняем адрес возврата из прерывания.

    wrl   @10, master.isr.retaddr
    ; Обнуляем прерывание в регистре INTR
    getl 1
    sllq @1, @13
    setq #INTR, @1
  complete

  master.isr.stop:

    ; Возвращаемся из прерывания:

    rdl   master.isr.retaddr
    jmp   @1

    ; Разрешаем прерывания:

    getq  #PSW
    or    @1, PSW.ONIRQS
    setq  #PSW, @1

    getl #SP
    addl @1, sizeof.ptr
    setl #SP, @1

  complete

  STUB.isr:
    rdl   #SP
    jmp   @1
  complete


;===============================================================================
; Целочисленное деление знаковых слов.
; (Использует алгоритм беззнакового деления.)
; Если делитель ноль, то результат не определен.
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
;   union Result {
;     uint64_t value;
;     struct {
;       int32_t Quotient;
;       int32_t Remainder;
;     } fields;
;   }
; Result divsl (int32_t Numerator, int32_t Denominator);
; 
; 1. Изменить знак у делимого если оно отрицательное. Сделать тоже для делителя.
; 2. Преобразовать делимое и делитель в беззнаковое значение.
; 3. Использовать алгоритм беззнакового деления.
; 4. Преобразовать частное и остаток в знаковое представление.
; 5. Изменить знак частного, если знаки делимого и делителя были различные.
; 6. Если делимое отрицательное, изменить знак остатка.
;===============================================================================

.text

  .alias div.Denominator  #BP, +20
  .alias div.Numerator    #BP, +16
  .alias div.Remainder    #BP, +12
  .alias div.Quotient     #BP, +8
  .alias div.RetAddr      #BP, +4
  .alias div.PrevBP       #BP
  .alias div.Index        #BP, -4

  .alias div.arg.Denominator  #SP, -4
  .alias div.arg.Numerator    #SP, -8
  .alias div.arg.Remainder    #SP, -12
  .alias div.arg.Quotient     #SP, -16
  .alias div.arg.RetAddr      #SP, -20

  .alias div.sizeof.args      4*sizeof.long

  .alias div.res.Remainder    #SP, +8
  .alias div.res.Quotient     #SP, +4

  divsl:
  divsl.prologue:

    jmp   divsl.check.numerator

    getl  #SP
    getl  #BP
    subl  @2, sizeof.ptr
    wrl   @2, #SP,-sizeof.ptr
    setl  #SP, @2
    setl  #BP, @3

  complete
  divsl.check.numerator:               ; if (N < 0)

    rdsl   div.Numerator
    js    @1, divsl.neg.num
    jns   @2, divsl.pos.num
    wrl	  @0,0x7FFFC
  
  complete
  divsl.neg.num:                       ; if (D < 0)

    rdsl   div.Denominator
    js    @1, divsl.neg.num.neg.denom
    jns   @2, divsl.neg.num.pos.denom
    wrl	  @0,0x7FFFC

  complete
  divsl.neg.num.neg.denom:             ; divl (-N,-D)

    jmp   divl

    getl  #SP

    rdsl  div.Denominator
    rdsl  div.Numerator
    getl  divsl.neg.num.neg.denom.returns

    insubsl @3, 0
    insubsl @3, 0
    subl  @6, div.sizeof.args + sizeof.ptr

    wrsl  @3, div.arg.Denominator
    wrsl  @3, div.arg.Numerator
    wrl   @6, div.arg.RetAddr

    setl  #SP, @4
  
  complete
  divsl.neg.num.neg.denom.returns:      ; R = -R

    jmp   divsl.epilogue

    getl  #SP
    
    rdl   div.res.Remainder
    rdl   div.res.Quotient

    insubsl @2, 0
    addl  @4, div.sizeof.args + sizeof.ptr

    wrl   @2, div.Remainder
    wrl   @4, div.Quotient

    setl  #SP, @3

  complete
  divsl.neg.num.pos.denom:              ; divl (-N,D)

    jmp   divl

    getl  #SP

    rdsl  div.Denominator
    rdsl  div.Numerator
    getl  divsl.neg.num.pos.denom.returns

    insubsl @2, 0
    subl  @5, div.sizeof.args + sizeof.ptr

    wrsl  @5, div.arg.Denominator
    wrsl  @3, div.arg.Numerator
    wrl   @5, div.arg.RetAddr

    setl  #SP, @4

  complete
  divsl.neg.num.pos.denom.returns:      ; Q = -Q; R = -R;

    jmp   divsl.epilogue

    getl  #SP
    
    rdl   div.res.Remainder
    rdl   div.res.Quotient

    insubsl @2, 0
    insubsl @2, 0
    addl  @5, div.sizeof.args + sizeof.ptr

    wrl   @3, div.Remainder
    wrl   @3, div.Quotient

    setl  #SP, @3

  complete
  divsl.pos.num:                        ; if (D < 0)

    rdsl  div.Denominator
    js    @1, divsl.pos.num.neg.denom
    jns   @2, divsl.pos.num.pos.denom
    wrl	  @0,0x7FFFC

  complete
  divsl.pos.num.neg.denom:              ; divl (N,-D)

    jmp   divl

    getl  #SP

    rdsl  div.Denominator
    rdsl  div.Numerator
    getl  divsl.pos.num.neg.denom.returns

    insubsl @3, 0
    subl  @5, div.sizeof.args + sizeof.ptr

    wrsl  @2, div.arg.Denominator
    wrsl  @5, div.arg.Numerator
    wrl   @5, div.arg.RetAddr

    setl  #SP, @4

  complete
  divsl.pos.num.neg.denom.returns:      ; Q = -Q

    jmp   divsl.epilogue

    getl  #SP
    
    rdl   div.res.Remainder
    rdl   div.res.Quotient

    insubsl @1, 0
    addl  @4, div.sizeof.args + sizeof.ptr

    wrl   @4, div.Remainder
    wrl   @3, div.Quotient

    setl  #SP, @3

  complete
  divsl.pos.num.pos.denom:               ; divl (N,D)

    jmp   divl

    getl  #SP

    rdsl  div.Denominator
    rdsl  div.Numerator
    getl  divsl.pos.num.pos.denom.returns

    subl  @4, div.sizeof.args + sizeof.ptr

    wrsl  @4, div.arg.Denominator
    wrsl  @4, div.arg.Numerator
    wrl   @4, div.arg.RetAddr

    setl  #SP, @4

  complete
  divsl.pos.num.pos.denom.returns:

    jmp   divsl.epilogue

    getl  #SP
    
    rdl   div.res.Remainder
    rdl   div.res.Quotient

    addl  @3, div.sizeof.args + sizeof.ptr

    wrl   @3, div.Remainder
    wrl   @3, div.Quotient

    setl  #SP, @3

  complete
  divsl.epilogue:

    ; Строим возврат.

    rdl   div.RetAddr
    jmp   @1

    getl  #BP
    rdl   div.PrevBP
    setl  #BP, @1
    addl  @3, sizeof.ptr
    setl  #SP, @1
    wrl	  @0,0x7FFFC

  complete

;==============================================================================
; Целочисленное деление беззнаковых слов.
; Если делитель ноль, то результат не определен.
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
;   union Result {
;     uint64_t value;
;     struct {
;       uint32_t Quotient;
;       uint32_t Remainder;
;     } fields;
;   }
; Result divl (uint31_t Numerator, uint31_t Denominator);
;==============================================================================

  divl:
  divl.prologue:

    jmp   divl.arg.check

    getl  #SP
    getl  #BP
    subl  @2, sizeof.ptr
    wrl   @2, #SP,-sizeof.ptr
    setl  #SP, @2
    setl  #BP, @3

  complete
  divl.arg.check:                       ; Делимое должно быть больше делителя.

    rdl   div.Numerator
    rdl   div.Denominator

/*  Из-за аппаратной ошибки выставления флага переноса/заёма
*   этот код для некоторых значений работает неправильно
*    subl  @2, @1                        ; if ( N < D )
*    jb    @1, divl.simple.result
*    jae   @2, divl.prepare
*/

    maxl  @1, @2
    subl  @3, @1                        ; if ( N < D )
    jne    @1, divl.simple.result
    je   @2, divl.prepare
    wrl	  @0,0x7FFFC

  complete

  divl.simple.result:                   ; Если делимое все таки меньше делителя, то оно и есть остаток.

    jmp   divl.epilogue
    getl  0
    rdl   div.Numerator
    wrl   @2, div.Quotient
    wrl   @2, div.Remainder
  
  complete
  divl.prepare:                         ; Выравниваем делитель таким образом, что знакоместо старшего бита
                                        ; делителя соответствует знакоместу старшего бита делимого.
    jmp   divl.check.remainder
    rdl   div.Numerator
    rdl   div.Denominator
                                        ; За компанию, превращаем делимое в остаток,
                                        ; чем оно и будет являться на протяжении выполнения алгоритма.
    wrl   @2, div.Remainder
    norml @3                            ; Позиция старшего значащего бита делимого
    norml @3                            ; Позиция старшего значащего бита делителя,
                                        ; должна быть <= чем у делимого, 
                                        ; т.к. исходим из условия, что делимое > делителя.
    subl  @1, @2                        ; Расстояние на которое нужно сдвинуть делитель
                                        ; чтобы старшие значащие биты делимого и делителя совпадали.
    slll  @5, @1                        ; Сдвигаем делитель таки образом,
                                        ; чтобы старшие значащие биты делимого и делителя
                                        ; совпадали по знакоместу.
    wrl   @1, div.Denominator
                                        ; Устанавливаем бит базы-индекса в позицию старшего значащего бита делителя.
    getl  1                             ; Выставляем бит базы-индекса...
    slll  @1, @4                        ; ... и сдвигаем ее так же как и делитель.
    wrl   @1, div.Index
                                        ; Обнуляем начальное значение частного.
    getl  0
    wrl   @1, div.Quotient

  complete
  divl.check.remainder:                 ; Проверяем условие подбора очередной степени частного.
                                        ; Если в данной позиции (степени) делителя остаток больше делителя, то
                                        ; значит можно вычесть из остатка данную степень делителя и, соответсвенно,
                                        ; скорректировать частное.
                                        ; Иначе -- понижаем степень делителя.
    rdl   div.Remainder
    rdl   div.Denominator

/*  Из-за аппаратной ошибки выставления флага переноса/заёма
*   этот код для некоторых значений работает неправильно
*    subl  @2, @1                        ; if R < D
*    jb    @1, divl.next.order
*    jae   @2, divl.calculate.remainder
*/

    maxl @1, @2
    subl @3, @1
    jne    @1, divl.next.order
    je   @2, divl.calculate.remainder
    wrl	  @0,0x7FFFC

  complete
  divl.calculate.remainder:             ; Вычисляем остаток и частное.

    jmp   divl.next.order
    rdl   div.Remainder
    rdl   div.Denominator
    subl  @2, @1                        ; Вычисляем остаток:     (R-D').
    wrl   @1, div.Remainder             ; Сохраняем остаток: R <- (R-D').
    
    rdl   div.Quotient                  ; Частное.
    rdl   div.Index
    addl  @2, @1                        ; Складываем частное с базой-индексом: Q'+I
    wrl   @1, div.Quotient              ; Запоминаем частное: Q'<=(Q'+I)
  
  complete
  divl.next.order:                      ; Сдвигаемся к следующему разряду.

    rdl   div.Denominator
    rdl   div.Index
    slrl  @2, 1                         ;     (D'>>1)
    slrl  @2, 1                         ;     (I >>1)
    wrl   @2, div.Denominator           ; D'<=(D'>>1)
    wrl   @2, div.Index                 ; I <=(I >>1)
                                        ; Проверяем что все разряды обработаны.
                                        ; if (I == 0) // База-индекс в младщем разряде.
    je    @3, divl.epilogue             ; then Алгоритм завершен
    jne   @4, divl.check.remainder
    wrl	  @0,0x7FFFC
  
  complete
  divl.epilogue:

    ; Строим возврат.

    rdl   div.RetAddr
    jmp   @1

    getl  #BP
    rdl   div.PrevBP
    setl  #BP, @1
    addl  @3, sizeof.ptr
    setl  #SP, @1
    wrl	  @0,0x7FFFC

  complete


 
  
;===============================================================================
; Реализация функции, которая выполняет команду setl в регистр #PSW аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; void asm_setl_PSW(int ARG1);
;
;===============================================================================

asm_setl_PSW:
    rdl #SP
	rdl #SP + 4 ;значение пользователя для PSW
	setl #48, @1
	jmp @3
complete 

;===============================================================================
; Реализация функции, которая выполняет команду getl в регистр #PSW аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; int asm_getl_PSW();
;
;===============================================================================

asm_getl_PSW:
    getl #48
	wrl @1, #SP + 4 ;текущее значение PSW
    rdl #SP
	jmp @1
complete

;===============================================================================
; Реализация функции, которая выполняет команду setl в регистр #MSKR аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; void asm_setl_MSKR(int ARG1);
;
;===============================================================================

asm_setl_MSKR:
    rdl #SP
	rdl #SP + 4 ;значение пользователя для PSW
	setl #50, @1
	jmp @3
complete 

;===============================================================================
; Реализация функции, которая выполняет команду getl в регистр #MSKR аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; int asm_getl_MSKR();
;
;===============================================================================

asm_getl_MSKR:
    getl #50
	wrl @1, #SP + 4 ;текущее значение PSW
    rdl #SP
	jmp @1
complete 

;===============================================================================
; Реализация функции, которая выполняет команду setl в регистр #INTR аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; void asm_setl_INTR(int ARG1);
;
;===============================================================================

asm_setl_INTR:
    rdl #SP
	rdl #SP + 4 ;значение пользователя для PSW
	setl #49, @1
	jmp @3
complete 

;===============================================================================
; Реализация функции, которая выполняет команду getl в регистр #INTR аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; int asm_getl_INTR();
;
;===============================================================================

asm_getl_INTR:
    getl #49
	wrl @1, #SP + 4 ;текущее значение PSW
    rdl #SP
	jmp @1
complete 

;===============================================================================
; Реализация функции, которая выполняет команду getl в регистр #INTNUM аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; int asm_getl_INTNUM();
;
;===============================================================================

asm_getl_INTNUM:
    getl #56
	wrl @1, #SP + 4 ;текущее значение PSW
    rdl #SP
	jmp @1
complete 

;===============================================================================
; Реализация функции, которая выполняет команду setl в регистр #PLLCR аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; void asm_setl_PLLCR(int ARG1);
;
;===============================================================================

asm_setl_PLLCR:
    rdl #SP
	rdl #SP + 4 ;значение пользователя для PLLCR
	setl #16, @1
	jmp @3
complete 

;===============================================================================
; Реализация функции, которая выполняет команду getl в регистр #PLLCR аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; int asm_getl_PLLCR();
;
;===============================================================================

asm_getl_PLLCR:
    getl #16
	wrl @1, #SP + 4 ;текущее значение PLLCR
    rdl #SP
	jmp @1
complete 

;===============================================================================
; Реализация функции, которая выполняет команду setl в регистр #PLLSTR аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; void asm_setl_PLLSTR(int ARG1);
;
;===============================================================================

asm_setl_PLLSTR:
    rdl #SP
	rdl #SP + 4 ;значение пользователя для PLLSTR
	setl #17, @1
	jmp @3
complete 

;===============================================================================
; Реализация функции, которая выполняет команду getl в регистр #PLLSTR аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; int asm_getl_PLLSTR();
;
;===============================================================================

asm_getl_PLLSTR:
    getl #17
	wrl @1, #SP + 4 ;текущее значение PLLSTR
    rdl #SP
	jmp @1
complete  

;===============================================================================
; Реализация функции, которая выполняет команду setl в регистр #PLLTMCR аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; void asm_setl_PLLTMCR(int ARG1);
;
;===============================================================================

asm_setl_PLLTMCR:
    rdl #SP
	rdl #SP + 4 ;значение пользователя для PLLTMCR
	setl #18, @1
	jmp @3
complete 

;===============================================================================
; Реализация функции, которая выполняет команду getl в регистр #PLLTMCR аналогично ассемблеру 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; int asm_getl_PLLTMCR();
;
;===============================================================================

asm_getl_PLLTMCR:
    getl #18
	wrl @1, #SP + 4 ;текущее значение PLLTMCR
    rdl #SP
	jmp @1
complete  

;===============================================================================
; Реализация функции, которая выполняет настройку PLL 
; Интерфейс вызова ф-ции соответствует следующему объявлению в синтаксисе Си:
;
; int asm_set_PLL(unsigned char RCNT, unsigned char NCNT, unsigned char MXVCO, unsigned char Enable);
;
;===============================================================================

asm_set_PLL:
    rdl #SP + 8 ;RCNT
    rdl #SP + 12 ;NCNT
    rdl #SP + 20 ;EN
    andl @3, 0x7
    andl @3, 0x7F
    andl @3, 0x1
    slll @3, 12
    slll @2, 15
    addl @4, @2
    addl @1, @2
    setl #16, @1
    jmp asm_wait_pll_lock 
complete   

asm_wait_pll_lock:
    getl #17 
    getl 1 
    andl @1, @2 
    je @1, asm_wait_pll_lock 
    jne @2, asm_pll_set 
complete

asm_pll_set:
    rdl #SP + 8 ;RCNT
    rdl #SP + 12 ;NCNT
    rdl #SP + 16 ;MXVCO
    rdl #SP + 20 ;EN
    andl @4, 0x7
    andl @4, 0x7F
    andl @4, 0xF
    andl @4, 0x1
    slll @4, 12
    slll @3, 8
    slll @3, 15
    addl @6, @3
    addl @3, @2
    addl @1, @2
    setl #16, @1
    rdl #SP
	jmp @1
complete
  
.end
