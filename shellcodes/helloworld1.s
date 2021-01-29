; Taken from "The Art of Exploitation" by Jon Erickson
BITS 32           ; Tell nasm this is a 32-bit code.

  call mark_below ; Call bellow the string to instructions
  db "Hello, world!", 0x0a, 0x0d ; with newline and carriage return bytes.

mark_below:
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
