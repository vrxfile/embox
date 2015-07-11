/**
 * @file
 * @brief No-op stubs for interrupt/IPL interfaces.
 *
 * @date Sep 15, 2012
 * @author Eldar Abusalimov
 */

#ifndef ARCH_INTERRUPT_STUB_H_
#define ARCH_INTERRUPT_STUB_H_

#define INTERRUPT_STUB

#ifndef __ASSEMBLER__

#include <compiler.h>

typedef unsigned int __ipl_t;

static _INLINE void ipl_init(void) { }
static _INLINE __ipl_t ipl_save(void) { return 0; }
static _INLINE void ipl_restore(__ipl_t ipl) { }

#endif /* __ASSEMBLER__ */

#endif /* ARCH_INTERRUPT_STUB_H_ */
