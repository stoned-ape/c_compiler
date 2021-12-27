# C Compiler

Use `make` to build, the executables is named `compiler`.

This compiler has two modes, shell mode and file mode.
```bash
./compiler #creates a shell
```

```bash
./compiler main.c #creates a gen.s assembly file
```

In this screen shot we can see the compiler being run in
shell mode with `make run`.  At the prompt, I typed in `r=17+1"`.
Whatever is stored in the variable "r" is returned by the shell.
The first thing returned is the lexer output, then the abstract
syntax tree, then the generated assembly, and finally, the value
returned by the process we compiled.  The return value is 18 because
we set "r" to equal 17 plus one. 

![img](pic.png)