#include <stdio.h>
#include<fcntl.h> 

int main()
{
	int fd = open("foo.txt", O_RDONLY | O_CREAT);  
	return fd;
}
