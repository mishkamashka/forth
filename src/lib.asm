global string_length
global print_newline
global print_char
global print_string
global print_uint
global print_int
global parse_int
global parse_uint
global string_equals
global read_char
global read_word
global string_copy
global in_fd

section .text

; rdi - first symbol of the string
; rax - return string length
string_length:
	mov rax, -1
	.loop:
		inc rax
		cmp byte[rdi + rax], 0
		jnz .loop
	ret

; rdi - first symbol of the string
print_string:
	call string_length
	mov rdx, rax	; string length
	mov rsi, rdi	; string pointer
	mov rax, 1	; write
	mov rdi, 1	; stdout
	syscall
	ret

; rdi - symbol
print_char:
	push rdi
	mov rdi, 1	; stdout
	mov rsi, rsp	; char pointer
	mov rax, 1	; write
	mov rdx, 1	; length
	syscall
	pop rdi
	ret

print_newline:
	mov rdi, 10
	call print_char
	ret

; rax - int to print
print_int:
	cmp rdi, 0
	jns print_uint	; if unsighed (positive) -> print_uint
	xor rdi, -1	; invert
	inc rdi		; +1
	push rdi
	mov rdi, '-'
	call print_char	; print '-'
	pop rdi

print_uint:
	mov rax, rdi
	mov r8, 10		; for division
	times 11 push word 0	; reserve place in stack for 20 symbols + 0x00	
	mov r9, rsp
	add rsp, 20
	.loop:
		xor rdx, rdx
		dec rsp
		div r8		; rax = (rdx rax)/r8, rdx = res
		add rdx, '0'	; add 30 to make ascii code
		mov byte [rsp], dl
		test rax, rax	; end of number?
		jnz .loop
        mov rdi, rsp
        call print_string
        mov rsp, r9
        add rsp, 22		; back to previous stack pointer
        ret

; rsi - first string pointer
; rdi - second string pointer
string_equals:
        push rdi
        push rsi
        call string_length
	pop rsi
	pop rdi
        mov r8, rax		; first string length
        mov rax, rdi
	mov rdi, rsi
	mov rsi, rax
	push rdi
	push rsi
	push r8
        call string_length
	pop r8
	pop rsi
	pop rdi
        mov rcx, rax		; second string length
	xor rax, rax
        cmp r8, rcx		; if lengths are not equal - exit
        jnz .exit
        test rcx, rcx		; if empty strings - return 1
        jz .emptystring
        .loop:
        	dec rcx
		mov r9b, byte[rdi + rcx]	; symbol from first string to r9b
		cmp r9b, byte[rsi + rcx]	; compare to symbol from second string
		jnz .exit
		test rcx, rcx			; end of strings -> exit
		jnz .loop
        .emptystring:
        	inc rax
        .exit:
        	ret

read_char:
        xor rax, rax
        push rax	; take place in stack with zero, not to override necessary data in stack, couse reads only in memory
        mov rdi, 0	; read
        mov rsi, rsp	; char pointer
        mov rdx, 1	; size
        syscall
        mov rax, [rsp]
        add rsp, 8
        ret

section .data
in_fd: times 256 db 0

section .text

; rdi - return word buffer
; rdx - word length
read_word:
        push r8
        mov r8, in_fd
        .skip:
		call read_char
		test rax, rax	; check if end
		jz .exit
		cmp rax, 32	; skip till first letter
		jle .skip
        .read:
		mov [r8], rax	; if letter move rax to word_buffer
		inc r8
		call read_char
		cmp rax, 32
		jg .read	; till space or lower
        .exit:
		mov rdi, in_fd
		call string_length
		mov rdx, rax
		mov rax, in_fd
	pop r8
	ret

; rdi - string pointer
; rax - return number
; rdx - return number length
parse_uint:
        xor rax, rax
	xor rdx, rdx
	xor rcx, rcx	; counter
	xor r11, r11	; buf
	mov r8, '0'	; to convert to ascii
	mov r9, '9'
	mov r10, 10	; to build a number
	.loop:
		mov r11b, byte[rdi, rcx]
		cmp r11b, r8b	; char - '0'
		js .break	; if sign (not a number) -> exit
		cmp r9b, r11b	; '9' - char
		js .break	; if sign (not a number)
		sub  r11, r8
		mul r10
		add rax, r11
		inc rcx
		test rax, rax
		jnz .loop
	.break:
	mov rdx, rcx
        ret

; rdi - string pointer
; rax - return string number
; rdx - return string length
parse_int:
        push r12
	xor r12, r12
	cmp byte[rdi], '-'	; if positive -> parse_uint
	jnz .positive
	inc rdi
	mov r12, -1
	.positive:
		call parse_uint
	xor rax, r12	; invert if negative
	sub rax, r12	; +1 if negative
	sub rdx, r12
	pop r12
	ret

; rdi - string pointer
; rsi - destination pointer
string_copy:
	.loop:
		xor rax, rax
		mov al, byte[rdi]
		mov byte[rsi], al
		inc rdi
		inc rsi
		test rax, rax
		jnz .loop
        ret
