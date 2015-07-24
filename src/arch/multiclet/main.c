/**
 * @file 
 * @brief
 *
 * @author  Anton Kozlov 
 * @date    21.07.2015
 */

#include <stddef.h>

extern void kernel_start(void);

struct diag;
extern void mcp_putc(const struct diag *dev, char ch);
extern void mcp_init(const struct diag *dev);

int main(void) {
	mcp_init(NULL);
	mcp_putc(NULL, 'a');
	/*kernel_start();*/
}

