#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(){
	int answer = 0;
	long int result = syscall(334, 2, 3, &answer);
	printf("2+3 =:%d", answer);
	return 0;
}
