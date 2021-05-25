#include "backend.h"

//communitive    : +,*,&,|,^,&&,||,==,!=
//non-communitive: -,/,>>,<<,<,>,>=,<=,=,+=,-=,/=

//format and instuction with two parameters
string inst(string in,string a1,string a2){
    return "\t"+in+" "+a1+","+a2+"\n";
}

//saves a register before it is modified and restores it afterward
string stack_wrap(string s,string reg){
    return "\tpushq "+reg+"\n"+s+"\tpopq "+reg+"\n";
}

//subtract
string sub(string src,string dest,bool dms){
    string s;
    s=inst("subq ",src,dest);
    if(!dms) s+="\tnegq "+dest+"\n";  //subtraction does not commute
    return s;
}

//implements integer division and the modulus
string divmod(string src,string dest,bool dms,bool div){
    string s="",core="";
    string dr=dms ? src:dest; //divisor
    string dd=dms ? dest:src; //dividend
    bool urcx=false;
    if(dr=="%rax"){
        s+=inst("movq",dr,"%rcx");
        dr="%rcx";
        urcx=true;
    }
    if(dd!="%rax") core+=inst("movq",dd,"%rax");
    core+="\tcqo\n";
    core+="\tidivq "+dr+"\n";
    if(div){
        if(dest!="%rax") core+=inst("movq","%rax",dest); //division
    }else{
        if(dest!="%rdx") core+=inst("movq","%rdx",dest); //modulus
    }
    
    if(dest!="%rdx") core=stack_wrap(core,"%rdx");
    s+=core;
    
    if(urcx) s=stack_wrap(s,"%rcx");
    if(dest!="%rax") s=stack_wrap(s,"%rax");
    return s;
}

//swap the contents of two registers
string reg_swap(string r1,string r2){
    return "\tpushq "+r1+"\n\tmovq "+r2+","+r1+"\n\tpopq "+r2+"\n";
}
//implements bitshift operators <<,>>
string shift(string src,string dest,bool dms,bool shl){
    string s="";
    if(!dms) s+=reg_swap(src,dest);
    bool urcx=false;
    if(src!="%rcx"){
        s+=inst("movq",src,"%rcx");
        urcx=true;
    }
    if(shl) s+=inst("sal","%cl",dest);
    else    s+=inst("sar","%cl",dest);
    if(urcx) s=stack_wrap(s,"%rcx");
    return s;
}
//implements == and !=
//conditional moves are preferable to jumps
string comp_eq(string src,string dest,bool eq){
    string s="";
    
    s+=inst("cmp",src,dest);
    s+=inst("movq","$0",dest);
    s+=inst("movq","$1","%rcx");
    s+=inst(eq ? "cmoveq":"cmovneq","%rcx",dest);
    
    return stack_wrap(s,"%rcx");
}
//implements <,>,<=,>=
string comp_ltgt(string src,string dest,bool dms,bool gt,bool eq){
    string s="";
    
    if(dms) gt=!gt;
    
    s+=inst("cmp",dest,src);
    s+=inst("movq","$0",dest);
    s+=inst("movq","$1","%rcx");
    string cmov="cmov";
    cmov+=gt?"g":"l";
    cmov+=eq?"eq":"q";
    s+=inst(cmov,"%rcx",dest);
    
    return stack_wrap(s,"%rcx");
}
//implements && and ||
string bool_and_or(string src,string dest,bool isand){
    string s="";
    s+=inst("movq","$1","%rcx");
    s+=inst("cmpq","$0",dest);
    s+=inst("movq","$0",dest);
    s+=inst("cmovneq","%rcx",dest);  //anything that isnt zero is true in c
    
    s+=inst("cmpq","$0",src);
    s+=inst("movq","$0","%rdx");
    s+=inst("cmovneq","%rcx","%rdx");
    
    s+=inst(isand?"andq":"orq","%rdx",dest);
    return s;
}


