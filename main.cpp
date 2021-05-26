#include "backend.h"

//add some formating to the raw assembly the compiler generated
//so it will assemble
string asm_format(string s,string funcs=""){
    string h="";
    h+=".globl    _main                   ## -- Begin function main\n";
    h+=".p2align    4, 0x90 \n";
    h+=funcs;
    h+="_main: \n";
    s=h+s;
    s+="\tretq\n";
    return s;
}

//write a string into a file
void string2file(string s,string fname){
    int fd=open(fname.c_str(),O_RDWR|O_TRUNC|O_CREAT);
    assert(fd!=0);
    int er=write(fd,s.c_str(),s.size());
    assert(er==s.size());
}

//use gcc to convert assembly file into a binary executable
void assemble_file(string ass,string exe){
    int id=fork();
    if(id==0){
        int e=execl("/usr/bin/gcc","gcc",ass.c_str(),"-o",exe.c_str(),nullptr);
        assert(e!=-1);
        exit(1);
    }
    int res=0;
    waitpid(id,&res,0);
    assert(WEXITSTATUS(res)==0);
}

//run an executable and return the exit status
//Im using the exit code to get ouput from the generated program
//total hack
int exec_file(string fname){
    int id=fork();
    if(id==0){
        int e=execl(fname.c_str(),fname.c_str(),nullptr);
        assert(e!=-1);
        exit(1);
    }
    int res=0;
    waitpid(id,&res,0);
    assert(WIFEXITED(res));
    return  (int)(char)WEXITSTATUS(res);
}

//these functions are used to format c code from the command so it can be compiled
string c_exp_format(string s){
    string h="";
    h+="int main(){\n";
    h+="return "+s+";\n";
    h+="}\n";
    return h;
}

string c_block_format(string s){
    string h="";
    h+="int main(){\n";
    h+="int r=0;\n";
    h+=s;
    h+="return r;\n";
    h+="}\n";
    return h;
}

//Im compiling input with gcc so it can be compared with the output from
//my compiler
void gcc_compile_file(string c,string exe){
    int id=fork();
    if(id==0){
        int e=execl("/usr/bin/gcc","gcc","-w",c.c_str(),"-o",exe.c_str(),nullptr);
        assert(e!=-1);
        exit(1);
    }
    int res=0;
    waitpid(id,&res,0);
    assert(WEXITSTATUS(res)==0);
}

void block_run(string ass,string funcs){
    string ass_code=asm_format(ass,funcs);
    string2file(ass_code,"gen.s");
    assemble_file("gen.s","gen");
    int mark=exec_file("gen");
    cout<<"mark: "<<mark<<endl;
}


//to validate that this compiler works, I pass the same c code to gcc.
//then I run both executables to see if the outputs match
bool exp_validate(string ass,string s){
    string ass_code=asm_format(ass);
    string2file(ass_code,"gen.s");
    assemble_file("gen.s","gen");
    int mark=exec_file("gen");
    cout<<"mark: "<<mark<<endl;
    
    string c_code=c_exp_format(s);
    string2file(c_code,"groundtruth.c");
    gcc_compile_file("groundtruth.c","groundtruth");
    
    int gcc=exec_file("groundtruth");
    cout<<"gcc : "<<gcc<<endl;
    
    if(mark!=gcc) cout<<"validation failed\n";
    return mark==gcc;
}

bool block_validate(string ass,string cc){
    string ass_code=asm_format(ass);
    string2file(ass_code,"gen.s");
    assemble_file("gen.s","gen");
    int mark=exec_file("gen");
    cout<<"mark: "<<mark<<endl;
    
    string c_code=c_block_format(cc);
    string2file(c_code,"groundtruth.c");
    gcc_compile_file("groundtruth.c","groundtruth");
    
    int gcc=exec_file("groundtruth");
    cout<<"gcc : "<<gcc<<endl;
    
    if(mark!=gcc) cout<<"validation failed\n";
    return mark==gcc;
    
    return true;
}

string funcs_asm(exp_node* root){
    auto ftrees=extract_func_defs(root);
    string s="";
    vector<string> func_names;
    for(auto i:ftrees){
        i->print();
        stack_info(i);
        s+=backend(i);
        func_names.push_back("_"+i->lexeme);
    }
    for(auto i:func_names){
        s=".global "+i+"\n"+s;
    }
    cout<<s;
    return s;
}


int main(int argc,char **argv){
    if(argc>1){
        cout<<"file mode\n";
        int fd=open(argv[1],O_RDONLY);
        assert(fd!=-1);
        int len=lseek(fd,0,SEEK_END);
        assert(len!=-1);
        lseek(fd,0,SEEK_SET);
        char buf[len];
        int rd=read(fd,buf,len);
        close(fd);
        assert(rd==len);
        string code="";
        for(int i=0;i<len;i++) code+=buf[i];
        cout<<code;
        cout<<"\nlength: "<<len<<endl;
        code=preprocessor(code);
//        cout<<code<<endl;
        vector<token> vt=lexer(code);
        block_node root(&vt[0],vt.size());
//        root.print();
        code=funcs_asm(&root);
//        cout<<"nig\n"<<code;
        fd=open("gen.s",O_RDWR|O_CREAT|O_TRUNC);
        assert(fd!=-1);
        len=write(fd,&code[0],code.size());
        assert(len==code.size());
        exit(0);
    }
    
    
    char buf[512];
    cout<<"C compiler\n";
    while(1){
        memset(buf,0,sizeof buf);
        write(1,">> ",3);
        int n=read(0,buf,sizeof buf);
        string s=buf;

        s=preprocessor(s);
        if(s.size()==0) continue;
        

        //cout<<s<<endl;
        vector<token> vt=lexer("int r=0;"+s+"r;");
        
        //lex_print(&vt[0],vt.size());
        
        //exp_node root(&vt[0],vt.size());
        block_node root(&vt[0],vt.size());
        root.scope=true;
        
        
        string fasm=funcs_asm(&root);
        
        
        stack_info(&root);
        
        //root.verbose_print();
        root.print();
        
        string ass=backend(&root);
        cout<<ass;
        block_run(ass,fasm);
        
        //block_validate(ass,s);
        
      
    }
}
