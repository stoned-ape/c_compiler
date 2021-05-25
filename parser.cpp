#include "parser.h"

//returns true if the expression is wrapped in parantheses
bool is_enclosed(token *a,int n){
    //scan
    bool base=true;
    int p=0;
    for(int i=0;i<n;i++){
        if(a[i].lexeme=="(") p++;
        if(a[i].lexeme==")") p--;
        if(a[i].type==binop && p==0 && i!=0) base=false;
    }
    //invalid expression check
    if(p!=0){
        cout<<"unbalenced parentheses"<<endl;
        throw(0);
    }
    //null paranthesis check
    if(a[0].lexeme=="(" && a[n-1].lexeme==")" && base) return true;
    return false;
}


int split_by_operator(token *a,int n,string op,bool l2r){
    int p=0;
    if(l2r){  //scan from backwards for left to right associative operators
        for(int i=n-1;i>=0;i--){
            if(a[i].lexeme=="(") p++;
            if(a[i].lexeme==")") p--;
            if(a[i].lexeme==op && p==0) return i;
        }
    }else{    //and forwards for right to left associative operators
        for(int i=0;i<n;i++){
            if(a[i].lexeme=="(") p++;
            if(a[i].lexeme==")") p--;
            if(a[i].lexeme==op && p==0) return i;
        }
    }
    return -1;
}

//the order of this array is important
//it must be the reverse of the order of operations
string ordered_ops[]={
    ",","=",">>=","<<=","+=","-=","*=","/=","%=","&=","|=",
    "^=","||","&&","|","^","&","==","!=","<=",">=","<",">>",
    "<<",">","+","-","%","/","*"
};

//divide and conquor
//find the operator with the lowest precedence, split the array in half
//and recursively evalute the two halves
int split_by_all(token *a,int n){
    bool l2r=false;
    for(int i=0;i<sizeof ordered_ops;i++){
        if(i==12) l2r=true;
        int x=split_by_operator(a,n,ordered_ops[i],l2r);
        if(x!=-1) return x;
    }
    cout<<"no operators\n";
    throw(0);
    return -1;
}

extern string token_type_names[];
//matches a function call
bool is_func_call(token *a,int n){
    if(a[0].type!=fname) return false;
    return is_enclosed(a+1,n-1);
}
//matches a function definition
bool is_func_def(token *a,int n){
    if(a[0].type!=tname || a[1].type!=fname) return false;
    return is_enclosed(a+2,n-2);
}

string p_wrap(string s){return "("+s+")";}

int op_prec(string op){
    for(int i=0;i<sizeof ordered_ops;i++)
        if(op==ordered_ops[i]) return i;
    return -1;
}





exp_node::exp_node(){
    scope=isdec=false;
    left=right=nullptr;
    offset=vars_sz=0;
    type_str="";
}
//contruct an abtract syntax tree for c expressions
//this function implements a context free grammar
//the comments follow the corresponding notation
exp_node::exp_node(token *a,int n){
    scope=isdec=false;
    left=right=nullptr;
    offset=vars_sz=0;
    type_str="";
    assert(n>0);
    if(n==1){       //exp -> literal | variable_name
        assert(a[0].type!=delim);
        assert(a[0].type!=wrap);
        type=a[0].type;
        lexeme=a[0].lexeme;
        return;
    }
    if(is_enclosed(a,n)){  //exp -> (exp)
        *this=exp_node(a+1,n-2);
        return;
    }
    if(n==2){    //exp -> type_name variable_name
        assert(a[0].type==tname);
        assert(a[1].type==vname);
        type=a[1].type;
        lexeme=a[1].lexeme;
        type_str=a[0].lexeme;
        isdec=true;
        return;
    }
    if(is_func_call(a,n)){ // exp -> function_name() | function_name(exp)
        assert(a[1].lexeme=="(");
        type=a[0].type;
        lexeme=a[0].lexeme;
        if(n!=3) left=new exp_node(a+1,n-1);
        return;
    }
    if(is_func_def(a,n)){
        assert(a[2].lexeme=="(");
        isdec=true;
        type=a[1].type;
        lexeme=a[1].lexeme;
        type_str=a[0].lexeme;
        if(n!=4) left=new exp_node(a+2,n-2);
        return;
    }
    // exp -> exp binary_operator exp
    int x=split_by_all(a,n);
    *this=exp_node(a+x,1);
    left=new exp_node(a,x);
    right=new exp_node(a+x+1,n-x-1);
    assert(x!=-1);
        
}

