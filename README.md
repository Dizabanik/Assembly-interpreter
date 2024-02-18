# Simple Assembly interpreter using C

### Instruction Set for a Simple Assembly Language

#### Data Movement
- `mov x, y`: Copy the value of `y` (either an integer or the content of a register) into register `x`.

#### Arithmetic Operations
- `inc x`: Increase the content of register `x` by one.
- `dec x`: Decrease the content of register `x` by one.
- `add x, y`: Add the value of `y` (either an integer or the content of a register) to the content of register `x`.
- `sub x, y`: Subtract the value of `y` (either an integer or the content of a register) from the content of register `x`.
- `mul x, y`: Multiply the content of register `x` by `y` (either an integer or the content of a register).
- `div x, y`: Perform integer division on the content of register `x` by `y` (either an integer or the content of a register).

#### Control Flow
- `label:`: Define a label position. Jump and call instructions reference these labels.
- `jmp lbl`: Jump to the label `lbl`.
- `cmp x, y`: Compare `x` (either an integer or the content of a register) and `y` (either an integer or the content of a register).
- `jne lbl`: Jump to the label `lbl` if the values of the previous `cmp` command were not equal.
- `je lbl`: Jump to the label `lbl` if the values of the previous `cmp` command were equal.
- `jge lbl`: Jump to the label `lbl` if `x` was greater than or equal to `y` in the previous `cmp` command.
- `jg lbl`: Jump to the label `lbl` if `x` was greater than `y` in the previous `cmp` command.
- `jle lbl`: Jump to the label `lbl` if `x` was less than or equal to `y` in the previous `cmp` command.
- `jl lbl`: Jump to the label `lbl` if `x` was less than `y` in the previous `cmp` command.
- `call lbl`: Call the subroutine identified by `lbl`. The instruction pointer returns to the next instruction after the `call` when a `ret` is encountered in the subroutine.
- `ret`: Return from a subroutine.

#### Output
- `msg 'Register: ', x`: Store the output of the program. Text strings are delimited by single quotes. It may contain text strings and registers.

#### Program Termination
- `end`: Indicates the end of the program, returning the stored output. If the program terminates without this instruction, it should return the default output.

#### Comments
- `; comment`: Comments are ignored during program execution.
