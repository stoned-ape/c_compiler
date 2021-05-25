#include "parser.h"

#ifndef backend_h
#define backend_h

//communitive    : +,*,&,|,^,&&,||,==,!=
//non-communitive: -,/,>>,<<,<,>,>=,<=,=,+=,-=,/=


//assembly generating functions
string inst(string in,string a1,string a2);

string stack_wrap(string s,string reg);

string sub(string src,string dest,bool dms);

string divmod(string src,string dest,bool dms,bool div);

string reg_swap(string r1,string r2);

string shift(string src,string dest,bool dms,bool shl);

string comp_eq(string src,string dest,bool eq);

string comp_ltgt(string src,string dest,bool dms,bool gt,bool eq);

string bool_and_or(string src,string dest,bool isand);

string scope_wrap(string s,int vars_sz=0);

string assign(string reg);

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

//encodes the size of stack frames and the relative addresses of
//local variables into the abstract syntax tree
void rec_stack_info(exp_node *node,symstack *st);
void stack_info(exp_node *node);


//converts and AST into assembly code
string backend(exp_node *node,bool left=false,string tag="tag_");

#endif
