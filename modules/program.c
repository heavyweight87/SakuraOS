//#include <stdio.h>
//#include<fcntl.h> 

int main()
{
	//int fd = open("foo.txt", O_RDONLY | O_CREAT);  
//	return fd;
asm volatile("movl $0xDEADBEEF, %eax");
while(1);
return 0;
}
