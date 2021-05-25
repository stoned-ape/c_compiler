#ifndef symstack_h
#define symstack_h

#include <vector>
#include <iostream>
using namespace std;


//used to store a variables name and its position on the stack
//relative to the base pointer
struct localvar{
    string name;
    int bp,offset;
    localvar(string n):name(n),offset(0),bp(0){}
};


//the symbol stack
//used to store the names and relative addresses of local variables
//allows you to push and pop stack frames as well as individual variables
//stack frames allow groups of variables to go in and out of scope
struct symstack{
    int sp,bp;      //we need a stack pointer and a base pointer to emulate the call stack
    vector<localvar> v;
    symstack():sp(0),bp(0){}
    void push(localvar x);
    localvar pop();
    void push_frame();
    void pop_frame();
    localvar operator[](int i);
    localvar find(string n);
    int find_idx(string n);
    
};

//print the symbolic stack
ostream &operator<<(ostream &os,symstack st);

#endif
