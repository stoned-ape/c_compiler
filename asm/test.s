

//int
.global _add // +
.global _sub // -
.global _mul // *
.global _div // /
.global _mod // %
 
.global _and // &
.global _orr // |
.global _xor // ^
.global _shl // <<
.global _shr // >>

.global _gt  // >
.global _lt  // <
.global _gte // >=
.global _lte // <=
.global _eq  // ==
.global _neq // !=
 
.global _ban // &&
.global _bor // ||
 
.global _addeq // +=
.global _subeq // -=
.global _muleq // *=
.global _diveq // /=
.global _modeq // %=
 
.global _andeq // &=
.global _orreq // |=
.global _xoreq // ^=
.global _shleq // >>=
.global _shreq // <<=

.global _asn // =

//float
.global _fadd // +
.global _fsub // -
.global _fmul // *
.global _fdiv // /
.global _fmod // %

.global _fgt  // >
.global _flt  // <
.global _fgte // >=
.global _flte // <=
.global _feq  // ==
.global _fneq // !=
 
.global _fban // &&
.global _fbor // ||

.global _faddeq // +=
.global _fsubeq // -=
.global _fmuleq // *=
.global _fdiveq // /=

.global _fasn // =

//string ordered_ops[]={
//    ",","=",">>=","<<=","+=","-=","*=","/=","%=","&=","|=",
//    "^=","||","&&","|","^","&","==","!=","<=",">=","<",">>",
//    "<<",">","+","-","%","/","*"
//};

//long *operator(long *rdi,long *rsi);

.global bruh@

bruh@:

//arithmatic
_add:
    movq (%rdi),%rax //load first argument into rax
    addq (%rsi),%rax //add second argument to rax
    ret              //result is returned in rax

_sub:
    movq (%rdi),%rax
    subq (%rsi),%rax
    ret

_mul:
    movq (%rdi),%rax
    imulq (%rsi),%rax
    ret

_div:
    movq (%rdi),%rax
    movq (%rsi),%r9
    cqo              //sign extend rax into rdx because the dividend is rdx:rax
    idivq %r9        //dividend must be a register, unlike imul
    ret              //quotient is put in rax

_mod:
    movq (%rdi),%rax
    movq (%rsi),%r9
    cqo
    idivq %r9
    movq %rdx,%rax
    ret

//logical
_and:
    movq (%rdi),%rax
    andq (%rsi),%rax
    ret

_orr:
    movq (%rdi),%rax
    orq (%rsi),%rax
    ret

_xor:
    movq (%rdi),%rax
    xorq (%rsi),%rax
    ret

_shl:
    movq (%rdi),%rax
    movq (%rsi),%rcx  //variable shift amount must be stored in rcx
    salq %cl,%rax     //only use the bottom byte of rcx for shifting
    ret

_shr:
    movq (%rdi),%rax
    movq (%rsi),%rcx
    sarq %cl,%rax
    ret

//boolean
_gt:
    movq (%rdi),%r9
    movq $0,%rax      //rax=0;
    movq $1,%r10
    cmp (%rsi),%r9    //compare the two arguments
    cmovgq %r10,%rax  //if(*rdi>*rsi) rax=1;
    ret               //return rax;

_lt:
    movq (%rdi),%r9
    movq $0,%rax
    movq $1,%r10
    cmp (%rsi),%r9
    cmovlq %r10,%rax
    ret

_gte:
    movq (%rdi),%r9
    movq $0,%rax
    movq $1,%r10
    cmp (%rsi),%r9
    cmovgeq %r10,%rax
    ret

_lte:
    movq (%rdi),%r9
    movq $0,%rax
    movq $1,%r10
    cmp (%rsi),%r9
    cmovleq %r10,%rax
    ret

_eq:
    movq (%rdi),%r9
    movq $0,%rax
    movq $1,%r10
    cmp (%rsi),%r9
    cmoveq %r10,%rax
    ret

_neq:
    movq (%rdi),%r9
    movq $0,%rax
    movq $1,%r10
    cmp (%rsi),%r9
    cmovneq %r10,%rax
    ret
//boolean and "&&"
_ban:
    movq $1,%r10

    movq $0,%rax
    cmpq $0,(%rdi)
    cmovnz %r10,%rax  //anything other than 0 evaluates to true in C

    movq $0,%rbx
    cmpq $0,(%rsi)
    cmovnz %r10,%rbx

    andq %rbx,%rax    //bitwise AND is equivalent to boolean AND here
    ret
//boolean or "||"
_bor:
    movq $1,%r10

    movq $0,%rax
    cmpq $0,(%rdi)
    cmovnz %r10,%rax

    movq $0,%rbx
    cmpq $0,(%rsi)
    cmovnz %r10,%rbx

    orq %rbx,%rax
    ret



//assignment operators
//exactly the same as non assigning versions except for the "movq %rax,(%rdi)" line

//arithmatic assignment
_addeq:
    movq (%rdi),%rax
    addq (%rsi),%rax
    movq %rax,(%rdi)  //store the result in the first argument
    ret

