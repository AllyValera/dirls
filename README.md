# dirls
As a final project, this repository implements a simple command-line program using the FileSystem Interface (API). The program serves as a limited version of the ls command, with default behavior equivalent to ls -R. It accepts command-line arguments, including flags such as -l, -a, -f, and -d, providing enhanced functionality for listing and organizing files and directories.

## Features
- **Recursive Listing**: Mimics ls -R behavior by default.
- **Customizable Output**: Accepts flags for options like -l, -a, -f, and -d.
- **Directory Focused**: Includes the option to print only directories using the -d flag.
- **Getopt Integration**: Efficiently parses command-line arguments using getopt.

## Known Limitations
- **Flag Application to First Directory Only**: Currently, the program applies flags to the first directory only when handling multiple directories with a single arglist.
- **Link Handling**: Doesn't follow links correctly and doesn't list the absolute or relative file for the link.

## Usage
The program should be able to accept multiple argument lists, each made up of a `-flagstring` plus a `dirname`. If the last `dirname` is left out, it should assume the current working directory (pwd).

**Examples**:
- `./dirls -la`
- `./dirls -l testdir -al ../anotherdir ../andanother`
- `./dirls -lf testdir -d`

If no directory is given, the program will use the current working directory. 

## Help Command
To display the usage information, use the following command:
```bash
./dirls.out -h
```

## Notes
- **Compilation Instructions**: Compile using "g++ -std=c++17 dirls.cpp -o dirls.out". This is necessary due to the lines where absolute paths are converted to relative ones.
- **Code Comments**: Several chunks of code from a previous implementation, where the assignment was misunderstood, have been commented out. They are left in the codebase just in case they are ever needed.

## Usage Disclaimer
This project is intended for educational purposes and as a reference for learning. It is not recommended for direct submission in academic settings, including but not limited to school assignments, projects, or assessments.

Using this code directly for academic purposes may violate academic integrity policies. It is crucial to understand and adhere to your institution's guidelines regarding the use of external resources in your academic work.

If you have a similar assignment, it is strongly advised to understand the requirements, guidelines, and policies of your educational institution before using or referencing this project. Misuse of this code for academic purposes is at your own risk.

The author of this project disclaims any responsibility for any consequences resulting from the misuse or inappropriate use of this code in an academic context.