//used to print the AST
void exp_node::print(int n){
    if(right) right->print(n+1);
    for(int i=0;i<n;i++) cout<<"      ";
    cout<<token_type_names[(int)type]<<" : "<<type_str<<" "<<lexeme<<endl;
    if(left) left->print(n+1);
}

void exp_node::verbose_print(int n){
    if(right) right->verbose_print(n+1);
    for(int i=0;i<n;i++) cout<<"      ";
    cout<<"{"<<token_type_names[(int)type]<<" : "<<type_str<<" "<<lexeme;
    cout<<", scope : "<<scope<<", off : "<<offset<<", vsc : "<<vars_sz;
    cout<<", dec : "<<isdec<<"}\n";
    if(left) left->verbose_print(n+1);
}
//unfinished
string exp_node::add_parens(exp_node *c,string s){
    if(c && c->type==binop && op_prec(lexeme)>op_prec(c->lexeme))
        return p_wrap(s);
    return s;
}
//unfinished
string exp_node::stringify(bool is_block){
    if((type==intl && type<=tname) || type==vname)
        return lexeme+(is_block ?";":"");
    string ls="",rs="";
    bool lblock,rblock;
    lblock=rblock=is_block;
    switch(type){
        case binop: lblock=rblock=false;break;
        case delim: lblock=rblock=true;break;
        default:;
    }
    if(left) ls=left->stringify(lblock);
    if(right) rs=right->stringify(rblock);
    string fs="";
    switch(type){
        case binop:
            ls=add_parens(left,ls);
            rs=add_parens(right,rs);
            fs=ls+lexeme+rs;
            break;
        case delim:
            if(lexeme=="{}") fs="{"+ls+"}"+rs;
            else fs=ls+";"+rs;
            break;
        default:;
    }
    return fs+(is_block ?";":"");
}

//a basic block is a c expression followed by a semicolon and nothing more
//for example : int i=14/3;
bool is_basic_block(token *a,int n){
    if(a[n-1].lexeme!=";") return false;
    for(int i=0;i<n-1;i++){
        string lex=a[i].lexeme;
        if(lex==";" || lex=="{" || lex=="}" ||
           a[i].type==keyword) return false;
    }
    return true;
}
//an enclosed block is a block wrapped in curly braces
//for example : {int i=13;}
bool is_enclosed_block(token *a,int n){
    if(a[0].lexeme!="{" || a[n-1].lexeme!="}") return false;
    //scan
    bool base=true;
    int p=0;
    for(int i=0;i<n;i++){
        if(a[i].lexeme=="{") p++;
        if(a[i].lexeme=="}") p--;
        if(p==0 && i!=n-1) base=false;
    }
    //invalid expression check
    if(p!=0){
        cout<<"unbalenced brackets"<<endl;
        throw(0);
    }
    return base;
}

//used to break up blocks that are just a list of expressions followed by semicolons
//for example: int i=0;int j=0;float x=1.0;
int split_block_by(token *a,int n,string kw){
    int b=0;
    int p=0;
    for(int i=0;i<n;i++){
        if(a[i].lexeme=="{") b++;
        if(a[i].lexeme=="}") b--;
        if(a[i].lexeme=="(") p++;
        if(a[i].lexeme==")") p--;
        if(a[i].lexeme==kw && b==0 && p==0) return i;
    }
    return -1;
}
//print token array
void lex_print(token *a,int n){
    for(int i=0;i<n;i++) cout<<a[i];
    cout<<endl;
}

//contructs a subtree to store for loop parameters
//say we have : for(init ; com ; inc)
//we can store the three parameters inc, com, and init in a tree like this
/*
        init
 fpp
                com
        fpc
                inc
 
*/
exp_node *loop_parameter_tree(exp_node *init,exp_node *com,exp_node *inc){
    auto fpp=new exp_node();
    auto fpc=new exp_node();
    fpp->type=forparams;
    fpc->type=forparams;
    fpp->left=init;
    fpp->right=fpc;
    fpc->left=com;
    fpc->right=inc;
    return fpp;
}
//used to find the closing paranthesis of a for loop
int for_param_helper(token *d,int rem){
    int p=1;
    for(int i=0;i<rem;i++){
        if(d[i].lexeme=="(") p++;
        if(d[i].lexeme==")") p--;
        if(p==0) return i;
    }
    return -1;
}

