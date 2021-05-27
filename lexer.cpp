#include "lexer.h"


//just removes "//" and "/* */" comments
string nocomment(string s){
    int linecom=-1;
    int mullcom=-1;
    bool quote=false;
    for(int i=0;i<s.size();i++){
        if(s[i]=='"' && linecom==-1 && mullcom==-1) quote=!quote;
        if(s.substr(i,2)=="//" && linecom==-1 && mullcom==-1 && !quote) linecom=i;
        if(s[i]=='\n' && linecom!=-1){
            s.erase(s.begin()+linecom,s.begin()+i);
            i=linecom-1;
            linecom=-1;
            continue;
        }
        if(s.substr(i,2)=="/*" && linecom==-1 && mullcom==-1 && !quote) mullcom=i;
        if(s.substr(i,2)=="*/" && mullcom!=-1){
            s.erase(s.begin()+mullcom,s.begin()+i+2);
            i=mullcom-1;
            mullcom=-1;
            continue;
        }
    }
    return s;
}

string onespace(string s){
    //reduce multiple continuous spaces with a single space
    //if they are not enclosed within double quotes
    bool quote=false;
    for(int i=0;i<s.size()-1;i++){
        if(s[i]=='"') quote=!quote;
        while(s[i]==' ' && s[i+1]==' ' && !quote) s.erase(s.begin()+i+1);
    }
    if(s[0]==' ') s=s.substr(1,s.size()-1); //remove leading space
    if(s.size()>0 && s[s.size()-1]==' ') s=s.substr(0,s.size()-1); //remove trailing space
    return s;
}

bool char_match(char c,string s){
    for(auto i:s) if(i==c) return true;
    return false;
}

bool non_printable(char c){
    return c<32;
}

//remove tabs and new lines
string nospecwhitespace(string s){
    bool quote=false;
    string banned="\n\t\r";
    assert(banned.size()==3);
    for(int i=0;i<s.size();i++){
        if(s[i]=='"') quote=!quote;
        if((char_match(s[i],banned) || non_printable(s[i])) && !quote){
            s.erase(s.begin()+i);
            i--;
        }
    }
    return s;
}

//putting it all together
string preprocessor(string s){
    s=nocomment(s);
    s=nospecwhitespace(s);
    s=onespace(s);
    for(int i=0;i<s.size()-1;i++) assert(!(s[i]==' ' && s[i+1]==' '));
    return s;
}
//tokens
//names={fnames,vnames}
//finite word set={typenames,keywords}
//  typenames={char,int,long,float,double,void}
//  keywords={for,while,if,else,break,continue,return}
//literals={int,float,char,char*}
//binary operators:
//  arithmatic={+,-,*,/,%}
//  bitwise={&,|,^,>>,<<}
//  boolean={&&,||,<,>,<=,>=,==,!=}
//  assignment=(=,+=,-=,*=,/=,%=,&=,|=,^=,>>=,<<=}
//wrappers={\{,\},(,),",'}
//delimeters={\,,;}

//these function are used to determine a tokens type
//return the length if its a match else return -1
int test_keyword(string s){
    static string kws[]={"for","while","if","else","break","continue","return"};
    for(int i=0;i<sizeof(kws);i++){
        if(s.substr(0,kws[i].size())==kws[i]) return kws[i].size();
    }
    return -1;
}

int test_typename(string s){
    static string tns[]={"char","int","long","float","double","void"};
    for(int i=0;i<sizeof(tns);i++){
        if(s.substr(0,tns[i].size())==tns[i]) return tns[i].size();
    }
    return -1;
}

int test_u_op(string s){
    static string bos[]={"--","++","-","+","&","!","~","*"};
    for(int i=0;i<sizeof(bos);i++){
        if(s.substr(0,bos[i].size())==bos[i]) return bos[i].size();
    }
    return -1;
}

int test_bin_op(string s){
    static string bos[]={
        ">>=","<<=",">>","<<","&&","||","<=",">=","==",
        "!=","+=","-=","*=","/=","%=","&=","|=","^=","<",
        ">","=","+","-","*","/","%","&","|","^",","
    };
    for(int i=0;i<sizeof(bos);i++){
        if(s.substr(0,bos[i].size())==bos[i]) return bos[i].size();
    }
    return -1;
}

