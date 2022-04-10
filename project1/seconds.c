#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>
#include <asm/param.h>

#define BUFFER_SIZE 128

#define PROC_NAME "seconds"

unsigned long int volatile fl_jiffies, r_jiffies;
const int hz = HZ;

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

//static struct file_operations proc_ops = {
//        .owner = THIS_MODULE,
//        .read = proc_read,
//};

static struct proc_ops proc_ops = {
        //.owner = THIS_MODULE,
        .proc_read = proc_read,
};

//const struct proc_ops proc_ops2 = {
//	.read = proc_read,
//}


/* This function is called when the module is loaded. */
int proc_init(void)
{
        proc_create(PROC_NAME, 0, NULL, &proc_ops);
        fl_jiffies = jiffies;

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {

        remove_proc_entry(PROC_NAME, NULL);

        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/jiffies is read.
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;
	
	r_jiffies = jiffies;
        rv = sprintf(buffer, "%d seconds have elapesd.\n", (r_jiffies-fl_jiffies)/hz);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("SGG");

