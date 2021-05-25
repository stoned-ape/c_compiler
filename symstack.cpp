
#include "symstack.h"


//symbol stack utilities
void symstack::push(localvar x){
    x.bp=bp;
    x.offset=sp-bp;
    v.push_back(x);
    sp++;
}
localvar symstack::pop(){
    assert(sp>0);
    localvar x=v[sp-1];
    sp--;
    return x;
}
void symstack::push_frame(){
    auto x=localvar("*");
    x.bp=bp;
    push(x);
    bp=sp;
}
void symstack::pop_frame(){
    assert(bp>0);
    sp=bp;
    auto x=pop();
    bp=x.bp;
    assert(x.name=="*");
}
localvar symstack::operator[](int i){
    assert(i<sp);
    return v[i];
}
//used to find the address of a variable
localvar symstack::find(string n){
    for(int i=0;i<sp;i++){
        auto x=v[i];
        if(x.name==n) return x;
    }
    cout<<"variable "<<n<<" used before it was defined\n";
    throw(0);
    return localvar("*");
}
int symstack::find_idx(string n){
    for(int i=0;i<sp;i++){
        auto x=v[i];
        if(x.name==n) return i;
    }
    cout<<"variable "<<n<<" used before it was defined\n";
    throw(0);
    return -1;
}

//print the symbol stack
ostream &operator<<(ostream &os,symstack st){
    for(int i=0;i<st.sp;i++){
        auto x=st.v[i];
        os<<x.name<<"  ";
    }
    os<<endl;
    return os;
}
