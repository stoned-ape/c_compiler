#include <stdio.h>

#define FUNC(name) extern long name(long *a,long *b);
#define FLOATFUNC(name) extern double name(double *a,double *b);
#define FLOATCOMPFUNC(name) extern long name(double *a,double *b);


//non assigning operators
FUNC(add);
FUNC(sub);
FUNC(mul);
FUNC(div);
FUNC(mod);
FUNC(and);
FUNC(orr);
FUNC(xor);
FUNC(shl);
FUNC(shr);
FUNC(gt );
FUNC(lt );
FUNC(gte);
FUNC(lte);
FUNC(eq );
FUNC(neq);
FUNC(ban);
FUNC(bor);

//assigning operators
FUNC(addeq);
FUNC(subeq);
FUNC(muleq);
FUNC(diveq);
FUNC(modeq);
FUNC(andeq);
FUNC(orreq);
FUNC(xoreq);
FUNC(shleq);
FUNC(shreq);

//normal assignment
FUNC(asn);

//floating point operators
FLOATFUNC(fadd);
FLOATFUNC(fsub);
FLOATFUNC(fmul);
FLOATFUNC(fdiv);

FLOATCOMPFUNC(fgt );
FLOATCOMPFUNC(flt );
FLOATCOMPFUNC(fgte);
FLOATCOMPFUNC(flte);
FLOATCOMPFUNC(feq );
FLOATCOMPFUNC(fneq);
FLOATCOMPFUNC(fban);
FLOATCOMPFUNC(fbor);

//float assigning operators
FLOATFUNC(faddeq);
FLOATFUNC(fsubeq);
FLOATFUNC(fmuleq);
FLOATFUNC(fdiveq);

//normal float assignment
FLOATFUNC(fasn);


long (*funcs[])(long*,long*)={
    add,sub,mul,div,mod,and,orr,xor,shl,shr,gt,lt,gte,lte,eq,neq,ban,bor
};

long (*eqfuncs[])(long*,long*)={
    addeq,subeq,muleq,diveq,modeq,andeq,orreq,xoreq,shleq,shreq
};

double (*floatfuncs[])(double*,double*)={
    fadd,fsub,fmul,fdiv
};

long (*floatcompfuncs[])(double*,double*)={
    fgt,flt,fgte,flte,feq,fneq,fban,fbor
};

double (*eqfloatfuncs[])(double*,double*)={
    faddeq,fsubeq,fmuleq,fdiveq
};


long test(long x,long y,int i){
    switch(i){
        case 0: return x+y;
        case 1: return x-y;
        case 2: return x*y;
        case 3: return x/y;
        case 4: return x%y;
        case 5: return x&y;
        case 6: return x|y;
        case 7: return x^y;
        case 8: return x<<y;
        case 9: return x>>y;
        case 10: return x>y;
        case 11: return x<y;
        case 12: return x>=y;
        case 13: return x<=y;
        case 14: return x==y;
        case 15: return x!=y;
        case 16: return x&&y;
        case 17: return x||y;
        default: printf("test error\n");
    }
    return 0;
}

double floattest(double x,double y,int i){
    switch(i){
        case 0: return x+y;
        case 1: return x-y;
        case 2: return x*y;
        case 3: return x/y;
        default: printf("test error\n");
    }
    return 0;
}
long floatcomptest(double x,double y,int i){
    switch(i){
        case 0: return x>y;
        case 1: return x<y;
        case 2: return x>=y;
        case 3: return x<=y;
        case 4: return x==y;
        case 5: return x!=y;
        case 6: return x&&y;
        case 7: return x||y;
        default: printf("test error\n");
    }
    return 0;
}


int main(){

    
    long n=128;
    long fn=sizeof(funcs)/8;
    printf("%lu\n",sizeof(funcs)/8);
    for(int i=0;i<fn;i++){
        for(long a=-n;a<n;a++){
            for(long b=-n;b<n;b++){
                if((i==3 || i==4) && b==0) continue; //dont divide by zero
                if(funcs[i](&a,&b)!=test(a,b,i))
                    printf("int error %ld %ld %ld %d \n",a,b,funcs[i](&a,&b),i);
            }
        }
    }
    printf("done with non assigning operators\n");
    
    fn=sizeof(eqfuncs)/8;
    printf("%lu\n",sizeof(eqfuncs)/8);
    for(int i=0;i<fn;i++){
        for(long a=-n;a<n;a++){
            for(long b=-n;b<n;b++){
                if((i==3 || i==4) && b==0) continue; //dont divide by zero
                long A=a; //value to be modified
                long res=eqfuncs[i](&A,&b);
                long t=test(a,b,i);
                if(res!=t || A!=t) printf("int assign error %d\n",i);
            }
        }
    }
    
    for(long b=-n;b<n;b++){
        long a,c; //value to be modified
        long res=asn(&a,&b);
        long t=(c=b);
        if(res!=t || a!=b) printf("error\n");
    }
    
    printf("done with assigning operators\n");
    
    
    fn=sizeof(floatfuncs)/8;
    printf("%lu\n",sizeof(floatfuncs)/8);
    for(int i=0;i<fn;i++){
        for(double a=-n;a<n;a++){
            for(double b=-n;b<n;b++){
                if(i==3 && b==0) continue; //dont divide by zero
                if(floatfuncs[i](&a,&b)!=floattest(a,b,i)) printf("float error\n");
            }
        }
    }
    printf("done with non assigning float operators\n");
    
    fn=sizeof(floatcompfuncs)/8;
    printf("%lu\n",sizeof(floatcompfuncs)/8);
    for(int i=0;i<fn;i++){
        for(double a=-n;a<n;a++){
            for(double b=-n;b<n;b++){
                if(floatcompfuncs[i](&a,&b)!=floatcomptest(a,b,i)) printf("float compare error\n");
            }
        }
    }
    printf("done with float comparison operators\n");
    
    fn=sizeof(eqfloatfuncs)/8;
    printf("%lu\n",sizeof(eqfloatfuncs)/8);
    for(int i=0;i<fn;i++){
        for(double a=-n;a<n;a++){
            for(double b=-n;b<n;b++){
                if(i==3 && b==0) continue; //dont divide by zero
                double A=a; //value to be modified
                double res=eqfloatfuncs[i](&A,&b);
                double t=floattest(a,b,i);
                if(res!=t || A!=t) printf("float assign error\n");
            }
        }
    }
    
    for(double b=-n;b<n;b++){
        double a,c; //value to be modified
        double res=fasn(&a,&b);
        double t=(c=b);
        if(res!=t || a!=b) printf("float assign error\n");
    }
    
    printf("done with assigning float operators\n");
}
