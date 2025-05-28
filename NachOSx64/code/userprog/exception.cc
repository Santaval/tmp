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
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <unistd.h>


/*
 *  System call interface: Halt()
 */
void NachOS_Halt() {		// System call 0

	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();

}


/*
 *  System call interface: void Exit( int )
 */
void NachOS_Exit() {		// System call 1
   DEBUG('u', "Exit system call\n");
   int exitStatus = machine->ReadRegister(4); 
   currentThread->setExitStatus(exitStatus);
   currentThread->Finish();	// Current thread is finishing
   return;

}

void AdvancePC() {
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
    return;
}
/**
 * Auxiliary function for Exec()
 */
void StartProcess(void*) {
   // set all registers in zero
   // PCReg, PreviousPCReg, NextPCReg
    openFileTable->addThread();
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    machine->Run();
}

void StartExecProcess(void*) {
   // set all registers in zero
   // PCReg, PreviousPCReg, NextPCReg
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    machine->Run(); // no return
}

/*
 *  System call interface: SpaceId Exec( char * )
 */
void NachOS_Exec() {		// System call 2
   // the name of the executable was previously stored in a register
   int filenameAddr = machine->ReadRegister(4);
   char filename[256];
   int value;
   int i = 0;
   
   // read from user memory
   do {
      machine->ReadMem(filenameAddr + i, 1, &value);
      filename[i] = (char)value;
   } while (filename[i++] != '\0');

   // open executable
   OpenFile* executable = fileSystem->Open(filename);
   if (executable == NULL) {
      printf("Unable to open file %s\n", filename);
      machine->WriteRegister(2, -1);
      return;
   }

   // set a space address for a new thread
   AddrSpace* space = new AddrSpace(executable);
   delete executable;

   // create a new thread with the recently made space address
   Thread* newThread = new Thread("user program");
   newThread->space = space;

   // add created thread to the Table
   int id = threadTable->addThread(newThread);
   newThread->setID(id);
   
   // assign to child thread and execute it
   newThread->Fork(StartExecProcess, NULL);
   // write again the id
   machine->WriteRegister(2, newThread->getID());
   AdvancePC();
   return;
}


/*
 *  System call interface: int Join( SpaceId )
 */
void NachOS_Join() {		// System call 3
   DEBUG('u', "Join system call\n");

   // read id of thread in memory
   int childTid = machine->ReadRegister(4);
   Thread* child = threadTable->getThread(childTid); // get the thread from

    if (child == nullptr) {
        printf("Join failed: thread %d not found\n", childTid);
        machine->WriteRegister(2, -1);
        return;
    }

    child->Join(childTid); // block until child finishes

    int status = child->getExitStatus(); 
    machine->WriteRegister(2, status);
    AdvancePC();
   return;
}


/*
 *  System call interface: void Create( char * )
 */
void NachOS_Create() {		// System call 4

}


/*
 *  System call interface: OpenFileId Open( char * )
 */
void NachOS_Open() {		// System call 5
   int addr = machine->ReadRegister(4);

   char filename[256];
   int ch;
   int i = 0;

   do {
      if (!machine->ReadMem(addr + i, 1, &ch)) {
         printf("Error while reading user file\n");
         machine->WriteRegister(2, -1);  // Error
         AdvancePC();
         return;
      }
      filename[i] = (char)ch;
      i++;
   } while (filename[i - 1] != '\0' && i < 256);


   OpenFile *file = fileSystem->Open(filename);
   if (file == nullptr) {
      printf("File not found: %s\n", filename);
      machine->WriteRegister(2, -1);  // Error
      AdvancePC();
      return;
   }
   int unixHandle = open(filename, O_RDWR); 
    if (unixHandle == -1) {
        machine->WriteRegister(2, -1);
        AdvancePC();
        return;
    }

   int nachosHandle = openFileTable->Open(unixHandle);
    if (nachosHandle == -1) {
        close(unixHandle);  // No space in table
        machine->WriteRegister(2, -1);
    } else {
        machine->WriteRegister(2, nachosHandle);
    }

    AdvancePC();
    return;
}


/*
 *  System call interface: OpenFileId Write( char *, int, OpenFileId )
 */
