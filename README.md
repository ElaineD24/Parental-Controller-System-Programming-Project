# Parental-Controller-System-Programming-Project

V1.0
Apr 2018

Yizhang Cao
Elaine Deng

Operating System: Ubuntu (32bit) Linux 16.04

Compile
make
gcc -o userspace userspace.c

Installation
sudo insmod ParentalControl

Testing
./userspace

Removal
sudo rmmod ParentalControl

Description
•	Designed and Developed a Parental Controller for Linux consisting a kernel module and a user interface which allows restrictions to user specified IP addresses
•	Implemented the kernel module using Packet Filtering Hooks API
