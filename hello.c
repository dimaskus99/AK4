#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Horbyk Dmytro <horbyk.dmytro2017@gmail.com>");
MODULE_DESCRIPTION("Hello, world with parameters and list in Linux Kernel");
MODULE_LICENSE("Dual BSD/GPL");

static uint hello_count = 1;
module_param(hello_count, uint, 0644);
MODULE_PARM_DESC(hello_count, "Number of times to print 'Hello, world!'");

struct hello_event {
	struct list_head list;
	ktime_t timestamp;
};

static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
	struct hello_event *new_event;
	unsigned int i;

	if (hello_count == 0 || (hello_count >= 5 && hello_count <= 10)) {
		pr_warn("Invalid hello_count: %d. Proceeding anyway.\n",
			hello_count);
	} else if (hello_count > 10) {
		pr_err("hello_count too large: %d. Module will not load.\n",
		       hello_count);
		return -EINVAL;
	}

	for (i = 0; i < hello_count; i++) {
		pr_emerg("Hello, world!\n");

		new_event = kmalloc(sizeof(*new_event), GFP_KERNEL);
		if (!new_event) {
			pr_err("Failed to allocate memory for event\n");
			return -ENOMEM;
		}

		new_event->timestamp = ktime_get();
		list_add(&new_event->list, &hello_list);
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_event *event;
	struct list_head *pos, *q;

	pr_emerg("Exiting 'Hello, world!' module\n");

	list_for_each_safe(pos, q, &hello_list) {
		event = list_entry(pos, struct hello_event, list);
		pr_emerg("Event timestamp: %lld ns\n",
			 ktime_to_ns(event->timestamp));
		list_del(pos);
		kfree(event);
	}
}

module_init(hello_init);
module_exit(hello_exit);