void NachOS_Write() {		// System call 6
   int addr = machine->ReadRegister( 4 );    // where to start from
   int size = machine->ReadRegister( 5 );    // how many spaces to move
   OpenFileId NachosHandle = machine->ReadRegister( 6 );
   char*  buffer = new char[ size +1];


   DEBUG('u', "Write system call\n");
   int end = 0;
   for (int i = 0; i < size; ++i) {
      int temp;
      if (machine->ReadMem(addr + i, 1, &temp)) {
         buffer[i] = (char)temp;
      } else {
         end = 1;
         break;
      }
   }
   if (end == 0) {
      // Write in terminal
      consoleSem->P();
      if (NachosHandle == ConsoleOutput) {
         buffer[size] = 0;
         // browse fro given size the amount of characters given in size
         //for (int i = 0; i < size; ++i) {
            printf("%s", buffer);
         //}
         delete[] buffer;
         fflush(stdout);
         machine->WriteRegister(2, size);

      } else if (NachosHandle == ConsoleInput) { // store in register
      
         machine->WriteRegister( 2, -1 );

      } else if (NachosHandle == ConsoleError) { // an error appeared
      
         printf( "%d\n", machine->ReadRegister( 4 ) );

      } else {
         // verify if file is indeed open
         
         int unixHandle = openFileTable->getUnixHandle(NachosHandle);
         if (unixHandle == -1) {
            machine->WriteRegister(2, -1);
            
         } else {
            int writtenB = write(unixHandle, buffer, size);
            // return either the failure sign or the amount of bytes written to user
            if (writtenB < 0) {
               machine->WriteRegister( 2, -1 );
            } else {
               machine->WriteRegister( 2, writtenB);
            }     
         }
         
         //return; 
      } 
      consoleSem->V();  
   }
    
   AdvancePC();
   return;
}


/*
 *  System call interface: OpenFileId Read( char *, int, OpenFileId )
 */
void NachOS_Read() {		// System call 7
   int addr = machine->ReadRegister( 4 );    // where to start from
   int size = machine->ReadRegister( 5 );    // how many spaces to move
   OpenFileId NachosHandle = machine->ReadRegister( 6 );
   char buffer[size];
   if (NachosHandle == ConsoleInput) {
      int readB = 0;
      char inputBuffer[32];  // large temp buffer
      
      if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) {
         machine->WriteRegister(2, -1);
         
      } else {
         // Copy up to 'size' bytes or until newline or null
         for (int i = 0; i < size && inputBuffer[i] != '\0' && inputBuffer[i] != '\n'; i++) {
            buffer[i] = inputBuffer[i];
            readB++;
         }
         int error0 = 0;
         // Write to user space
         for (int i = 0; i < readB; i++) {
            if (!machine->WriteMem(addr + i, 1, buffer[i])) {
               machine->WriteRegister(2, -1);
               error0 = 1;
            }
         }
         if (error0 == 0) {
             machine->WriteRegister(2, readB);
         }  
      }
    
   } else { // for all other open files
      int unixHandle = openFileTable->getUnixHandle(NachosHandle);
      if (unixHandle == -1) {
         machine->WriteRegister(2, -1);  // Invalid 
      } else {
         int readB = read(unixHandle, buffer, size);
   
         if (readB > 0) {
            int error = 0;
            // Write to user memory
            for (int i = 0; i < readB; i++) {
               if (!machine->WriteMem(addr + i, 1, buffer[i])) {
                  machine->WriteRegister(2, -1);
                  error = 1;
                  break;
               } 
            }
            if (error == 0) {
               // either write to user the fail sign or the amount of read bytes    
            machine->WriteRegister(2, readB);  
            }     
         } else {
            machine->WriteRegister(2, -1);  // Error reading
         }   
      }   
   }
   AdvancePC();
   return;
}


/*
 *  System call interface: void Close( OpenFileId )
 */
void NachOS_Close() {		// System call 8
   int NachosHandle = machine->ReadRegister(6);
   if (NachosHandle == ConsoleInput || NachosHandle == ConsoleOutput) {
      // just in case, but closing the console does not happen
      machine->WriteRegister(2, 0);
      
   } else {
      // get the Unix id from Table
      int unixHandle = openFileTable->getUnixHandle(NachosHandle);
      if (unixHandle == -1) {
         machine->WriteRegister(2, -1); // invalid FD

      } else {
         // if file descriptor was valid, then it was indeed an open file
         int result = close(unixHandle);
         if (result == -1) {
             AdvancePC();
            machine->WriteRegister(2, -1); // could not close
         } else {
            int i = openFileTable->Close(NachosHandle);
            // write either failure (-1) or success (0)
            machine->WriteRegister(2, i == 0 ? 0 : -1);    
         }   
      }
   }
   AdvancePC();
   return;
}

/**
 * Auxiliary function for Fork()
 */
static void ForkAux(void* arg) {
    int funcAddr = (intptr_t)arg;
    // reset all program counters to zero      
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    // notice that each thread should have three program counters
    machine->WriteRegister(RetAddrReg, 4 );
    machine->WriteRegister(PCReg, funcAddr);
    machine->WriteRegister(NextPCReg, funcAddr + 4);
    
    machine->Run();
  
}

