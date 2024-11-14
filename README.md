# Kernel-error-handling-sytem
This project implements a Linux kernel module that captures kernel logs and exposes them through the /proc/captured_logs file. It uses the kmsg_dump API to capture logs and provides a simple interface for users to read logs from userspace for debugging and monitoring purposes. Licensed under GPL.

Build the Module:

Clone the repository.
Use make to build the module


Insert the Module:
sudo insmod log_capture.ko

Unload the Module:
sudo rmmod log_capture


