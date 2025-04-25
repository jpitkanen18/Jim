# Jim - A "Revolutionary" "Text Editor"

![image](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![image](https://img.shields.io/badge/apple%20silicon-333333?style=for-the-badge&logo=apple&logoColor=white)

Jim is a lightweight Vim-clone written in C++ with manual memory management. Jim offers basic text editing functionalities. While it’s still a proof-of-concept with some rough edges, it explores the balance between low-level control and performance.

![image](images/jim.png)

### Core Features:
- Manual memory management (expect some rough edges).
- A combination of C and C++ coding conventions (An unholy combo of classes, templates and `malloc()`).
- Basic text editing features with a focus on speed and efficiency.
- WRITE and QUIT commands!! (This took a while)

**Note**: This is a proof-of-concept/exploratory project, it’s not intended for production use yet (and will clear your files on save if you mistreat it).

## Building
### Dependencies
- C++ compiler
- ncurses
- zsh (Bash is not welcome here)

### Build
```bash
$ chmod +x build.zsh
$ ./build.zsh
```

### Running
```bash
./jim YOUR_FILE_NAME
```

## Why I Built Jim:
I wanted to explore the efficiency gains and challenges of manual memory management (and the usefulness of abstractions that remove the need for it) in a practical project. This text editor helped me learn about the inner workings of C++ and Vim.

and also

I was left unattended with a C++ compiler and a terminal emulator.
