# Shellcodes - Multiple choices questions
---
**Which of the following assembly instructions are equivalent to the instruction `mov eax, 4`** ?
- `mov ax, 4`
- `mov al 4`
- `mov ah 4`
- `mov eax, 0x4`
- `mov eax, 0x04`

**Which of the following assembly instructions are equivalent to the instruction `mov eax, 0`** ?
- `mov ax, 0`
- `mov al 0`
- `mov ah 0`
- `xor eax, eax`
- `sub eax, eax`

**Which of the following assembly instructions are correctly formated shellcodes ?** ?

*  
        mov eax, 4
        mov ebx, 1
        mov edx, 15
        int 0x80

*  
        mov eax, 0xbffff364
        sub eax, 0xbffff368
        xor ebx, eax
        inc ebx
        int 0x80
*  
        mov eax, 0xbffff364
        add eax, 0xbf00f368
        xor ebx, eax
        inc ebx
        int 0x80
*  
        mov eax, 0xbffff364
        add eax, 0xbff00368
        xor ebx, eax
        inc ebx
        int 0x80
