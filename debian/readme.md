# Debian
Install *Debian 8 "jessie"*. Select "SSH Server" and "Standard System Utilities".


## GRUB
Reduce the GRUB boot timeout.

```sh
vi /etc/default/grub
update-grub
```


## Updates
Install software updates.

```sh
apt-get update
apt-get upgrade
```


## Packages
Install software packages.

```sh
apt-get install zsh vim tmux tree sudo resolvconf apt-file
apt-get install make git subversion p7zip zip unzip
```


## Configuration
Inspect the current network configuration.

```sh
ip addr
ip route
cat /etc/resolv.conf
```

Set a static IP address.

```sh
vi /etc/network/interfaces
```

```nginx
auto eth0
iface eth0 inet static
  address 10.0.0.11
  netmask 255.255.255.0
  #gateway 10.0.0.1
  #dns-nameservers 8.8.8.8 8.8.4.4

#auto eth1
allow-hotplug eth1
iface eth1 inet dhcp
```

```sh
ifup --no-act eth0
ifup --no-act eth1
tmux -c "ifdown eth0; sleep 1; ifup eth0"
tmux -c "ifdown eth1; sleep 1; ifup eth1"
echo -e "nameserver 8.8.8.8\nnameserver 8.8.4.4" | resolvconf -a eth0.inet  # Only on Debian 7.9 and older.
```

Add an administrator account at the bottom of the *sudoers* file before the `#includedir` directive.

```sh
EDITOR=vi visudo
```

```diff
# Administrator account.
qis    ALL=(ALL:ALL) NOPASSWD:ALL
```

Change the login shell.

```sh
chsh -s /usr/bin/zsh qis
```

Create an SSH user directory.

```sh
install -o qis -g qis -d /home/qis/.ssh
```

Upload the configuration files.

```sh
scp .ssh/id_rsa.pub qis@debian:.ssh/authorized_keys
scp -r .vim .vimrc .zsh .zshrc .tmux.conf qis@debian:
```


## Fixed Network Interface Names
When dealing with VMs it is a good idea to bind network interface names to the MAC address.

Set `GRUB_CMDLINE_LINUX="net.ifnames=0 biosdevname=0"` in `/etc/default/grub`.

Create a configuration file `/etc/udev/rules.d/interfaces.rules`.

```
SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", KERNEL=="eth*", ATTR{address}=="00:15:5d:01:02:01", NAME="eth0"
SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", KERNEL=="eth*", ATTR{address}=="00:15:5d:01:02:02", NAME="eth1"
SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", KERNEL=="eth*", ATTR{address}=="00:15:5d:01:02:03", NAME="eth2"
```


## CMake
<https://cmake.org/download>

```sh
wget https://cmake.org/files/v3.5/cmake-3.5.2-Linux-x86_64.sh
yes | sudo sh cmake-3.5.2-Linux-x86_64.sh --prefix=/opt
sudo mv /opt/cmake-3.5.2-Linux-x86_64 /opt/cmake
```

NOTE: Add `/opt/cmake/bin` to the `PATH` environment variable.


## LLVM
<http://llvm.org/apt>

```sh
wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key | sudo apt-key add -
sudo vim /etc/apt/sources.list.d/llvm.list
```

```
deb http://llvm.org/apt/jessie/ llvm-toolchain-jessie-3.8 main
deb-src http://llvm.org/apt/jessie/ llvm-toolchain-jessie-3.8 main
```

```sh
sudo apt-get update
sudo apt-get install make clang-3.8 clang-format-3.8
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-3.8 50
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-3.8 50
sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-3.8 50
```


## GCC
<https://gcc.gnu.org/wiki/InstallingGCC>

```sh
sudo apt-get install build-essential
wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-6.1.0/gcc-6.1.0.tar.bz2
tar xf gcc-6.1.0.tar.bz2
cd gcc-6.1.0 && ./contrib/download_prerequisites
cd ..
mkdir objdir && cd objdir
../gcc-6.1.0/configure --prefix=/opt/gcc --enable-languages=c,c++ --disable-multilib
make
sudo make install
for i in gcc g++ c++ cpp gcc-ar gcc-nm gcc-ranlib; do
  sudo update-alternatives --install /usr/bin/$i $i /opt/gcc/bin/$i 50
done
```

NOTE: It might be a good idea to add `/opt/gcc/bin` to the `PATH` environment variable.
