global _start

%include "macro.inc"

%define pc r15		; command counter
%define w r14		; current instruction
%define rstack r13	; return stack

section .text

%include "kernel.inc"
%include "util-words.inc"
%include "interpreter.inc"
%include "lib.inc"

section .bss

resq 1023		; return stack end
rstack_start: resq 1	; return stack start

user_mem: resq 65536	; data for user

section .data

last_word: dq _lw	; last word in the dictionary
dp: dq user_mem		; user data pointer
stack_start: dq 0	; stack address

section .text

_start:
	mov rstack, rstack_start
	mov [stack_start], rsp
	mov pc, forth_init

next:
	mov w, pc
	add pc, 8
	mov w, [w]
	jmp [w]
