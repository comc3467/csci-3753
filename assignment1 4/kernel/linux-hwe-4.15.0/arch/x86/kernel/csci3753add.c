#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage long sys_csci3753add(int number1, int number2, int *answer)
{
  printk(KERN_ALERT "Value for number 1 is:%d", number1);
  printk(KERN_ALERT "Value for number 2 is %d\n", number2);
  *answer = number1 + number2;
  printk(KERN_ALERT "Result of number1 + number2 is %d", *answer);
  return 0;
}
