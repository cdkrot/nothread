.section .text
        .globl NOTHREAD_IMPL_deep_bootstrap
        .globl NOTHREAD_IMPL_switch_context

// expected bootstrap protype: bootstrap(func_t* func, void* args, exec_context* pcontext)
// [rdi, rsi, rdx]

        
// extracts arguments for bootstrap from stack and hands execution there.
// expected stack: [pbootstrap, func, args, ctx] (TAIL), no args via registers.
NOTHREAD_IMPL_deep_bootstrap:
        pop %rdx
        pop %rsi
        pop %rdi
        pop %rcx
	mov %rsp, %rbp

	push %rbp
	push %rbp
	
	mov %rsp, %rbp
        jmp *%rcx

// void switch_context(void* self, void* target)
// self=%rdi, target=%rsi
NOTHREAD_IMPL_switch_context:
        push %rbp
	mov  %rsp, %rbp
	push %rbx
	push %r12
	push %r13
	push %r14
	push %r15

	leaq switch_context_return(%rip), %rax
	movq %rax, 0x0(%rdi) /* save to self->p_ret */
        movq %rsp, 0x8(%rdi) /* save to self->p_stack */

        movq 0x18(%rsi), %rax
        movq %rdi, 0x0(%rax) /* save to *(target->p_caller_context) */
        
        movq 0x8(%rsi), %rsp /* restore target->p_stack */
        movq 0x0(%rsi), %rcx /* load    target->p_ret */
        jmp *%rcx
        
switch_context_return:
        pop %r15
        pop %r14
        pop %r13
        pop %r12
        pop %rbx
        pop %rbp
        retq

/* No need for executable stack.  */
.section .note.GNU-stack,"",%progbits
