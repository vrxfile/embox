/*
 * omapl138_int.c
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

EMBOX_UNIT_INIT(omapl138_intc_init);

/**
 * Specefic constants (n = 0 to 2, m = 0 to 95)
 */
#define OMAPL138_INTC_BASE           0x48200000
#define OMAPL138_INTC_REVISION       (OMAPL138_INTC_BASE + 0x00000000) /* REV 7:0b */
#define OMAPL138_INTC_SYSCONFIG      (OMAPL138_INTC_BASE + 0x00000010) /* SOFTRESET 1b, AUTOIDLE 0b */
#define OMAPL138_INTC_SYSSTATUS      (OMAPL138_INTC_BASE + 0x00000014) /* RESETDONE 0b */
#define OMAPL138_INTC_SIR_IRQ        (OMAPL138_INTC_BASE + 0x00000040) /* SPURIOUSIRQFLAG 31:7b, ACTIVEIRQ 6:0b */
#define OMAPL138_INTC_SIR_FIQ        (OMAPL138_INTC_BASE + 0x00000044) /* SPURIOUSFIQFLAG 31:7b, ACTIVEFIQ 6:0b */
#define OMAPL138_INTC_CONTROL        (OMAPL138_INTC_BASE + 0x00000048) /* NEWFIQAGR 1b, NEWIRQAGR 0b */
#define OMAPL138_INTC_PROTECTION     (OMAPL138_INTC_BASE + 0x0000004C) /* PROTECTION 0b */
#define OMAPL138_INTC_IDLE           (OMAPL138_INTC_BASE + 0x00000050) /* TURBO 1b, FUNCIDLE 0b */
#define OMAPL138_INTC_IRQ_PRIORITY   (OMAPL138_INTC_BASE + 0x00000060) /* SPURIOUSIRQFLAG 31:6b, IRQPRIORITY 5:0b */
#define OMAPL138_INTC_FIQ_PRIORITY   (OMAPL138_INTC_BASE + 0x00000064) /* SPURIOUSFIQFLAG 31:6b, FIQPRIORITY 5:0b */
#define OMAPL138_INTC_THRESHOLD      (OMAPL138_INTC_BASE + 0x00000068) /* PRIORITYTHRESHOLD 7:0b */
#define OMAPL138_INTC_ITR(n)         (OMAPL138_INTC_BASE + 0x00000080 + (n) * 0x20) /* ITR 31:0b */
#define OMAPL138_INTC_MIR(n)         (OMAPL138_INTC_BASE + 0x00000084 + (n) * 0x20) /* MIR 31:0b */
#define OMAPL138_INTC_MIR_CLEAR(n)   (OMAPL138_INTC_BASE + 0x00000088 + (n) * 0x20) /* MIRCLEAR 31:0b */
#define OMAPL138_INTC_MIR_SET(n)     (OMAPL138_INTC_BASE + 0x0000008C + (n) * 0x20) /* MIRSET 31:0b */
#define OMAPL138_INTC_ISR_SET(n)     (OMAPL138_INTC_BASE + 0x00000090 + (n) * 0x20) /* ISRSET 31:0b */
#define OMAPL138_INTC_ISR_CLEAR(n)   (OMAPL138_INTC_BASE + 0x00000094 + (n) * 0x20) /* ISRCLEAR 31:0b */
#define OMAPL138_INTC_PENDING_IRQ(n) (OMAPL138_INTC_BASE + 0x00000098 + (n) * 0x20) /* PENDINGIRQ 31:0b */
#define OMAPL138_INTC_PENDING_FIQ(n) (OMAPL138_INTC_BASE + 0x0000009C + (n) * 0x20) /* PENDINGFIQ 31:0b */
#define OMAPL138_INTC_ILR(m)         (OMAPL138_INTC_BASE + 0x00000100 + (m) * 0x4) /* PRIORITY 7:2b, FIQNIRQ 0b */

#define INTC_SYSCONFIG_RESET (1 << 1)
#define INTC_CONTROL_NEWIRQARG (1 << 0)
#define INTC_CONTROL_NEWFIQARG (1 << 1)
#define INTC_SIR_IRQ_ACTIVE_MASK 0x7f

void software_init_hook(void) {
	REG_STORE(OMAPL138_INTC_SYSCONFIG, INTC_SYSCONFIG_RESET);

	REG_STORE(OMAPL138_INTC_IDLE, 0x0);

	for (int m = 0; m < __IRQCTRL_IRQS_TOTAL; ++m) {
		REG_STORE(OMAPL138_INTC_ILR(m), 0x0);
	}

	REG_STORE(OMAPL138_INTC_MIR_SET(0), ~0);
	REG_STORE(OMAPL138_INTC_MIR_SET(1), ~0);
	REG_STORE(OMAPL138_INTC_MIR_SET(2), ~0);

	REG_STORE(OMAPL138_INTC_ISR_SET(0), 0);
	REG_STORE(OMAPL138_INTC_ISR_SET(1), 0);
	REG_STORE(OMAPL138_INTC_ISR_SET(2), 0);
}


static int omapl138_intc_init(void)
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
	REG_STORE(OMAPL138_INTC_MIR_CLEAR(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

void irqctrl_disable(unsigned int interrupt_nr)
{
	printk("point irq_dis\n");
	REG_STORE(OMAPL138_INTC_MIR_SET(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

void irqctrl_clear(unsigned int interrupt_nr)
{
	printk("point irq_clr\n");
	REG_STORE(OMAPL138_INTC_ISR_CLEAR(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

void irqctrl_force(unsigned int interrupt_nr)
{
	printk("point irq_force\n");
	REG_STORE(OMAPL138_INTC_ISR_SET(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
}

int irqctrl_pending(unsigned int interrupt_nr)
{
	return REG_LOAD(OMAPL138_INTC_PENDING_IRQ(interrupt_nr >> 5)) & 1 << (interrupt_nr & 0x1f);
}

void interrupt_handle(void)
{
	printk("point 1\n");
	unsigned int irq = REG_LOAD(OMAPL138_INTC_SIR_IRQ) & INTC_SIR_IRQ_ACTIVE_MASK;

	printk("point 2\n");
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	printk("point 3\n");
	irqctrl_disable(irq);

	printk("point 4\n");
	REG_STORE(OMAPL138_INTC_CONTROL, INTC_CONTROL_NEWIRQARG); /* reset IRQ output and enable new IRQ generation */

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
	printk("point 6\n");
	irqctrl_enable(irq);
	printk("point 7\n");
	critical_leave(CRITICAL_IRQ_HANDLER);
	printk("point 8\n");
	critical_dispatch_pending();
	printk("point 9\n");
}

void swi_handle(void)
{
	printk("swi!\n");
}

