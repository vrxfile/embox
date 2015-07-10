/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.07.2015
 */

#ifndef COMPILER_H_
#define COMPILER_H_

#ifdef __GNUC__

#define _NORETURN __attribute__ ((noreturn))

#else /* __GNUC__ */

#define _NORETURN

#endif /* __GNUC__ */

#endif /* COMPILER_H_ */

