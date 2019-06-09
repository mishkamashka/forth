;section .bss
;rs:             resq 65536
;u_mem:          resq 65536
;dict: 		resq 65536
;word_buffer:    resb 1024
;state:		resq 1	

section .text

; work with stack

native "drop", drop
    	pop rax
    	xor rax, rax
    	jmp next

native "swap", swap
    	pop rax
    	pop rdx
    	push rax
    	push rdx
    	jmp next

native "dup", dup
    	push qword [rsp]
    	jmp next

; ariphmetic operators

native "+", plus
    	pop rdx
    	pop rax
    	add rax, rdx
    	push rax
    	jmp next

native "-", minus
    	pop rdx
    	pop rax
    	sub rax, rdx
    	push rax
    	jmp next
  
native "*", mul
    	pop rdx
    	pop rax
    	imul rdx
    	push rax
    	jmp next

native "/", div
  	 pop rcx
  	 pop rax
  	 cqo
  	 idiv rcx
  	 push rax
   	 jmp next

native "%", mod
    	pop rcx
    	pop rax
   	cqo
    	idiv rcx
	push rdx
    	jmp next

native "=", equal
  	pop rax
	pop rcx
	cmp rax, rcx
   	jz .eq
    	push 0
   	jmp next
.eq:
    	push 1
    	jmp next

native "<", ls
    	pop rax
    	pop rcx
    	cmp rcx, rax
    	jl .less
    	push 0
    	jmp next
.less:
    	push 1
    	jmp next    
    
    
; logic operators

native "not", not
	pop rax
    	test rax, rax
    	setz al
    	movzx rax, al
    	push rax
    	jmp next

native "and", and
    	pop rax
    	and [rsp], rax
    	jmp next

native "or", or
    	pop rax
    	or [rsp], rax
    	jmp next

native "land", land
    	pop rax
    	pop rcx
    	test rax, rax
    	jz .no
    	push rcx
    	jmp next
.no:
    	push rax
    	jmp next

native "lor", lor
    	pop rax
    	pop rcx
    	test rax, rax
    	jnz .yes
    	push rcx
    	jmp next
.yes:
    	push rax
    	jmp next

; return stack

native ">r", to_return_stack
    	pop rax
	rpush rax
    	jmp next

native "r>", from_return_stack
    	rpop rax
    	push rax
    	jmp next

native "r@", cp_from_rstack
    	push qword [rstack]
    	jmp next
    
; memory operators

native "@", read
    	pop rax
    	push qword [rax]
    	jmp next

native "!", write
    	pop rax
    	pop rdx
    	mov [rax], rdx
    	jmp next

native "c!", write_byte
    	pop rax
    	pop rdx
    	mov [rax], dl
    	jmp next

native "c@", read_byte
    	pop rax
    	movzx rax, byte [rax]
    	push rax
    	jmp next

native "execute", execute
    	pop rax
    	mov w, rax
    	jmp [rax]

native "foth-dp", forth_dp
	push dp
	jmp next

; execution management

native "docol", docol
	rpush pc
	add w, 8
	mov pc, w
        jmp next

native "branch", branch
    	mov pc, [pc]
    	jmp next

native "0branch", branch0
    	pop rax
    	test rax, rax
    	jnz .skip
    	mov pc, [pc]
    	jmp next
.skip:
    	add pc, 8
    	jmp next

native "exit", exit
	rpop pc
	jmp next

; other

native "lit", lit
    	push qword [pc]
    	add pc, 8
    	jmp next
    
native "sp", sp
    	push rsp
    	jmp next
        
native "bye", bye
    	mov rax, 60
    	xor rdi, rdi
    	syscall

native ".S", show
    	mov rax, rsp
.loop:
    	cmp rax, [rsp_b]
    	jz next
    	mov rdi, [rax]
    	push rax
    	call print_int
    	call print_newline
    	pop rax
    	add rax, 8
    	jmp .loop

native "forth-stack-restore", forth_stack_restore
	mov rsp, [stack_start]
	jmp next

native "foth-stack-base", forth_stack_base
	push qword[stack_start]
	jmp next

native "syscall", syscall
	pop r9
	pop r8
	pop r10
	pop rdx
	pop rsi
	pop rdi
	pop rax
	syscall
	push rax
	push rdx
	jmp next
