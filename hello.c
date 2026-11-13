// SPDX-License-Identifier: GPL-2.0
/*
 * hello.c - "Hello, world!" Linux kernel module.
 *
 * The number of greetings is set by the "count" module parameter.
 * The kernel time of every greeting is stored in a linked list and
 * printed when the module is unloaded.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_AUTHOR("Vladyslav Musiienko, IO-44");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count,
		 "How many times to print \"Hello, world!\" (default 1; 0 or 5..10 - warning; >10 - error)");

struct hello_event {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(hello_list);

static void hello_free_list(bool print)
{
	struct hello_event *ev, *tmp;

	list_for_each_entry_safe(ev, tmp, &hello_list, list) {
		if (print)
			pr_info("event time: %lld ns\n",
				(long long)ktime_to_ns(ev->time));
		list_del(&ev->list);
		kfree(ev);
	}
}

static int __init hello_init(void)
{
	uint i;

	if (count > 10) {
		pr_err("invalid count=%u: must not be greater than 10\n",
		       count);
		return -EINVAL;
	}

	if (count == 0 || (count >= 5 && count <= 10))
		pr_warn("count=%u is 0 or between 5 and 10, continuing\n",
			count);

	for (i = 0; i < count; i++) {
		struct hello_event *ev = kmalloc(sizeof(*ev), GFP_KERNEL);

		if (!ev) {
			hello_free_list(false);
			return -ENOMEM;
		}

		ev->time = ktime_get();
		list_add_tail(&ev->list, &hello_list);
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	hello_free_list(true);
	pr_info("Goodbye!\n");
}

module_init(hello_init);
module_exit(hello_exit);