/*
 *  System call interface: void Fork( void (*func)() )
 */
void NachOS_Fork() {	// System call 9
   Thread* child = new Thread("child thread");
   // share address space with parent
   child->space = currentThread->space;
   threadTable->addThread(child);
   // read function value to fork stored in registers
   intptr_t funcAddr = (intptr_t) machine->ReadRegister(4);
   child->Fork(ForkAux, (void*) funcAddr);
   AdvancePC();
   return;
}


/*
 *  System call interface: void Yield()
 */
void NachOS_Yield() {		// System call 10
   currentThread->Yield();
    AdvancePC();
   return;
}


/*
 *  System call interface: Sem_t SemCreate( int )
 */
void NachOS_SemCreate() {		// System call 11
   int initial = machine->ReadRegister(4);
   if (semaphore != nullptr) delete semaphore;
   semaphore = new Semaphore("inidivual_sem", initial);
   machine->WriteRegister(2,0);
   AdvancePC();
   return;
}


/*
 *  System call interface: int SemDestroy( Sem_t )
 */
void NachOS_SemDestroy() {		// System call 12
   int id = machine->ReadRegister(4); // ignored
    if (semaphore != nullptr) {
        delete semaphore;
        semaphore = nullptr;
    }
    AdvancePC();
    return;
}


/*
 *  System call interface: int SemSignal( Sem_t )
 */
void NachOS_SemSignal() {		// System call 13
   int id = machine->ReadRegister(4); // ignored in this case
    if (semaphore != nullptr) {
      semaphore->V();
   }
    AdvancePC();
    return;
   
}


/*
 *  System call interface: int SemWait( Sem_t )
 */
void NachOS_SemWait() {		// System call 14
   int id = machine->ReadRegister(4);
   if (semaphore != nullptr) {
      semaphore->P();
   }
   AdvancePC();
   return;
}


/*
 *  System call interface: Lock_t LockCreate( int )
 */
void NachOS_LockCreate() {		// System call 15
}


/*
 *  System call interface: int LockDestroy( Lock_t )
 */
void NachOS_LockDestroy() {		// System call 16
}


/*
 *  System call interface: int LockAcquire( Lock_t )
 */
void NachOS_LockAcquire() {		// System call 17
}


/*
 *  System call interface: int LockRelease( Lock_t )
 */
void NachOS_LockRelease() {		// System call 18
}


/*
 *  System call interface: Cond_t LockCreate( int )
 */
void NachOS_CondCreate() {		// System call 19
}


/*
 *  System call interface: int CondDestroy( Cond_t )
 */
void NachOS_CondDestroy() {		// System call 20
}


/*
 *  System call interface: int CondSignal( Cond_t )
 */
void NachOS_CondSignal() {		// System call 21
}


/*
 *  System call interface: int CondWait( Cond_t )
 */
void NachOS_CondWait() {		// System call 22
}


/*
 *  System call interface: int CondBroadcast( Cond_t )
 */
void NachOS_CondBroadcast() {		// System call 23
}


/*
 *  System call interface: Socket_t Socket( int, int )
 */
void NachOS_Socket() {			// System call 30

   int domain = machine->ReadRegister(4); // arg1
   int type = machine->ReadRegister(5);   // arg2
   
   // convert registers form of NachOS into Unix
   domain = (domain == 0) ? AF_INET : AF_INET6;
   type = (type == 0 )? SOCK_STREAM: SOCK_DGRAM;
   int UnixId = socket(domain, type, 0);

   if (UnixId < 0) {
      machine->WriteRegister(2, -1); // error
       AdvancePC();
      return;
   }
   
   // convert that UNIX id into NachOS handle to write it in registers
   int nachosHandle = openFileTable->Open(UnixId);
   
   machine->WriteRegister(2, nachosHandle);
   AdvancePC();
   return;
}


/*
 *  System call interface: Socket_t Connect( char *, int )
 */
