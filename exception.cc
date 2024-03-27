// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void IncreasePC()
{
    int counter = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, counter);
    counter = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, counter);
    machine->WriteRegister(NextPCReg, counter + 4);
}

// Input: Khong gian dia chi User(int) - gioi han cua buffer(int)
// Output: Bo nho dem Buffer(char*)
// Chuc nang: Sao chep vung nho User sang vung nho System
char* User2System(int virtAddr, int limit)
{
    int i; //chi so index
    int oneChar;
    char* kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; //can cho chuoi terminal
    if (kernelBuf == NULL)
        return kernelBuf;
        
    memset(kernelBuf, 0, limit + 1);
    
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}


// Input: Khong gian vung nho User(int) - gioi han cua buffer(int) - bo nho dem buffer(char*)
// Output: So byte da sao chep(int)
// Chuc nang: Sao chep vung nho System sang vung nho User
int System2User(int virtAddr, int len, char* buffer)
{
    if (len < 0) return -1;
    if (len == 0)return len;
    int i = 0;
    int oneChar = 0;
    do{
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

void
ExceptionHandler(ExceptionType which)
{
    int op1, op2, result;
    int type = machine->ReadRegister(2);

    switch(which)
    {
        case NoException:
            return;

        case PageFaultException:
            DEBUG('a', "PageFault is happening!.\n");
            interrupt->Halt();
            break;

        case BusErrorException:
            DEBUG('a', "BusError is happening!.\n");
            interrupt->Halt();
            break;

        case AddressErrorException:
            DEBUG('a', "AddressError is happening!.\n");
            interrupt->Halt();
            break;

        case OverflowException:
            DEBUG('a', "Overflow is happening!.\n");
            interrupt->Halt();
            break;

        case IllegalInstrException:
            DEBUG('a', "IllegalInstr is happening!.\n");
            interrupt->Halt();
            break;

        case NumExceptionTypes:
            DEBUG('a', "IllegalInstr is happening!.\n");
            interrupt->Halt();
            break;

        case SyscallException:
                switch(type)
                {
                    case SC_Halt:
                        DEBUG('a', "Shutdown, initiated by user program.\n");
                        interrupt->Halt();
                        break;

                    case SC_Sub:
                        op1 = machine->ReadRegister (4); 
                        op2 = machine->ReadRegister (5); 
                        result = op1 - op2; 
                        machine->WriteRegister (2, result); 
                        interrupt->Halt();
                        break; 

                    case SC_PrintInt:
                    {  
                    // Input: mot so integer
                    // Output: khong co 
                    // Chuc nang: In so nguyen len man hinh console
                        int number = machine->ReadRegister(4);
                        if(number == 0)
                            {
                                gSynchConsole->Write("0", 1); // In ra man hinh so 0
                                IncreasePC();
                                return;    
                            }
                            
                            /*Qua trinh chuyen so thanh chuoi de in ra man hinh*/
                            bool isNegative = false; // gia su la so duong
                            int numberOfNum = 0; // Bien de luu so chu so cua number
                            int firstNumIndex = 0; 
                    
                            if(number < 0)
                            {
                                isNegative = true;
                                number = number * -1; // Nham chuyen so am thanh so duong de tinh so chu so
                                firstNumIndex = 1; 
                            }   
                            
                            int t_number = number; // bien tam cho number
                            while(t_number)
                            {
                                numberOfNum++;
                                t_number /= 10;
                            }
            
                            // Tao buffer chuoi de in ra man hinh
                            char* buffer;
                            int MAX_BUFFER = 255;
                            buffer = new char[MAX_BUFFER + 1];
                            for(int i = firstNumIndex + numberOfNum - 1; i >= firstNumIndex; i--)
                            {
                                buffer[i] = (char)((number % 10) + 48);
                                number /= 10;
                            }
                            if(isNegative)
                            {
                                buffer[0] = '-';
                            buffer[numberOfNum + 1] = 0;
                                gSynchConsole->Write(buffer, numberOfNum + 1);
                                delete buffer;
                                IncreasePC();
                                return;
                            }
                            buffer[numberOfNum] = 0;    
                            gSynchConsole->Write(buffer, numberOfNum);
                            delete buffer;
                            IncreasePC();
                            return;
                            break;
                    }

                    case SC_PrintChar:
                        {
                            // Input: Ki tu(char)
                            // Output: Ki tu(char)
                            // Cong dung: Xuat mot ki tu la tham so arg ra man hinh
                            char c = (char)machine->ReadRegister(4); // Doc ki tu tu thanh ghi r4
                            gSynchConsole->Write(&c, 1); // In ky tu tu bien c, 1 byte
                            IncreasePC();
                            break;
                        }

                    case SC_PrintString:
                        {
                            // Input: Buffer(char*)
                            // Output: Chuoi doc duoc tu buffer(char*)
                            // Cong dung: Xuat mot chuoi la tham so buffer truyen vao ra man hinh
                            int virtAddr;
                            char* buffer;
                            virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
                            buffer = User2System(virtAddr, 255); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai 255 ki tu
                            int length = 0;
                            while (buffer[length] != 0) length++; // Dem do dai that cua chuoi
                            gSynchConsole->Write(buffer, length + 1); // Goi ham Write cua SynchConsole de in chuoi
                            delete buffer; 
                            IncreasePC(); // Tang Program Counter 
                            //return;
                            break;
                        }

                    case SC_PrintFloat:
                        {
                            IncreasePC();
                            //return;
                            break;
                        }

                    case SC_Create: 
                        { 
                            int virtAddr; 
                            char* filename; 
                            //DEBUG(‘a’,"\n SC_Create call ..."); 
                            //DEBUG(‘a’,"\n Reading virtual address of filename"); 
                            // Lấy tham số tên tập tin từ thanh ghi r4 
                            virtAddr = machine->ReadRegister(4); 
                            //DEBUG (‘a’,"\n Reading filename."); 
                            // MaxFileLength là = 32 
                            filename = User2System(virtAddr,MaxFileLength+1); 
                            if (filename == NULL) 
                            { 
                             //printf("\n Not enough memory in system"); 
                             //DEBUG(‘a’,"\n Not enough memory in system"); 
                             machine->WriteRegister(2,-1); // trả về lỗi cho chương 
                             // trình người dùng 
                             delete filename; 
                             return; 
                            } 
                            //DEBUG(‘a’,"\n Finish reading filename."); 
                            //DEBUG(‘a’,"\n File name : '"<<filename<<"'"); 
                            // Create file with size = 0 
                            // Dùng đối tượng fileSystem của lớp OpenFile để tạo file, 
                            // việc tạo file này là sử dụng các thủ tục tạo file của hệ điều 
                            // hành Linux, chúng ta không quản ly trực tiếp các block trên 
                            // đĩa cứng cấp phát cho file, việc quản ly các block của file 
                            // trên ổ đĩa là một đồ án khác 
                            if (!fileSystem->Create(filename,0)) 
                            { 
                             //printf("\n Error create file '%s'",filename); 
                             machine->WriteRegister(2,-1); 
                             delete filename; 
                             return; 
                            } 
                            machine->WriteRegister(2,0); // trả về cho chương trình 
                             // người dùng thành công 
                            delete filename; 
                            break; 
                         }
 
                }                            
    }
}
