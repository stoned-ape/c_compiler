


#ifndef parser_h
#define parser_h

#include "lexer.h"

//helper functions for expression parsing
bool is_enclosed(token *a,int n);

int split_by_all(token *a,int n);

extern string token_type_names[];

bool is_func_call(token *a,int n);
bool is_func_def(token *a,int n);

string p_wrap(string s);

int op_prec(string op);



//binary abtract syntax tree node
struct exp_node{
    exp_node *left,*right;  //two child nodes
    string lexeme,type_str;
    token_type type;
    bool isdec,scope; //if type=vname or fname: isdec indecates whether the name is being declared
    int offset; //if type=vname: this repersents the variables offset from the base ptr
    int vars_sz; //if scope: this stores the number of local variables defined in that scope
    exp_node();
    exp_node(token *a,int n); //construct an AST for a c expression from an array of tokens
    void print(int n=0);
    void verbose_print(int n=0);
    string add_parens(exp_node *c,string s);
    string stringify(bool is_block=true);
};

//helper functions for block parsing
bool is_basic_block(token *a,int n);

bool is_enclosed_block(token *a,int n);


int split_block_by(token *a,int n,string kw);

//print an array of token
void lex_print(token *a,int n);

//construct a subtree for storing the parameters of a for loop
exp_node *loop_parameter_tree(exp_node *init,exp_node *com,exp_node *inc);

int for_param_helper(token *d,int rem);

struct block_node:exp_node{
    block_node(token *a,int n); //construct an AST for a c block from an array of tokens
};

#endif
