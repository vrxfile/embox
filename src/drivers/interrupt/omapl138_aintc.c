/*
 * omapl138_aintc.c
 *
 *  Created on: Aug 12, 2015
 *      Author: Rostislav Varzar
 */

#include <assert.h>

#include <kernel/critical.h>
#include <hal/reg.h>
#include <hal/ipl.h>
#include <drivers/irqctrl.h>

#include <kernel/irq.h>
#include <embox/unit.h>
#include <kernel/printk.h>

EMBOX_UNIT_INIT(omapl138_aintc_init);

/*
 * Specific constants (n = 0 to 3, m = 0 to 25, p = 0 to 1)
 */
#define OMAPL138_AINTC_BASE1			0xFFFEE000
#define OMAPL138_AINTC_BASE2			0xFFFEF000
#define OMAPL138_AINTC_REVID			(OMAPL138_AINTC_BASE1 + 0x00000000)	/* Revision ID of the AINTC - 4E82A900h */
#define OMAPL138_AINTC_CR				(OMAPL138_AINTC_BASE1 + 0x00000004)	/* Control Register */
#define OMAPL138_AINTC_GER				(OMAPL138_AINTC_BASE1 + 0x00000010)	/* Global Enable Register */
#define OMAPL138_AINTC_GNLR				(OMAPL138_AINTC_BASE1 + 0x0000001C)	/*  */
#define OMAPL138_AINTC_SISR				(OMAPL138_AINTC_BASE1 + 0x00000020)	/*  */
#define OMAPL138_AINTC_SICR				(OMAPL138_AINTC_BASE1 + 0x00000024)	/*  */
#define OMAPL138_AINTC_EISR				(OMAPL138_AINTC_BASE1 + 0x00000028)	/*  */
#define OMAPL138_AINTC_EICR				(OMAPL138_AINTC_BASE1 + 0x0000002C)	/*  */
#define OMAPL138_AINTC_HIEISR			(OMAPL138_AINTC_BASE1 + 0x00000034)	/*  */
#define OMAPL138_AINTC_HIEICR			(OMAPL138_AINTC_BASE1 + 0x00000038)	/*  */
#define OMAPL138_AINTC_VBR				(OMAPL138_AINTC_BASE1 + 0x00000050)	/*  */
#define OMAPL138_AINTC_VSR				(OMAPL138_AINTC_BASE1 + 0x00000054)	/*  */
#define OMAPL138_AINTC_VNR				(OMAPL138_AINTC_BASE1 + 0x00000058)	/*  */
#define OMAPL138_AINTC_GPIR				(OMAPL138_AINTC_BASE1 + 0x00000080)	/*  */
#define OMAPL138_AINTC_GPVR				(OMAPL138_AINTC_BASE1 + 0x00000084)	/*  */
#define OMAPL138_AINTC_SRSR(n)			(OMAPL138_AINTC_BASE1 + 0x00000200 + ((n) * 0x04))	/*  */
#define OMAPL138_AINTC_SECR(n)			(OMAPL138_AINTC_BASE1 + 0x00000280 + ((n) * 0x04))	/*  */
#define OMAPL138_AINTC_ESR(n)			(OMAPL138_AINTC_BASE1 + 0x00000300 + ((n) * 0x04))	/*  */
#define OMAPL138_AINTC_ECR(n)			(OMAPL138_AINTC_BASE1 + 0x00000380 + ((n) * 0x04))	/*  */
#define OMAPL138_AINTC_CMR(m)			(OMAPL138_AINTC_BASE1 + 0x00000400 + ((m) * 0x04))	/*  */
#define OMAPL138_AINTC_HIPIR(p)			(OMAPL138_AINTC_BASE1 + 0x00000900 + ((p) * 0x04))	/*  */
#define OMAPL138_AINTC_HINLR(p)			(OMAPL138_AINTC_BASE2 + 0x00000100 + ((p) * 0x04))	/*  */
#define OMAPL138_AINTC_HIER				(OMAPL138_AINTC_BASE2 + 0x00000500)	/*  */
#define OMAPL138_AINTC_HIPVR(p)			(OMAPL138_AINTC_BASE2 + 0x00000600 + ((p) * 0x04))	/*  */

#define AINTC_GLOBAL_ENABLE				(1 << 0)
#define AINTC_FIQ_ENABLE				(1 << 0)
#define AINTC_IRQ_ENABLE				(1 << 1)
#define AINTC_IRQ_ACTIVE_MASK			0x7F

void software_init_hook(void)
{
	printk("point init_hook\n");
	REG_STORE(OMAPL138_AINTC_GER, AINTC_GLOBAL_ENABLE);

	REG_STORE(OMAPL138_AINTC_HIER, (AINTC_FIQ_ENABLE | AINTC_IRQ_ENABLE));

	for (unsigned int m = 0; m < __IRQCTRL_IRQS_TOTAL; ++m)
	{
		irqctrl_clear(m);
		irqctrl_disable(m);
	}

	// To add default priority settings!
}

static int omapl138_aintc_init(void)
{
	return 0;
}

/* 32 bits per word;
 * interrupt_nr: lower 5 bits -- offset in word,
 * else -- register n
 */
void irqctrl_enable(unsigned int interrupt_nr)
{
	printk("point irq_en\n");
	REG_STORE(OMAPL138_AINTC_ESR(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

void irqctrl_disable(unsigned int interrupt_nr)
{
	printk("point irq_dis\n");
	REG_STORE(OMAPL138_AINTC_ECR(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

void irqctrl_clear(unsigned int interrupt_nr)
{
	printk("point irq_clr\n");
	REG_STORE(OMAPL138_AINTC_SECR(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

void irqctrl_force(unsigned int interrupt_nr)
{
	printk("point irq_force\n");
	REG_STORE(OMAPL138_AINTC_SRSR(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

int irqctrl_pending(unsigned int interrupt_nr)
{
	printk("point pending_irq\n");
	return REG_LOAD(OMAPL138_AINTC_SECR(interrupt_nr >> 5)) & 1 << (interrupt_nr & 0x1f);
}

void interrupt_handle(void)
{
	printk("point 1\n");
	//unsigned int irq = REG_LOAD(OMAPL138_INTC_SIR_IRQ) & INTC_SIR_IRQ_ACTIVE_MASK;
	unsigned int irq = REG_LOAD(OMAPL138_AINTC_GPIR) & AINTC_IRQ_ACTIVE_MASK;

	printk("point 2\n");
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	printk("point 3\n");
	irqctrl_disable(irq);

	printk("point 4\n");
	//REG_STORE(OMAPL138_INTC_CONTROL, INTC_CONTROL_NEWIRQARG); /* reset IRQ output and enable new IRQ generation */
	irqctrl_clear(irq);

	printk("point 5\n");
	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		printk("point 5.1\n");
		ipl_enable();

		printk("point 5.2\n");
		irq_dispatch(irq);

		printk("point 5.3\n");
		ipl_disable();

	}

	//irqctrl_clear(irq);

	printk("point 6\n");
	irqctrl_enable(irq);

	printk("point 7\n");
	critical_leave(CRITICAL_IRQ_HANDLER);

	printk("point 8\n");
	critical_dispatch_pending();

	printk("point end\n");
}

void swi_handle(void)
{
	printk("swi!\n");
}