//place some assembly code into its own stack frame
string scope_wrap(string s,int vars_sz){
    assert(vars_sz>=0);
    string a="";
    a+="\tpushq %rbp //scope begin\n"; //save old base ptr
    a+=inst("movq","%rsp","%rbp"); //set new base ptr
    if(vars_sz) a+=inst("subq","$"+to_string(vars_sz*8),"%rsp"); //make room for local vars
    a+=s;
    if(vars_sz) a+=inst("addq","$"+to_string(vars_sz*8),"%rsp"); //deallocate local vars
    a+="\tpopq %rbp //scope end\n"; //restore old base ptr
    return a;
}

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

//encodes the size of stack frames and the relative addresses of
//local variables into the abstract syntax tree
void rec_stack_info(exp_node *node,symstack *st){
    if(!node) return;
    if(node->scope){  //when a node is a local scope
        st->push_frame(); //new stack frame for the nodes variables
        cout<<*st;
    }
    if(node->type==vname){
        if(node->isdec){  //if a variable is being declared it must be pushed onto the stack
            st->push(localvar(node->lexeme));
            cout<<*st;
            node->offset=st->sp-st->bp-1; //address relative to the base pointer
        }else{
            int x=st->find_idx(node->lexeme); //get the absolute address of the variable
            node->offset=x-st->bp;            //compute relative address
            assert(st->v[st->bp+node->offset].name==node->lexeme);
        }
    }else{
        //recursively traversing the tree
        rec_stack_info(node->left,st);
        rec_stack_info(node->right,st);
    }
    if(node->scope){
        assert(st->sp>=st->bp);
        node->vars_sz=st->sp-st->bp; //compute the number of local variables in the scope
        st->pop_frame();             //pop the stack frame
        cout<<*st;
    }
}


//just a wrapper
void stack_info(exp_node *node){
    auto st=symstack();
    rec_stack_info(node,&st);
}

//the address of a varible bein assigned will always be in r10
string assign(string reg){
    return inst("movq",reg,"(%r10)");
}

