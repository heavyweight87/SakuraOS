#include <stdio.h>
#include<fcntl.h> 

int main()
{
    int x = 0;
    while(1)
    {
        x++;
	    printf("Hello module! x = %d", x);
    }
	return 0;
}