void NachOS_Connect() {		// System call 31
   
   int nachosHandle = machine->ReadRegister(4); // socket file descr of NachOS
   int addressP = machine->ReadRegister(5);  //  IP P
   int port = machine->ReadRegister(6);   // port

   char ipAddr[32]; 
   int i = 0, ch = -1;
   // get ip Address from memory
   while (ch != '\0' && i < 32){
      machine->ReadMem(addressP + i, 1, &ch);
      
      ipAddr[i] = (char)ch;
      i++;
   }


   // get conversion of Nachos handle with table
   int unixHandle = openFileTable->getUnixHandle(nachosHandle);
   if (unixHandle < 0) { // invalid
      machine->WriteRegister(2, -1);
       AdvancePC();
      return;
   }
   // set general attributes for socket
   struct sockaddr_in serverAddr;
   memset(&serverAddr, 0, sizeof(serverAddr));
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_port = htons(port);
   if (inet_aton(ipAddr, &serverAddr.sin_addr) == 0) {
      machine->WriteRegister(2, -1); 
       AdvancePC();
      return;
   }
   // connect the UNIX conversion FD
   int result = connect(unixHandle, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
   machine->WriteRegister(2, result == 0 ? 0 : -1);
   AdvancePC();
   return;
}


/*
 *  System call interface: int Bind( Socket_t, int )
 */
void NachOS_Bind() {		// System call 32
}


/*
 *  System call interface: int Listen( Socket_t, int )
 */
void NachOS_Listen() {		// System call 33
}


/*
 *  System call interface: int Accept( Socket_t )
 */
void NachOS_Accept() {		// System call 34
}


/*
 *  System call interface: int Shutdown( Socket_t, int )
 */
void NachOS_Shutdown() {	// System call 25
}


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

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch ( which ) {

       case SyscallException:
          switch ( type ) {
             case SC_Halt:		// System call # 0
                NachOS_Halt();
                break;
             case SC_Exit:		// System call # 1
                NachOS_Exit();
                break;
             case SC_Exec:		// System call # 2
                NachOS_Exec();
                break;
             case SC_Join:		// System call # 3
                NachOS_Join();
                break;

             case SC_Create:		// System call # 4
                NachOS_Create();
                break;
             case SC_Open:		// System call # 5
                NachOS_Open();
                break;
             case SC_Read:		// System call # 6
                NachOS_Read();
                break;
             case SC_Write:		// System call # 7
                NachOS_Write();
                break;
             case SC_Close:		// System call # 8
                NachOS_Close();
                break;

             case SC_Fork:		// System call # 9
                NachOS_Fork();
                break;
             case SC_Yield:		// System call # 10
                NachOS_Yield();
                break;

             case SC_SemCreate:         // System call # 11
                NachOS_SemCreate();
                break;
             case SC_SemDestroy:        // System call # 12
                NachOS_SemDestroy();
                break;
             case SC_SemSignal:         // System call # 13
                NachOS_SemSignal();
                break;
             case SC_SemWait:           // System call # 14
                NachOS_SemWait();
                break;

             case SC_LckCreate:         // System call # 15
                NachOS_LockCreate();
                break;
             case SC_LckDestroy:        // System call # 16
                NachOS_LockDestroy();
                break;
             case SC_LckAcquire:         // System call # 17
                NachOS_LockAcquire();
                break;
             case SC_LckRelease:           // System call # 18
                NachOS_LockRelease();
                break;

             case SC_CondCreate:         // System call # 19
                NachOS_CondCreate();
                break;
             case SC_CondDestroy:        // System call # 20
                NachOS_CondDestroy();
                break;
             case SC_CondSignal:         // System call # 21
                NachOS_CondSignal();
                break;
             case SC_CondWait:           // System call # 22
                NachOS_CondWait();
                break;
             case SC_CondBroadcast:           // System call # 23
                NachOS_CondBroadcast();
                break;

             case SC_Socket:	// System call # 30
		         NachOS_Socket();
               break;
             case SC_Connect:	// System call # 31
		         NachOS_Connect();
               break;
             case SC_Bind:	// System call # 32
		         NachOS_Bind();
               break;
             case SC_Listen:	// System call # 33
		         NachOS_Listen();
               break;
             case SC_Accept:	// System call # 32
		         NachOS_Accept();
               break;
             case SC_Shutdown:	// System call # 33
		         NachOS_Shutdown();
               break;

             default:
                printf("Unexpected syscall exception %d\n", type );
                ASSERT( false );
                break;
          }
          
        // machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
        // machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
        // machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg)+4);
         break;
       case PageFaultException: {
          break;
       }

       case ReadOnlyException:
          printf( "Read Only exception (%d)\n", which );
          ASSERT( false );
          break;

       case BusErrorException:
          printf( "Bus error exception (%d)\n", which );
          ASSERT( false );
          break;

       case AddressErrorException:
          printf( "Address error exception (%d)\n", which );
          ASSERT( false );
          break;

       case OverflowException:
          printf( "Overflow exception (%d)\n", which );
          ASSERT( false );
          break;

       case IllegalInstrException:
          printf( "Ilegal instruction exception (%d)\n", which );
          ASSERT( false );
          break;

       default:
          printf( "Unexpected exception %d\n", which );
          ASSERT( false );
          break;
    }

}