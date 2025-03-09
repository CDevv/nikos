# nikOS
This is a hobby operating system for a x86_64 target using the Limine bootloader

## How to build?
You can build it on Ubuntu

* Install Dependencies
```
sudo apt install xorriso qemu-system-x86_64
```

* Install Limine:

You can install Limine from the [Limine Github repo](https://github.com/limine-bootloader/limine) and build it. Then put the limine directory into the root directory of this project. You should have the following directories:

```
.vscode
external
include
limine
src
```

* Run make

After that is done you can just run `make`

* Run

You can run it with qemu using `make run`. VSCode terminal seems to have a problem with that so just use your system's terminal when running `make run`