_subeq:
    movq (%rdi),%rax
    subq (%rsi),%rax
    movq %rax,(%rdi)
    ret

_muleq:
    movq (%rdi),%rax
    imulq (%rsi),%rax
    movq %rax,(%rdi)
    ret

_diveq:
    movq (%rdi),%rax
    movq (%rsi),%rbx
    cqo
    idivq %rbx
    movq %rax,(%rdi)
    ret

_modeq:
    movq (%rdi),%rax
    movq (%rsi),%rbx
    cqo
    idivq %rbx
    movq %rdx,%rax
    movq %rax,(%rdi)
    ret

//logical assignment
_andeq:
    movq (%rdi),%rax
    andq (%rsi),%rax
    movq %rax,(%rdi)
    ret

_orreq:
    movq (%rdi),%rax
    orq (%rsi),%rax
    movq %rax,(%rdi)
    ret

_xoreq:
    movq (%rdi),%rax
    xorq (%rsi),%rax
    movq %rax,(%rdi)
    ret

_shleq:
    movq (%rdi),%rax
    movq (%rsi),%rcx
    salq %cl,%rax
    movq %rax,(%rdi)
    ret

_shreq:
    movq (%rdi),%rax
    movq (%rsi),%rcx
    sarq %cl,%rax
    movq %rax,(%rdi)
    ret

//normal assignment
_asn:
    movq (%rsi),%rax
    movq %rax,(%rdi)
    ret



//floating point arithmetic operators
//same as the integer versions except here we xmm* float registers
//and *sd float instructions
_fadd:
    movsd (%rdi),%xmm0
    addsd (%rsi),%xmm0
    ret                 //float results are returned in xmm0, not rax

_fsub:
    movsd (%rdi),%xmm0
    subsd (%rsi),%xmm0
    ret

_fmul:
    movsd (%rdi),%xmm0
    mulsd (%rsi),%xmm0
    ret

_fdiv: //much simpler than the integer version
    movsd (%rdi),%xmm0
    divsd (%rsi),%xmm0
    ret
    

//floating point boolean
_fgt:
    movsd (%rdi),%xmm0
    movq $0,%rax
    movq $1,%r10
    comisd (%rsi),%xmm0 //floating point comparisons set different flags
    cmovaq %r10,%rax    //so we must use cmovaq instead of cmovgq
    ret

_flt:
    movsd (%rdi),%xmm0
    movq $0,%rax
    movq $1,%r10
    comisd (%rsi),%xmm0
    cmovbq %r10,%rax
    ret

_fgte:
    movsd (%rdi),%xmm0
    movq $0,%rax
    movq $1,%r10
    comisd (%rsi),%xmm0
    cmovaeq %r10,%rax
    ret

_flte:
    movsd (%rdi),%xmm0
    movq $0,%rax
    movq $1,%r10
    comisd (%rsi),%xmm0
    cmovbeq %r10,%rax
    ret

_feq:
    movsd (%rdi),%xmm0
    movq $0,%rax
    movq $1,%r10
    comisd (%rsi),%xmm0
    cmoveq %r10,%rax
    ret

_fneq:
    movsd (%rdi),%xmm0
    movq $0,%rax
    movq $1,%r10
    comisd (%rsi),%xmm0
    cmovneq %r10,%rax
    ret

_fban:
    movq $1,%r10

    movq $0,%rax
    cvtsi2sdq %rax,%xmm0  //convert integer zero into float zero
    comisd (%rdi),%xmm0   //float must be compared with float /no immediates
    cmovnz %r10,%rax

    movq $0,%rbx
    comisd (%rsi),%xmm0
    cmovnz %r10,%rbx

    andq %rbx,%rax
    ret

_fbor:
    movq $1,%r10

    movq $0,%rax
    cvtsi2sdq %rax,%xmm0
    comisd (%rdi),%xmm0
    cmovnz %r10,%rax

    movq $0,%rbx
    comisd (%rsi),%xmm0
    cmovnz %r10,%rbx

    orq %rbx,%rax
    ret

//floating point assigning arithmetic operators
//exactly the same as no assigning versions except for "movsd %xmm0,(%rdi)"
_faddeq:
    movsd (%rdi),%xmm0
    addsd (%rsi),%xmm0
    movsd %xmm0,(%rdi)
    ret

_fsubeq:
    movsd (%rdi),%xmm0
    subsd (%rsi),%xmm0
    movsd %xmm0,(%rdi)
    ret

_fmuleq:
    movsd (%rdi),%xmm0
    mulsd (%rsi),%xmm0
    movsd %xmm0,(%rdi)
    ret

_fdiveq:
    movsd (%rdi),%xmm0
    divsd (%rsi),%xmm0
    movsd %xmm0,(%rdi)
    ret

//normal assignment
_fasn:
    movsd (%rsi),%xmm0
    movq %xmm0,(%rdi)
    ret
    
