.globl    _main                   ## -- Begin function main
.p2align    4, 0x90 
_main: 
	pushq %rbp //scope begin
	movq %rsp,%rbp
	subq $8,%rsp
	movq $0,%rax
	movq -8(%rbp),%rbx
	leaq -8(%rbp),%r10
	movq %rax,(%r10)
	movq $9,%rax
	pushq %r9
	pushq %rax
	movq $9,%rax
	pushq %r9
	pushq %rax
	movq $9,%rax
	pushq %r9
	pushq %rax
	movq $9,%rax
	pushq %r9
	pushq %rax
	movq $9,%rax
	pushq %r9
	pushq %rax
	movq $9,%rax
	pushq %r9
	pushq %rax
	movq $9,%rax
	movq $9,%rbx
	addq  %rax,%rbx
	popq %rax
	popq %r9
	addq  %rax,%rbx
	popq %rax
	popq %r9
	addq  %rax,%rbx
	popq %rax
	popq %r9
	addq  %rax,%rbx
	popq %rax
	popq %r9
	addq  %rax,%rbx
	popq %rax
	popq %r9
	addq  %rax,%rbx
	popq %rax
	popq %r9
	addq  %rbx,%rax
	movq -8(%rbp),%rbx
	leaq -8(%rbp),%r10
	movq %rax,(%r10)
	movq -8(%rbp),%rax
	leaq -8(%rbp),%r9
	addq $8,%rsp
	popq %rbp //scope end
	retq
