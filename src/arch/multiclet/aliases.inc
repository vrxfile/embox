/*
 * Алиасы
*/

.ifndef .LALIASES.INC
.equiv .LALIASES.INC, 1

/*
 * Индексные регистры:
*/

.ifdef .LMCp0411100101
 // Допустимые номера индексных регистров: 32 - 39
 .alias SP		39	; Указатель вершины стека
 .alias BP		38	; Указатель базы функции (фрейм).
 .alias SI		37	; Адрес источника данных.
 .alias DI		36	; Адрес приемника данных.
 .alias CX		35	; Счетчик.
 .alias sizeof.DM	0x20000	; Размер памяти данных на кристалле
.else
 .ifdef .LMCp042R100102
  // Допустимые номера индексных регистров: 8 - 15
  .alias SP		15	; Указатель вершины стека
  .alias BP		14	; Указатель базы функции (фрейм).
  .alias SI		13	; Адрес источника данных.
  .alias DI		12	; Адрес приемника данных.
  .alias CX		11	; Счетчик.
  .alias sizeof.DM	0x7FFF4	; Размер памяти на кристалле
 .else
  .err "Unsupported architecture"
  .end
 .endif
.endif

/*
 * Константы системные
*/

.alias sizeof.byte	1
.alias sizeof.short	2
.alias sizeof.long	4
.alias sizeof.quad	8
.alias sizeof.float	sizeof.long
.alias sizeof.double	sizeof.quad
.alias sizeof.ptr	sizeof.long

.endif
