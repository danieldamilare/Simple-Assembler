##  Simple Assembler Interpreter
Support the following instructions:

```mov x, y``` - copy y (either an integer or the value of a register) into register x.
```inc x``` - increase the content of register x by one.
```idec x```- decrease the content of register x by one.
```add x, y``` - add the content of the register x with y (either an integer or the value of a register) and stores the result in x (i.e. register[x] += y).
```sub x, y``` - subtract y (either an integer or the value of a register) from the register x and stores the result in x (i.e. register[x] -= y).
```mul x, y``` - same with multiply (i.e. register[x] *= y).
```div x, y``` - same with integer division (i.e. register[x] /= y).
```label```: - define a label position (label = identifier + ":", an identifier being a string that does not match any other command). Jump commands and call are aimed to these labels positions in the program.
```jmp lbl``` - jumps to the label lbl.
```cmp x, y``` - compares x (either an integer or the value of a register) and y (either an integer or the value of a register). The result is used in the conditional jumps (jne, je, jge, jg, jle and jl)
```jne lbl``` - jump to the label lbl if the values of the previous cmp command were not equal.
```je lb``` - jump to the label lbl if the values of the previous cmp command were equal.
```jge lbl``` - jump to the label lbl if x was greater or equal than y in the previous cmp command.
```jg lbl``` - jump to the label lbl if x was greater than y in the previous cmp command.
```jle lbl``` - jump to the label lbl if x was less or equal than y in the previous cmp command.
```jl lbl``` - jump to the label lbl if x was less than y in the previous cmp command.
```call lbl``` - call to the subroutine identified by lbl. When a ret is found in a subroutine, the instruction pointer should return to the instruction next to this call command.
```ret``` - when a ret is found in a subroutine, the instruction pointer should return to the instruction that called the current function.
```msg 'Register: ', x``` - this instruction stores the output of the program. It may contain text strings (delimited by single quotes) and registers. The number of arguments isn't limited and will vary, depending on the program.
```end``` - this instruction indicates that the program ends correctly, so the stored output is returned (if the program terminates without this instruction it should return the default output: see below).
```; comment``` - comments should not be taken in consideration during the execution of the program.

** Taken from this [codewar kata](https://www.codewars.com/kata/58e61f3d8ff24f774400002c/train/c) **

