/**
 * @file
 * @brief TODO stub
 *
 * @date 14.02.10
 * @author Anton Bondarev
 */

#include <hal/arch.h>
#include <asm/cache.h>
#include <compiler.h>

void arch_init(void) {
	cache_enable();
}

void arch_idle(void) {
}

void arch_shutdown(arch_shutdown_mode_t mode) _NORETURN {
	while (1) {}
}
