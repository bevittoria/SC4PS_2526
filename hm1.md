# HomeWork 01 - CloudVeneto Setup Guide

Brief guide to create a Linux VM on CloudVeneto and prepare it to compile and run C programs.

## 1. Create the VM

Open the CloudVeneto dashboard:

```text
https://cloudveneto.ict.unipd.it/dashboard
```

Log in, select the course project, and create an SSH key pair from:

```text
Project -> Compute -> Key Pairs
```

Download the private key and protect it on your local machine:

```bash
mkdir -p ~/.ssh
mv sc4ps-key.pem ~/.ssh/
chmod 600 ~/.ssh/sc4ps-key.pem
```

Then create a new instance from:

```text
Project -> Compute -> Instances -> Launch Instance
```

Use an AlmaLinux image, select the key pair, choose the course network/flavor, and wait until the instance is `ACTIVE`.

## 2. Connect with SSH

If the VM has a reachable public IP:

```bash
ssh -i ~/.ssh/sc4ps-key.pem almalinux@VM_IP_ADDRESS
```

If access must go through the CloudVeneto gate, add this to `~/.ssh/config`:

```text
Host cloudveneto-gate
    HostName gate.cloudveneto.it
    User YOUR_CLOUDVENETO_USERNAME

Host sc4ps-vm
    HostName VM_IP_ADDRESS
    User almalinux
    IdentityFile ~/.ssh/sc4ps-key.pem
    ProxyJump cloudveneto-gate
```

Then connect with:

```bash
ssh sc4ps-vm
```

## 3. Install Development Tools

Update the system and install the tools needed for C programming:

```bash
sudo dnf update -y
sudo dnf groupinstall "Development Tools" -y
sudo dnf install git vim emacs nano gdb make -y
```

Check that GCC is installed:

```bash
gcc --version
```

## 4. Test a C Program

Create a working directory:

```bash
mkdir -p sc4ps/homework01
cd sc4ps/homework01
```

Create `hello.c`:

```c
#include <stdio.h>

int main(void)
{
    printf("Hello from CloudVeneto!\n");
    return 0;
}
```

Compile it:

```bash
gcc -Wall -Wextra hello.c -o hello
```

Run it:

```bash
./hello
```

Expected output:

```text
Hello from CloudVeneto!
```

The VM is now ready to compile and run C code.