int test_wrapper(string s){
    static string wrs[]={"{","}","(",")"};
    for(int i=0;i<sizeof(wrs);i++){
        if(s.substr(0,wrs[i].size())==wrs[i]) return wrs[i].size();
    }
    return -1;
}

int test_delimiter(string s){
    static string dls[]={";"};
    for(int i=0;i<sizeof(dls);i++){
        if(s.substr(0,dls[i].size())==dls[i]) return dls[i].size();
    }
    return -1;
}

int biggest_match(string s,string rgx){
    smatch sm;
    regex_search(s,sm,regex(rgx),regex_constants::match_continuous);
    int mx=-1;
    for(auto i:sm){
        mx=max(mx,(int)i.length());
    }
    return mx;
}
//using regular expressions to find literals in source code like "132" or "2.97"
int test_int_literal(string s){
    return biggest_match(s,"\\d+");
}

int test_float_literal(string s){
    return max(biggest_match(s,"\\d+\\.\\d*"),biggest_match(s,"\\d*\\.\\d+"));
}

int test_char_literal(string s){
    return biggest_match(s,"'\\\\?.'");
}

int test_cstr_literal(string s){
    return biggest_match(s,"\"[^\"]*\"");
}

bool match_is_fname(string s,int n){
    if(n<=0 ) return false;
    if(s.size()>n && s[n]=='(') return true;
    if(s.size()>n+1 && s[n]==' ' && s[n+1]=='(') return true;
    return false;
}
//variable names
int test_vname(string s){
    int n=biggest_match(s,"[_a-zA-Z][_a-zA-Z0-9]*");
    return match_is_fname(s,n) ?-1:n;
}
//function names
int test_fname(string s){
    int n=biggest_match(s,"[_a-zA-Z][_a-zA-Z0-9]*");
    return match_is_fname(s,n) ?n:-1;
}
//used for printing purposes
string token_type_names[]={
    "keyword","binop","wrap","delim",
    "intl","floatl","charl","cstrl","tname",
    "vname","fname","uop","forparams"
};
//find the longest possible match
int test_all(string s,token_type &type){
    static int (*funcs[])(string)={
        test_keyword,test_bin_op,test_wrapper,test_delimiter,
        test_int_literal,test_float_literal,test_char_literal,
        test_cstr_literal,test_typename,test_vname,test_fname,
        test_u_op
    };
    int mx=-1;
    for(int i=0;i<sizeof(funcs)/8;i++){
        int n=funcs[i](s);
        if(n>mx){
            mx=n;
            type=(token_type)i;
        }
    }
    return mx;
}


bool is_u_op(token *prev,token *now){
    assert(prev!=now);
    if(now->lexeme!="+" && now->lexeme!="-" &&
       now->lexeme!="&" && now->lexeme!="*") return false;
    assert(now->type==binop);
    if(!prev) return true;
    switch(prev->type){
        case keyword: return true;
        case binop: return true;
        case wrap: return prev->lexeme=="(" || prev->lexeme=="{";
        case delim: return true;
        case uop: return true;
        default: return false;
    }
    assert(1==0);
//    keyword,binop,wrap,delim,
//    intl,floatl,charl,cstrl,tname,vname,fname,forparams
    return false;
}

//converts a string of c source code into a vector of tokens
vector<token> lexer(string s){
    int i=0;
    vector<token> v;
    token *prev=nullptr;
    while(i<s.size()){
        if(s[i]==' '){
            i++;
        }
        token t(s.substr(i,s.size()));
//        assert(t.type!=uop);
        if(is_u_op(prev,&t)) t.type=uop;
        v.push_back(t);
        i+=t.n;
        prev=&v[v.size()-1];
    }
    return v;
}





ostream &operator<<(ostream &os,token t){
    os<<token_type_names[(int)t.type]<<" : "<<t.lexeme<<" , ";
    return os;
}
