[comment]: <> (This file was written in markdown and is much more readable if read at: https://github.com/luanharaujo/assembler)
# Assembler

This project aims to make an assembler program, to assemble a didactic assemble language. This project is the first assignment of the course: Basics Software - CIC116432 – [UnB](https://www.unb.br/) – 2020/01.

**Student: Luan Haickel Araújo**

**Student ID: 12/0125781**

## The choice

The student have to make a choice between one of the follow:

- [x] The assembler will accept MACRO directive; or
- [ ] identify errors during assembly.

### **On this work I chose accept MACRO directive.** 

The choice above was made with the following conditions;
- 2 is the maximum number of MACROS;
- a MACRO will never call an other MACRO;
- a MACRO will never have any directive; and
- each MACRO can have up to 2 arguments. 

## Prerequisites

This program probably works on any Unix base system with a modern GCC. But It was developed in a **Ubuntu** - 20.04.1 LTS running on a WSL - V1 inside a Windows 10. The compiler used was a **gcc (Ubuntu 9.3.0-10ubuntu2) 9.3.0**.

## Compilation
```
gcc -o montador montador.c
```

## Usage

### First:
```
./montador -p input_file.asm
```
### Then:
```
./montador -o input_file.pre
```

## Returns Status
| Return | Error |  
|--------:|:-------------|
| 0 | No error. |
| 1 | Wrong number of inputs in the program call.|
| 2 | Wrong option. |
| 3 | Error when trying to open the input file. |
| 4 | Error when trying to create the output file. |
| 5 | No colon after label. |
| 6 | No comma after first operator of COPY. |
| 7 | Invalid section or section order. |
| 8 | ENDMACRO whitout a MACRO. |
| 9 | Exceeded maximum number of MACROS. |
| 10 | Macro whit more than 2 arguments. |
| 11 |Unallowed operation within a MACRO |
| 12 |MACRO whitout a ENDMACRO. |
| 13 |MACRO definition with more arguments than the MACRO call. |

## Specifications:
The instruction set of this hypothetic assemble have only 14 instructions, presented in the table below.   


| Mnemonic | Operators |  Code | Size | Description |
|:--------:|:-------------:|:-------------:|:-------------:|------|
| ADD | 1 | 1 | 2 | ACC &#8592; ACC + MEM[OP]|
| SUB | 1 | 2 | 2 | ACC &#8592; ACC - MEM[OP]|
| MULT | 1 | 3 | 2 | ACC &#8592; ACC * MEM[OP]|
| DIV | 1 | 4 | 2 | ACC &#8592; ACC / MEM[OP]|
| JMP | 1 | 5 | 2 | PC &#8592; OP|
| JMPN | 1 | 6 | 2 | IF ACC < 0 , PC &#8592; OP|
| JMPP| 1 | 7 | 2 | IF ACC > 0 , PC &#8592; OP|
| JMPZ | 1 | 8 | 2 | IF ACC = 0 , PC &#8592; OP|
| COPY | 2 | 9 | 3 | MEN[OP2] &#8592; MEN[OP1]|
| LOAD | 1 | 10 | 2 | ACC &#8592; MEN[OP]|
| STORE | 1 | 11 | 2 | MEN[OP] &#8592; ACC|
| INPUT | 1 | 12 | 2 | MEN[OP] &#8592; STDIN|
| OUTPUT | 1 | 13 | 2 | STDOUT &#8592; MEN[OP]|
| STOP | 0 | 14 | 1 | End execution|

The next table define the directives for allocating memory in the data segment and pre processor directives.

| Mnemonic | Operators |  Code | Size | Description |
|:--------:|:-------------:|:-------------:|:-------------:|------|
| SECTION | 1 | - | 0 | Mark the start of code section (TEXT) or data section (DATA).|
| SPACE | 0 | - | 1 | Reserve 1 uninitialized memory address for storage one word.|
| CONST | 1 | - | 1 | Reserve memory for storing a 16-bit integer constant on a decimal base.|
| EQU | 1 | - | 0 | Creates a textual synonym for a symbol.|
| IF | 1 | - | 0 | Instructs the assembler to include the **next line of code** only if the operand value is 1|
| MACRO | 0 | - | 0 | Mark the beginning of a MACRO. Always within the TEXT section and before the main code.|
| ENDMACRO | 0 | - | 0 | Mark the end of a MACRO.|


The identifiers of variables and labels are limited to 50 characters and follow the common rules of the C language.

The TEXT section and DATA section will always be marked and will always be in the same order. As in the exemple below.

```
SECTION TEXT
ROT: INPUT N1
    COPY N1, N4 ;comment
    COPY N2, N3
    COPY N3, N3
    OUTPUT N3
    STOP

SECTION DATA
N1: SPACE
N2: CONST -48
N4: SPACE
```

### The assembler must:
- **NOT** be case sensitive;
- ignore any excessive space, enter, tab, etc;
- the **CONST** must accept decimal positive and negative numbers;
- be able to ignore comment (marked whit ';') at any point;
- the **COPY** must use a comma and a space between arguments. (COPY A, B) ;
- accept line break after a line marker;
- do one of the following for each time its called:
  - If called whit the -p option the assembler must do the pre-processing and generate a .pre text file whit the same name of the input file.
  - If called whit the -o option the assembler must do the assembling, base on the .pre file, and generate a .obj text file whit the same name of the input file.
  
### The input assemble code must:
- Have all the **EQU** directive in the beginning of the programe, **before the SECTION DATA and SECTION TEXT**.
- **NOT** have a **IF** without a previously **EQU**. 
  

#### Exemple of IF and EQU:

###### Input file:
```
L1:  EQU 1
L2:  EQU 0
SECTION TEXT
IF L1
LOAD SPACE ;do the operation if L1 is true
IF L2
INPUT SPACE ;do the operation if L2 is true

SECTION DATA
N: SPACE
```
##### Pre-processe file:

```
SECTION TEXT
LOAD SPACE

SECTION DATA
N: SPACE
```
## Examples

Some exemples codes to be assembled, given by the teacher, can be found in the exemples folder.

## Simulator

A simulator will be provided by the teacher to test the assembled codes. When this happens I will put the usage instructions in this section.