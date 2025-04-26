# Lebirun (C Version)
### Lebirun is a fun project i wanted to do to make a OS (that shouldn't be used in productions). This OS was formerly made with Cosmos in C#, now it's made in C and Assembly.
### I recommend running this OS in a hypervisor instead of your main machine, who knows what would happen

# How to compile (Tested with Debian 12.10 amd64 WSL)
### You have to first execute these commands:
```bash
sudo dpkg --add-architecture i386
sudo apt update
```
(This adds i386 support for dpkg)
### Then you have to do this to install the software needed:
```bash
sudo apt-get install gcc-multilib libc6-dev-i386 nasm xorriso grub-pc-bin grub-common
```
### After that, cd to the Lebirun directory and execute "make"
### And then you've compiled Lebirun. If you want to recompile with your changes, execute "make clean", and afterwards execute "make".
### If you want, you can also run ./build.sh in the same directory where you placed Lebirun in.
