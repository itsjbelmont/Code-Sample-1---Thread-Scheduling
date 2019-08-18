# 	** Copyright Jack Belmont **
#
# 	Context switching assembly function
#

.text
.globl contextSwitchThreads
.align 0x4

# void contextSwitchThreads(uint32_t** oldStackPtr, uint32_t *newStackPtr) #
contextSwitchThreads:
	pushfl						#Push flags to old stack
	pushal						#Push all registers to old stack
	
	movl 40(%esp), %eax			#Get a pointer to address where we want to store the old stack pointer
	movl %esp, (%eax)			#Store old stack pointer 
	movl 44(%esp), %eax			#Get the new stack pointer
	movl %eax, %esp				#Load new stack pointer into ESP
	
	popal						#Pop registers from new stack
	popfl						#Pop flags from new stack
	ret							#Return into new thread
	
	