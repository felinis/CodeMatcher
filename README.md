# CodeMatcher

PlayStation 2 MDEBUG (pre-DWARF v1) symbols extraction utility and code function matcher.


## How to use

To set up the environment, do the following:

1. Install the PS2 EE compiler from:
	https://archive.org/download/SNSystemsProDGPs2/ProDGPs2usrLocalSceFiles.zip

2. Extract it to your C:/ drive so that the path looks like: C:/usr/local/...

3. Prepare the ELF of the PS2 game you would like to code match, and your decompiled source code or object files to match against.

Now you can use one of the following modes of operation.

### Compile & compare source tree

`CodeMatcher.exe <elf_file> --compile <source_tree_absolute_path>`

CodeMatcher will invoke the PS2 EE compiler on your source tree and match it with the corresponding functions in the ELF file. It will show you which functions match and which ones don't.

### Match object file

`CodeMatcher.exe <elf_file> --match <object_file>`

CodeMatcher will match the object file with the corresponding functions in the ELF file. It will show you which functions match and which ones don't.

### Dump symbols

`CodeMatcher.exe <elf_file> --dump`

`CodeMatcher.exe <elf_file> --dump > output.txt`

CodeMatcher will dump the symbols from the ELF file to stdout, or to the specified file.


## How to build

### Linux setup

```bash
sudo apt install cmake libboost-all-dev
```

### Windows setup

It is recommended to use a package manager like Chocolatey to install the required packages, otherwise you will have to build Boost yourself.

```powershell
choco install cmake boost
```

### Building

Clone the repository and run the following commands

```bash
cd CodeMatcher
mkdir build
cd build
cmake ..
cmake --build .
```


## Images

These examples show the code matching process for the game "Sly Cooper and the Thievius Raccoonus", whose prototype contains STABS/MDEBUG debug symbols.
I got a few functions matching, but there is still a lot of work to do.

![image](https://user-images.githubusercontent.com/94763702/202547405-e7fe47dd-f6ae-49f1-b918-3cabe1f6cbee.png)
<img width="464" alt="first_match" src="https://user-images.githubusercontent.com/94763702/201522165-82a90021-f36d-4b39-b072-b22467768dbf.png">
<img width="674" alt="code_matcher_output" src="https://user-images.githubusercontent.com/94763702/201522168-fd5cfcad-009e-487f-8cc7-2f18d69edda9.png">
