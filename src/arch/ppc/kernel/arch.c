/**
 * @file
 *
 * @date Oct 17, 2012
 * @author: Anton Bondarev
 */

#include <hal/arch.h>
#include <compiler.h>

void arch_init(void) {
}

void arch_idle(void) {
}

void arch_shutdown(arch_shutdown_mode_t mode) _NORETURN {
	while (1) {}
}
