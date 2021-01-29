# Setting up the lab environment

For this course we won't be using our own private machine in order to protect us from undesired behaviours. We will be using a Virtual environment designed specifically for hackers : [Kali Linux](https://www.kali.org/)

We might need another Virtual Machine that we will call [Metasploitable](https://docs.rapid7.com/metasploit/metasploitable-2/) that is designed with a lot of common vulnerabilities that we will be exploiting.

**Why have two machines ?** We will see some exploit that requires a running server with specific port opened and Metasploitable has it all setup already and in a isolated environment nontheless.

## Download a VM manager

You can chose the one you want : [VirtualBox](https://www.virtualbox.org/wiki/Downloads) or [VMWare](https://www.vmware.com/products/workstation-player/workstation-player-evaluation.html)

For this tutorial I will be using VirtualBox as an example.

---
## Grab your Kali Linux Image

We give you two possibilities to get a running image with all the lab materials.  You can either [download an already set up image](#get-your-running-image-all-set-up) **or** [grab an image from the official Kali Linux website and set it up yourself](#setup-your-vm-by-yourself)

I suggest you try to set up your VM yourself following the guide below so you will be able to set up any VM in the future.

### Get your running image all set up

You can download the image [right here](/).

Once it's downloaded you can import it via your VM manager (there should be an import button that will setup everything for you).

When the installation is done try to connect with the following creditentials :
* user : kali
* pwd : kali

*(The keyboard is in US-en layout)*

---

### Setup your VM by yourself
Download the Kali Linux 2020.2 image on the [official offensive security website](https://www.offensive-security.com/kali-linux-vm-vmware-virtualbox-image-download/#1572305786534-030ce714-cc3b). Be sure to select the correct image depending on what VM Manager you installed (VirtualBox or VMWare).

Open VirtualBox and click on the big blue **New** button.

You can chose the name and the location of your VM as well as the OS. Chose Linux and select the Debian 32-bit version. You can keep the default amount of RAM allocated to the VM.

You will then need to link the image you downloaded. To that on the next screen select *Use an existing virtual hard disk drive* and click on the folder icon on the right. You will then need to add the new virtual disk by clicking *Add*.

Find where your image is and select it.

Create.

Done !

Try to run it and log in using the following creditentials : 
* user : kali
* pwd : kali

*(The keyboard is in US-en layout)*

## Bonus : Make yourself at home !

The VM is not really appealing as it is so I would suggest you set up an SSH connection so you can work easily via your terminal.

You will also surely need to pull/push github content on a private repo and if like me you're using a password manager there is a lot of chance you don't know your password. I therefore suggest you setup a ssh-key on your github account to be able to pull/push content without your github password.

### Setup SSH
First you need to make sure that the **SSH service** is running on your Kali Linux. 
```console
kali@kali:~$ sudo systemctl status ssh.service
# If it's not active you need to start it.
kali@kali:~$ sudo systemctl start ssh.service
```

Once it's done we need to setup the VM so it can connect to our own machine. To do so follow the steps bellow :

- Go to Virtualbox
- Open the *Settings* panel of your Kali Linux VM.
- Go to Network
- Select NAT and expand the *Advanced* dropdown.
- Click on *Port Forwarding*
- Add a new *Port Forwarding Rule* 
- You can leave *Name* empty. 
- In *Protocol* put **TCP**. 
- In *Host IP* put **127.0.0.1**. 
- In *Host Port* put **2222**. (or whatever unused port)
- You can leave *Guest IP* blanked.
- In *Guest Port* put **22** for ssh.
- Restart your VM.
- Try to log in using the following command : `ssh kali@127.0.0.1 -p 2222`. (The password is obviously still `kali`)

### Setup Github SSH-authentication

Refer to this [link](https://docs.github.com/en/github/authenticating-to-github/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent) to generate a ssh-key and link it to your repo.

Once it's done you need to change tour remote from `https://github.com/Donaschmi/my-awesome-repo.git` to `git@github.com:Donaschmi/my-awesome-repo.git`

When pulling/pushing you will be prompted to enter **the password your used to generate your sshkey**, not your github password.
