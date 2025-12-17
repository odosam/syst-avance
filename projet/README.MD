# Minishell

Minishell is a **mini Unix shell** implemented in C. 
It allows you to execute basic Unix commands, handle redirections, pipes, built-in commands, aliases and set environment variable.



## Features

- Built-in commands : `ls`, `ps`, `who`, `cd`,`pwd`,`echo`,`exit`,`export`,`alias`,`history`
- Pipe support (`|`)
- Redirections : 
  - `>`  : redirect output to a file (overwrite)  
  - `>>` : redirect output to a file (append)  
  - `<`  : take input from a file  
  - `<<` : here document  
- Execute commands in the **background** with `&`  
- Support for **aliases**  
- Command history  
- Environment variable

## How to Build

Clone the project and compile with  `make` : 
```bash
git clone https://github.com/odosam/syst-avance/tree/main/projet
cd projet
make
```

### Usage 

Run the shell : 
```bash
./bin/program
```

Run the manual : 
```bash
man ./minishell.1
```
