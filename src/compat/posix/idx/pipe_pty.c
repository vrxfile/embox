/**
 * @file
 *
 * @brief
 *
 * @date 17.11.2013
 * @author Anton Kozlov
 */

#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <poll.h>

#include <util/ring_buff.h>
#include <util/ring.h>
#include <util/member.h>

#include <drivers/pty.h>
#include <kernel/task.h>
#include <fs/idesc.h>
#include <fs/idesc_event.h>
#include <fs/flags.h>

#include <kernel/thread/thread_sched_wait.h>

#include <kernel/task/idesc_table.h>
#include <kernel/task/resource/idesc_table.h>
#include <mem/misc/pool.h>

struct pty;

struct idesc_pty {
	struct idesc idesc;
	struct pty *pty;
};

#define MAX_PTY 20
POOL_DEF(pty_pool, struct pty, MAX_PTY);
POOL_DEF(ipty_pool, struct idesc_pty, 2 * MAX_PTY);

#if 0
struct pty {
	struct pty pty;
	/*struct idesc *master, *slave;*/
};
#endif

static void pty_out_wake(struct tty *t) {
	struct pty *pty = pty_from_tty(t);

	idesc_notify(pty->master, POLLIN);
}

static inline int pty_wait(struct idesc *idesc, struct tty *t, int flags) {
	struct idesc_wait_link wl;

	return IDESC_WAIT_LOCKED(mutex_unlock(&t->lock),
			idesc, &wl, flags, SCHED_TIMEOUT_INFINITE,
			mutex_lock(&t->lock));
}

size_t pty_read(struct pty *pt, struct idesc *idesc, char *buff, size_t size) {
	struct tty *t = pty_to_tty(pt);
	int ret;

	mutex_lock(&t->lock);
	{
		do {
			ret = tty_out_buf(t, buff, size);
			if (ret) {
				break;
			}

			if (!t->idesc) {
				break;
			}

			ret = pty_wait(idesc, t, POLLIN | POLLERR);
		} while (!ret);
	}
	mutex_unlock(&t->lock);

	return ret;
}

size_t pty_write(struct pty *pt, const char *buff, size_t size) {
	struct tty *t = pty_to_tty(pt);
	const char *saved_buff = buff;

	while (size--)
		tty_rx_putc(t, *buff++, 0);

	return buff - saved_buff;
}

const struct tty_ops pty_ops = {
	.out_wake = pty_out_wake,
};

struct pty *pty_init(struct pty *p, struct idesc *master, struct idesc *slave) {

	tty_init(pty_to_tty(p), &pty_ops);
	p->tty.idesc = slave;
	p->master = master;

	return p;
}
static void pty_close(struct idesc *idesc);
static int pty_ioctl(struct idesc *idesc, int request, void *data);
static int pty_slave_write(struct idesc *desc, const void *buf, size_t nbyte);
static int pty_slave_read(struct idesc *idesc, void *buf, size_t nbyte);
static int pty_master_write(struct idesc *desc, const void *buf, size_t nbyte);
static int pty_master_read(struct idesc *idesc, void *buf, size_t nbyte);
static int pty_fstat(struct idesc *data, void *buff);
static int pty_master_status(struct idesc *idesc, int mask);
static int pty_slave_status(struct idesc *idesc, int mask);

static const struct idesc_ops pty_master_ops = {
		.write = pty_master_write,
		.read  = pty_master_read,
		.close = pty_close,
		.ioctl = pty_ioctl,
		/*.fstat = pty_fstat,*/
		.status = pty_master_status,
};

static const struct idesc_ops pty_slave_ops = {
		.write  = pty_slave_write,
		.read   = pty_slave_read,
		.close  = pty_close,
		.ioctl  = pty_ioctl,
		.fstat  = pty_fstat,
		.status = pty_slave_status,
};

static struct pty *pty_create(void) {
	struct pty *pty;

	pty = pool_alloc(&pty_pool);

//	if (pty) {
//		pty_init(&pty->pty);
//	}

	return pty;
}

static void pty_delete(struct pty *pty) {

	pool_free(&pty_pool, pty);
}

static struct idesc_pty *idesc_pty_create(struct pty *pty, const struct idesc_ops *ops) {
	struct idesc_pty *ipty;

	ipty = pool_alloc(&ipty_pool);

	if (ipty) {
		idesc_init(&ipty->idesc, ops, FS_MAY_READ | FS_MAY_WRITE);
	}

	ipty->pty = pty;

	return ipty;
}

static void idesc_pty_delete(struct idesc_pty *ipty, struct idesc **idesc) {

	*idesc = NULL;

	pool_free(&ipty_pool, ipty);
}

#if 0
static int pty_fixup_error(struct idesc *idesc, int code) {
	struct pty *pty;
	struct idesc *idesc_other;

	/* Negative => error, positive => some data read */
	if (code != 0) {
		return code;
	}

	pty = ((struct idesc_pty *) idesc)->pty;

	if (idesc == pty->master) {
		idesc_other = pty->slave;
	} else {
		assert(idesc == pty->slave);
		idesc_other = pty->master;
	}

	if (idesc_other == NULL) {
		return 0;
	}

//	if (code == -EAGAIN) {
//		return -EAGAIN;
//	}

	assert(idesc->idesc_flags & O_NONBLOCK);
	return -EAGAIN;
}
#endif

