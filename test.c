//this file is for testing the compiler

/*
 multi line comments allowed
 */

int fac1(int n){
    if(n<=1) return 1;
    return n*fac1(n-1);
};

int fac2(int n){
    int f=1;
    for(int i=1;i<=n;i+=1) f*=i;
    return f;
}

int fib1(int n){
    if(n<=1) return 1;      /*comment*/
    return fib1(n-1)+fib1(n-2);
};

int fib2(int n){
    int x=1;
    int y=1;
    int z=2;
    for(int i=0;i<n;i+=1){
        z=x+y;
        x=y;
        y=z;
    }
    return x;
}

int pow(int a,int b){
    int x=1;
    for(int i=0;i<b;i+=1){ //for loops allowed
        x*=a;
    }
    return x;
}

//get the output from the exit code
int main(){
    return fac2(5);
}