//putting it all together
//all left nodes store their return value in rax
//right nodes store return value in rbx
//that way, if the parent is an operator, it always knows where to find its operands
string backend(exp_node *node,bool left,string tag){
    if(!node) return "";
    if(node->type==intl){
        string assm="";
        assm+=inst("movq","$"+node->lexeme,left ?"%rbx":"%rax");
        return assm;
    }
    if(node->type==vname){
        string assm="";
        assm+=inst("movq",to_string(-(node->offset+1)*8)+"(%rbp)",left ?"%rbx":"%rax");
        assm+=inst("leaq",to_string(-(node->offset+1)*8)+"(%rbp)",left ?"%r10":"%r9");
        if(node->scope) assm=scope_wrap(assm,node->vars_sz);
        return assm;
    }
    if(node->type==binop){
        string assm="";
        assm+=backend(node->right,false,tag+"1");
        assm+=backend(node->left ,true ,tag+"0");
        string reg1=left ?"%rax":"%rbx";
        string reg2=left ?"%rbx":"%rax";
        
        string ptr1=left ?"%r9":"%r10";
        string ptr2=left ?"%r10":"%r9";
        string instr="";
        
        if     (node->lexeme=="+"  ) assm+=inst("addq ",reg1,reg2);
        else if(node->lexeme=="*"  ) assm+=inst("imulq",reg1,reg2);
        else if(node->lexeme=="&"  ) assm+=inst("andq ",reg1,reg2);
        else if(node->lexeme=="|"  ) assm+=inst("orq  ",reg1,reg2);
        else if(node->lexeme=="^"  ) assm+=inst("xorq ",reg1,reg2);
        else if(node->lexeme=="-"  ) assm+=sub(reg1,reg2,left);
        else if(node->lexeme=="/"  ) assm+=divmod(reg1,reg2,left,true);
        else if(node->lexeme=="%"  ) assm+=divmod(reg1,reg2,left,false);
        else if(node->lexeme=="<<" ) assm+=shift(reg1,reg2,left,true);
        else if(node->lexeme==">>" ) assm+=shift(reg1,reg2,left,false);
        else if(node->lexeme=="==" ) assm+=comp_eq(reg1,reg2,true);
        else if(node->lexeme=="!=" ) assm+=comp_eq(reg1,reg2,false);
        else if(node->lexeme==">"  ) assm+=comp_ltgt(reg1,reg2,left,true,false);
        else if(node->lexeme=="<"  ) assm+=comp_ltgt(reg1,reg2,left,false,false);
        else if(node->lexeme==">=" ) assm+=comp_ltgt(reg1,reg2,left,true,true);
        else if(node->lexeme=="<=" ) assm+=comp_ltgt(reg1,reg2,left,false,true);
        else if(node->lexeme=="&&" ) assm+=bool_and_or(reg1,reg2,true);
        else if(node->lexeme=="||" ) assm+=bool_and_or(reg1,reg2,false);
        else if(node->lexeme=="="  ) assm+=assign("%rax");
        else if(node->lexeme=="+=" ) assm+=inst("addq ",reg1,reg2)+assign(reg2);
        else if(node->lexeme=="*=" ) assm+=inst("imulq",reg1,reg2)+assign(reg2);
        else if(node->lexeme=="&=" ) assm+=inst("andq ",reg1,reg2)+assign(reg2);
        else if(node->lexeme=="|=" ) assm+=inst("orq  ",reg1,reg2)+assign(reg2);
        else if(node->lexeme=="^=" ) assm+=inst("xorq ",reg1,reg2)+assign(reg2);
        else if(node->lexeme=="-=" ) assm+=sub(reg1,reg2,left)+assign(reg2);
        else if(node->lexeme=="/=" ) assm+=divmod(reg1,reg2,left,true)+assign(reg2);
        else if(node->lexeme=="%=" ) assm+=divmod(reg1,reg2,left,false)+assign(reg2);
        else if(node->lexeme=="<<=") assm+=shift(reg1,reg2,left,true)+assign(reg2);
        else if(node->lexeme==">>=") assm+=shift(reg1,reg2,left,false)+assign(reg2);
        else{
            cout<<"undefined operator\n";
            throw(0);
        }
        
        if(left) assm=stack_wrap(stack_wrap(assm,"%rax"),"%r9");
        if(node->scope) assm=scope_wrap(assm,node->vars_sz);
        return assm;
    }
    if(node->type==delim){
        string assm="";
        assm+=backend(node->left ,false,tag+"0");
        assm+=backend(node->right,false,tag+"1");
        if(node->scope) assm=scope_wrap(assm,node->vars_sz);
        return assm;
    }
    if(node->type==keyword){
        string assm="";
        if(node->lexeme=="if"){
            assm+=tag+"if:\n";
            assm+=backend(node->left ,false,tag+"0");
            assm+=inst("cmp","$0","%rax");
            assm+="\tje "+tag+"endif \n";
            assm+=backend(node->right,false,tag+"1");
            assm+=tag+"endif:\n";
            
        }else if(node->lexeme=="while"){
            assm+=tag+"while:\n";
            assm+=backend(node->left ,false,tag+"0");
            assm+=inst("cmp","$0","%rax");
            assm+="\tje "+tag+"endwhile \n";
            assm+=backend(node->right,false,tag+"1");
            assm+="\tjmp "+tag+"while\n";
            assm+=tag+"endwhile:\n";
            
        }else if(node->lexeme=="for"){
            
            auto fpp=node->left;
            auto fpc=fpp->right;
            auto init=fpp->left;
            auto com=fpc->left;
            auto inc=fpc->right;
            
            if(init) assm+=backend(init,false,tag+"00");
            assm+=tag+"for:\n";
            if(com) assm+=backend(com,false,tag+"010");
            assm+=inst("cmp","$0","%rax");
            assm+="\tje "+tag+"endfor \n";
            assm+=backend(node->right,false,tag+"1");
            if(inc) assm+=backend(inc,false,tag+"010");
            assm+="\tjmp "+tag+"for\n";
            assm+=tag+"endfor:\n";
            
        }else{
            cout<<"undefined keyword\n";
            throw(0);
        }
        if(node->scope) assm=scope_wrap(assm,node->vars_sz);
        return assm;
    }
    cout<<"unimplemented language feature\n";
    throw(0);
    return "";
}

