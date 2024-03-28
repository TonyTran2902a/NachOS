#include "syscall.h"
//#define maxlen 32

int main()
{
    OpenFileId file;
    int i;

    Create("quicksort.txt");
    file = Open("quicksort.txt", 0);
    for(i = 0; i < 10; i++)
    {
        char ch[3];
        ch[0] = i + 48;
        ch[1] = ' ';
        Write(ch, 3, file);
    }
    
    Close(file);

    Halt();
}
