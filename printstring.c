#include "syscall.h"

int main()
{
    PrintChar('k');
    PrintString("\n");
    PrintString("F*** NachOS!");
    Halt();
    /* not reached */
}
