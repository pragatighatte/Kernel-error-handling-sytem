#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/kmsg_dump.h>
#include <linux/slab.h>

#define PROC_FILENAME "captured_logs"
#define BUFFER_SIZE 8192  // Increase the buffer size if needed

static struct proc_dir_entry *proc_file;
static char *log_buffer;
static ssize_t log_size = 0;

// Function to capture kernel logs
static void capture_kernel_logs(void) {
    struct kmsg_dump_iter iter;
    size_t len = 0;
    int ret;

    kmsg_dump_rewind(&iter);

    ret = kmsg_dump_get_buffer(&iter, true, log_buffer, BUFFER_SIZE, &len);
    if (ret < 0) {
        printk(KERN_ERR "Failed to capture kernel logs, error: %d\n", ret);
        return;
    }

    if (len >= BUFFER_SIZE) {
        printk(KERN_ERR "Log buffer overflow, captured logs truncated.\n");
        len = BUFFER_SIZE - 1;
    }

    log_buffer[len] = '\0'; // Null-terminate the buffer
    log_size = len;
}

// Function to read logs via /proc file
static ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    if (*pos > 0 || count < log_size)
        return 0;

    if (copy_to_user(buf, log_buffer, log_size))
        return -EFAULT;

    *pos = log_size;
    return log_size;
}

static const struct proc_ops proc_file_ops = {
    .proc_read = proc_read,
};

// Module initialization
static int __init log_capture_init(void) {
    log_buffer = kzalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!log_buffer) {
        printk(KERN_ERR "Failed to allocate memory for log buffer.\n");
        return -ENOMEM;
    }

    proc_file = proc_create(PROC_FILENAME, 0444, NULL, &proc_file_ops);
    if (!proc_file) {
        printk(KERN_ERR "Failed to create /proc file.\n");
        kfree(log_buffer);
        return -ENOMEM;
    }

    capture_kernel_logs();
    printk(KERN_INFO "Log capture module loaded.\n");
    return 0;
}

// Module cleanup
static void __exit log_capture_exit(void) {
    remove_proc_entry(PROC_FILENAME, NULL);
    kfree(log_buffer);
    printk(KERN_INFO "Log capture module unloaded.\n");
}

module_init(log_capture_init);
module_exit(log_capture_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pragati Ghatte");
MODULE_DESCRIPTION("A kernel module to capture and export kernel logs to /proc/captured_logs");

