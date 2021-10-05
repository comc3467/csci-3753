Conlan McConvey, comc3467@colorado.edu, ID: 109582433

In this assigment, I'm submitting 6 total files, 2 of which are test
cases located in the /home directory. The other 4 file paths are:
/home/kernel/linux-hwe-4.15.0/arch/x86/kernel/csci3753add.c
/home/kernel/linux-hwe-4.15.0/arch/x86/kernel/Makefile
/home/kernel/linux-hwe-4.15.0/arch/x86/entry/syscalls/syscall_64.tbl
/home/kernel/linux-hwe-4.15.0/include/linux/syscalls.h

test.c is the file that tests the case for the hello world system
call, and test-new.c is the file that tests the add system call. Both
of these files are in the /home directory, and should be used to test the
the program using 'gcc test.c' and './a.out'. For building these
calls, you must first make the edits to the appropriate files
(assuming you have  access to the linux-hwe-4.15.0), and then running
the 3 'sudo make' commands in the writeup
sudo make -j2 CC="ccache gcc"
sudo make -j2 modules_install
sudo make -j2 install
sudo reboot now

and then you can compile and test.

