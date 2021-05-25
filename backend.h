#include "parser.h"
#include "symstack.h"

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


//encodes the size of stack frames and the relative addresses of
//local variables into the abstract syntax tree
void rec_stack_info(exp_node *node,symstack *st);
void stack_info(exp_node *node);

vector<exp_node*> extract_func_defs(exp_node *node);

//converts and AST into assembly code
string backend(exp_node *node,bool left=false,string tag="tag_",string func_name="");

#endif
