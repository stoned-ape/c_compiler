#include <iostream>
#include <regex>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <assert.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

#ifndef lexer_h
#define lexer_h


using namespace std;

//this preprocessor just removes comments and whitespace
string preprocessor(string s);


enum token_type{keyword,binop,wrap,delim,
                intl,floatl,charl,cstrl,tname,vname,fname,forparams};


int test_all(string s,token_type &type);

struct token{
    string lexeme;  //the raw text that represents the token
    token_type type;
    int n;          //the number of characters in the lexeme
    token(string s){
        if(s.size()==0) return;
        n=test_all(s,type);
        assert(n>0);
        lexeme=s.substr(0,n);
    }
};

int biggest_match(string s,string rgx);

//print a token
ostream &operator<<(ostream &os,token t);
//perform lexical analysis on c source code
vector<token> lexer(string s);







#endif
