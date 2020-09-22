# Assembler

This project aims to make an assembler program, to assemble a didactic assemble language. This project is the first assignment of the curse Basics Software - CIC116432 – [UnB](https://www.unb.br/) – 2020/01.

**Student: Luan Haickel Araújo**

**Student ID: 12/0125781**

## Prerequisites

## Compilation

## Usage

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

The next table define the data allocation memory directives and pre processor directives.

| Mnemonic | Operators |  Code | Size | Description |
|:--------:|:-------------:|:-------------:|:-------------:|------|
| SECTION | 1 | - | 0 | Mark the start of code section (TEXT) or data section (DATA).|
| SPACE | 0 | - | 1 | Reserve 1 uninitialized memory address for storage one word.|
| CONST | 1 | - | 1 | Reserve memory for storing a 16-bit integer constant on a decimal or hexadecimal base.|
| EQU | 1 | - | 0 | Creates a textual synonym for a symbol.|
| IF | 1 | - | 0 | Instructs the assembler to include the **next line of code** only if the operand value is 1|
| MACRO | 0 | - | 0 | Mark the beginning of a MACRO. Always within the TEXT section and before the main code.|
| ENDMACRO | 0 | - | 0 | Mark the end of a MACRO.|


The identifiers of variables and labels are limited to 50 characters and follow the common rules of the C language.

The TEXT section and DATA section will always be marked and in the same order. Like the exemple below.

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
- **NOT** be case sensitive.
- Ignore any excessive spaces, enters, tabs, etc...
- The **CONST** must accept decimal positive and negative numbers.
- Be able to ignore comment (mark whit ';') at any point.
- The **COPY** must use a common and a space between arguments. (COPY A, B) 
- Accept line break after a line marker
  