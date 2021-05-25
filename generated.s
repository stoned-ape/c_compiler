.section    __TEXT,__text,regular,pure_instructions
.build_version macos, 10, 15    sdk_version 10, 15, 6
.globl    _main                   ## -- Begin function main
.p2align    4, 0x90 
_main: 
push %rbp
movq $1,%rax
movq $32,%rbx
andq  %rbx,%rax
popq %rbp
retq
