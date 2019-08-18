# Code-Sample-1---Thread-Scheduling
Sample code for showing to employers. Part of a larger project to write a 32 bit operating system kernel. This is the code for thread scheduling. Note that elsewhere in the source code I have set up the interrupt descriptor table and programmed the PIC's timer interrupt as the primary driver that determines when a thread will context switch. This scheduler runs in round robin format via the timer interrupts. 

The interrupt handler for the timer tells the PIC that it will handle the interrupt, and then calls the scheduler to perform the context switch into the next waiting thread.

I wish to keep the rest of the source code for this project in a private repository, but will share it personally apon request from a potential employer.