// here we define a context free grammar for a block of c code
block_node::block_node(token *a,int n):exp_node(){
    assert(n>0);
    if(n==1 && a[0].lexeme==";"){  // block -> ;
        type=delim;
        lexeme=";";
        return;
    }
    if(is_basic_block(a,n)){       // block -> exp;
        assert(a[n-1].lexeme==";");
        auto xpn=exp_node(a,n-1);
        *this=*(block_node*)&xpn;
        return;
    }
    if(is_enclosed_block(a,n)){    // block -> {block}
        *this=block_node(a+1,n-2);
        scope=true;
        return;
    }
    if(is_enclosed_block(a,n-1) && a[n-1].lexeme==";"){ // block -> {block};
        *this=block_node(a+1,n-3);
        scope=true;
        return;
    }
    int x=split_block_by(a,n,";");
    if(x!=-1 && x<n-1){           // block -> block ; block
        type=delim;
        lexeme=";";
        left=new block_node(a,x+1);
        right=new block_node(a+x+1,n-x-1);
        return;
    }
    x=split_block_by(a,n,"}");
    if(x!=-1 && x<n-1){         // block -> {block} block ;
        type=delim;
        lexeme="{}";
        left=new block_node(a,x+1);
        right=new block_node(a+x+1,n-x-1);
        return;
    }
    if(a[0].lexeme=="if" || a[0].lexeme=="while"){  // block -> if(exp) block | while(exp) block
        assert(a[1].lexeme=="(");
        type=a[0].type;
        lexeme=a[0].lexeme;
        int x=split_by_operator(a,n,")",false);
        assert(x!=-1);
        left=new exp_node(a+1,x);
        right=new block_node(a+x+1,n-x-1);
        right->scope=true;
        return;
    }
    if(a[0].lexeme=="else"){      //  block -> else block
        type=a[0].type;
        lexeme=a[0].lexeme;
        left=new block_node(a+1,n-1);
        left->scope=true;
        return;
    }
    if(a[0].lexeme=="break" || a[0].lexeme=="continue"){ //block -> break; | continue;
        assert(a[1].lexeme==";");
        type=a[0].type;
        lexeme=a[0].lexeme;
        return;
    }
    if(a[0].lexeme=="return"){        // block -> return exp;
        assert(a[n-1].lexeme==";");
        type=a[0].type;
        lexeme=a[0].lexeme;
        if(n>2) left=new exp_node(a+1,n-2);
        return;
    }
    if(a[0].lexeme=="for"){          // block -> for(exp;exp;exp) block
        assert(a[1].lexeme=="(");
        type=a[0].type;
        lexeme=a[0].lexeme;
        
        int rem=n-2;
        int x=-1;
        token *b=a+2;
        exp_node *params[3];
        for(int i=0;i<3;i++){
            if(i<2) x=split_block_by(b,rem,";");
            else    x=for_param_helper(b,rem);
            assert(x!=-1);
            if(x>=1) params[i]=new exp_node(b,x);
            else     params[i]=nullptr;
            rem-=x;
            b=b+x+1;
        }
        left=loop_parameter_tree(params[0],params[1],params[2]);
        right=new block_node(b,rem-3);
        right->scope=true;
        scope=true;
        return;
    }
    x=-1;
    for(int i=0;i<n;i++) if(a[i].lexeme=="{"){
        x=i;
        break;
    }
    assert(x!=-1);
    if(is_func_def(a,x)){   // block -> type_name function_name(exp) block
        assert(a[2].lexeme=="(");
        isdec=true;
        type=a[1].type;
        lexeme=a[1].lexeme;
        type_str=a[0].lexeme;
        if(x>4) left=new exp_node(a+2,x-2);
        right=new block_node(a+x,n-x);
        return;
    }
    
    
    assert(1==0);
}


