/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.07.2015
 */

#include <stddef.h>

extern void kernel_start(void);

int errno;

int main(void) {
	kernel_start();
	return 0;
}
