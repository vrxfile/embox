#ifndef MC_STDARG_H
#define MC_STDARG_H
/*
 * При данном варианте передача указателя на следующий неявный
 * формальный параметр (сохранён в переменной vl типа va_list), в
 * какую-либо функцию осуществляется по значению. И это очень неудобно,
 * так как весь код функции с переменным числом формальных параметров,
 * зависящий от параметров, извлекаемых с помощью va_arg(vl, <TYPE>),
 * должен располагаться в этой функции.
*/
/*
typedef struct __va_list* va_list;

#define __VA_ALIGNED_SIZE(x) ((sizeof(x) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))

#define va_start(ap, last) ((ap) = (void *)(((char *)&(last)) + __VA_ALIGNED_SIZE(last)))
#define va_end(ap) ((void*)0)

#define va_arg(ap, type) \
	( ((ap) = (va_list)((char *)(ap) + __VA_ALIGNED_SIZE(type))), \
	*(type *)(void *)((char *)(ap) - __VA_ALIGNED_SIZE(type)) )
*/

////////////////////////////////////////////////////////////////////////

/*
 * При данном варианте передача указателя на следующий неявный
 * формальный параметр в какую-либо функцию осуществляется с помощью
 * вспомогательной локальной структуры с именем __va_list__ (указатель
 * на которую сохранён в переменной vl типа va_list). В этом случае
 * весь код функции с переменным числом формальных параметров,
 * зависящий от параметров, извлекаемых с помощью va_arg(vl, <TYPE>),
 * может быть разбит на несколько функций. В тоже время накладываются
 * следующие ограничения:
 *  1. функция не должна содержать локальную переменную с именем
 *     __va_list__;
 *  2. так как локальная структура с именем __va_list__ объявляется в
 *     макросе va_start, то:
 *     2.1 макрос va_start в исходном коде функции должен быть
 *         использован только один раз;
 *     2.2 для C89 макрос va_start должен располагаться
 *         в исходном коде функции сразу после объявления последней
 *         локальной переменной функции и ни в каком другом месте.
*/

typedef struct
{
 void *stack_ptr;
} __va_list;

typedef __va_list* va_list;

#define __VA_ALIGNED_SIZE(x) ((sizeof(x) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))

#define va_start(ap, last) __va_list __va_list__ = { 0 }; __va_list__.stack_ptr = (void *)(((char *)&(last)) + __VA_ALIGNED_SIZE(last)); (ap) = &__va_list__
#define va_end(ap) ((void*)0)

#define va_arg(ap, type) \
	( ((ap)->stack_ptr = (va_list)((char *)(ap)->stack_ptr + __VA_ALIGNED_SIZE(type))), \
	*(type *)(void *)((char *)(ap)->stack_ptr - __VA_ALIGNED_SIZE(type)) )
#endif