static void pty_close(struct idesc *idesc) {
	struct idesc_pty *ipty = (struct idesc_pty *) idesc;
	struct idesc **ipty_pm, **ipty_ps;
	struct pty *pty;

	ipty_pm = &ipty->pty->master;
	ipty_ps = &ipty->pty->tty.idesc;

	pty = ipty->pty;

	sched_lock();
	{
		if (idesc == *ipty_pm) {
			idesc_pty_delete((struct idesc_pty *) idesc, ipty_pm);
			/* Wake up writing end if it is sleeping. */
			if (*ipty_ps)
				idesc_notify(*ipty_ps, POLLIN | POLLOUT | POLLERR);
		} else if (idesc == *ipty_ps) {
			idesc_pty_delete((struct idesc_pty *) idesc, ipty_ps);
			/* Wake up reading end if it is sleeping. */
			if (*ipty_pm)
				idesc_notify(*ipty_pm, POLLIN | POLLOUT | POLLERR);
		}

		/* Free memory if both of ends are closed. */
		if (NULL == *ipty_pm && NULL == *ipty_ps) {
			pty_delete(pty);
		}

	}
	sched_unlock();
}

static ssize_t pty_master_write(struct idesc *desc, const void *buf, size_t nbyte) {
	struct idesc_pty *ipty = (struct idesc_pty *)desc;
	assert(ipty != NULL);
	return pty_write(ipty->pty, buf, nbyte);
}

static ssize_t pty_master_read(struct idesc *desc, void *buf, size_t nbyte) {
	struct idesc_pty *ipty = (struct idesc_pty *) desc;
	assert(ipty != NULL);
	return pty_read(ipty->pty, desc, buf, nbyte);
}

static ssize_t pty_slave_write(struct idesc *desc, const void *buf, size_t nbyte) {
	struct idesc_pty *ipty = (struct idesc_pty *) desc;
	assert(ipty != NULL);
	return tty_write(pty_to_tty(ipty->pty), buf, nbyte);
}

static ssize_t pty_slave_read(struct idesc *desc, void *buf, size_t nbyte) {
	struct idesc_pty *ipty = (struct idesc_pty *) desc;
	assert(ipty != NULL);
	return tty_read(pty_to_tty(ipty->pty), buf, nbyte);
}

static int pty_fstat(struct idesc *data, void *buff) {
	struct stat *st = buff;

	st->st_mode = S_IFCHR;

	return 0;

}

static int pty_ioctl(struct idesc *idesc, int request, void *data) {
	struct idesc_pty *ipty = (struct idesc_pty *) idesc;
	assert(ipty != NULL);
	return tty_ioctl(pty_to_tty(ipty->pty), request, data);
}

//TODO check it
static int pty_master_status(struct idesc *idesc, int mask) {
	struct idesc_pty *ipty = (struct idesc_pty *) idesc;
	struct pty *pty = ipty->pty;
	int res;

	/* if slave is closed read/write/err will not block and will
 	 * cause error */
	if (!ipty->pty->tty.idesc) {
		return 1;
	}

	switch (mask) {
	case POLLIN:
		res = ring_can_read(&pty_to_tty(pty)->o_ring, TTY_IO_BUFF_SZ, 1);
		break;
	case POLLOUT:
		res = ring_can_write(&pty_to_tty(pty)->rx_ring, TTY_RX_BUFF_SZ, 1);
		break;
	default:
	case POLLERR:
		res = 0;
		break;
	}

	return res;
}

static int pty_slave_status(struct idesc *idesc, int mask) {
	struct idesc_pty *ipty = (struct idesc_pty *) idesc;
	struct pty *pty = ipty->pty;

	/* if master is closed read/write/err will not block and will
 	 * cause error */
	if (!ipty->pty->master) {
		return 1;
	}

	return tty_status(pty_to_tty(pty), mask);
}

int ppty(int ptyfds[2]) {
	int res;
	struct pty *pty;
	struct idesc_pty *master, *slave;
	struct idesc_table *it;

	it = task_resource_idesc_table(task_self());

	pty = NULL;
	master = slave = NULL;
	ptyfds[0] = ptyfds[1] = -1;

	pty = pty_create();
	if (!pty) {
		res = ENOMEM;
		goto out_err;
	}

	master = idesc_pty_create(pty, &pty_master_ops);
	slave = idesc_pty_create(pty, &pty_slave_ops);
	pty_init(pty, &master->idesc, &slave->idesc);

	if (!master || !slave) {
		res = ENOMEM;
		goto out_err;
	}

	ptyfds[0] = idesc_table_add(it, &master->idesc, 0);
	ptyfds[1] = idesc_table_add(it, &slave->idesc, 0);

	if (ptyfds[0] < 0) {
		res = -ptyfds[0];
		goto out_err;
	}

	if (ptyfds[1] < 0) {
		res = -ptyfds[1];
		goto out_err;
	}

	return 0;

out_err:
	if (ptyfds[1] >= 0) {
		idesc_table_del(it, ptyfds[1]);
	}
	if (ptyfds[0] >= 0) {
		idesc_table_del(it, ptyfds[0]);
	}
	if (master) {
		idesc_pty_delete(master, &pty->master);
	}
	if (slave) {
		idesc_pty_delete(slave, &pty->tty.idesc);
	}
	if (pty) {
		pty_delete(pty);
	}
	return SET_ERRNO(res);
}
