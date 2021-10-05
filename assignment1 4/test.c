#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
int main()
{
	long int hello = syscall(333);
	printf("Call 333 Return: %ld\n", hello);
	return 0;
}
