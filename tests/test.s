    .file   "test.c"
    .text
    .global fun1 
    .type fun1, @function

fun1:
.LFB0:
    .cfi_startproc
    pushq %rbp
    .cfi_def_cfa_offset 16
	.cfi_offset 6, -16
    moveq %rsp, %rbp
    .cfi_def_cfa_register 6

    movl $123, %rbp
    movl %rbp, %eax

    leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc

.LFE0:
    .size   .fun1,  .-fun1

    .global main
    .type main, @function

main:
.LFB1:
    .cfi_startproc
    pushq %rbp
    .cfi_def_cfa_offset 16
	.cfi_offset 6, -16
    moveq %rsp, %rbp
    .cfi_def_cfa_register 6

    call fun1

	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc

.LFE1:
    .size   .main,  .-main

    .ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.12) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits