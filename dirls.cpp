// Ally Valera 
// CISC 3595
// Recursive Directory Listing - dirls

// NOTE: Compile using "g++ -std=c++17 dirls.cpp -o dirls.out". This is because of the lines where I convert an absolute path to a relative one
// NOTE: I commented out chunks of code from my previous implementation where I misunderstood the assingment. I left them in just in case I needed them.

// Program using the FileSystem Interface (API) to develop a program that is a limited version of ls 
// but does ls -R by default. that takes arguments from the command line that includes a set of flags 
// such as -l, -a, -f, and -d that prints only the directory. 

#include <iostream>
#include <unistd.h> // For getopt to parse command line args
#include <dirent.h> // For opendir, readdir, closedir
#include <queue> // For easy management of non-option args
#include <sys/stat.h> // For ls -l implementation
#include <pwd.h> // For ls -l implementation
#include <grp.h> // For ls -l implementation
#include <string> // For getting permission string and long listing string for ls -l
#include <cstdio> // For opening files in ls -l implementation
#include <limits.h> // For handling symbolic links with realpath()
#include <stdlib.h> // For handling symbolic links with realpath()
#include <cstring> // String comparison in ls -l implementation
#include <filesystem> // For converting an absolute path to a relative one for symlinks

using namespace std;

// Class implementation loosely based on:
// https://stackoverflow.com/questions/65153208/c-cant-seem-to-filter-out-the-dot-files-in-my-recursive-directory-listing-pro
class dirls {
    public:
        // Main Functions
        dirls(queue<string> path, bool a, bool d, bool f, bool l, bool h);
        void execute(); // What functions to go through based on options?
        void usage(); // Option h
        void longListing(char* pathName); // Option l
        void directoryInformation(char* pathName); // Option d
        void recursiveListing (char* pathName); // Option R (default)
        void includeDotFiles (char* pathName); // Option a
      
    private:
        queue<string> path;
        bool aflag;
        bool dflag;
        bool fflag;
        bool lflag;
        bool hflag;
};

// Class constructor
dirls::dirls(queue<string> paths, bool a, bool f, bool d, bool l, bool h) {
    path = paths;
    aflag = a;
    fflag = f;
    dflag = d;
    lflag = l;
    hflag = h;
}

// Decides what option functions to go through for each flag
// Deals with multiple argument lists, as it focuses on the first item in the queue and pops its out when it has been dealt with
// Then, it proceeds to the next item that has moved to the front of the queue
void dirls::execute() {
    // Check for contraditions
    // -fd doesn't make sense and is contradictory because -d means don't recurse, just list the local named directory to see if it exists.
    if (fflag && dflag) {
        cout << "Error: -fd and -df contradict each other. Run the command again with different options." << endl;
        return;
    }

	while (!path.empty()) {
        // Takes the name of the first item in queue
		char* pathName = (char*) path.front().c_str(); 

        // If there are multiple items in the queue
		if (path.size() >= 1) {
			// cout << pathName << ":" << endl; 
		}

        // Check flags in a specific order to consider combinations of flags
        
        // ls -h
		if (hflag) {
			usage(); 
			path.pop();
			continue; 
		}

        // // ls -l
		// if (lflag) {
        //     longListing(pathName);
		// 	path.pop();
		// 	continue;
		// }

        // // ls -a
		// if (aflag) {
        //     includeDotFiles(pathName);
		// 	path.pop();
		// 	continue; // Continues to the next iteration of the loop
		// }

        // ls -d
		if (dflag) {
            directoryInformation(pathName);
			path.pop();
			continue;
		}

        // Note for ls -f:
        // -f follow symbolic links. NEVER follow symbolic links unless the -f flag is enabled. Use realpath to determine if the symbolic link points to a directory.
        // It is possible to have a recursive link, YOU DO NOT NEED TO HANDLE THIS CASE.

		// No flags are enabled
        // Program does ls -R by default, as specified by instructions
		recursiveListing(pathName);
		path.pop();
	}
}

// Outputs how to use the command
void dirls::usage() {
    cout << "Usage: dirls [(-[adflh]+) (dir)]*" << endl;
    cout << "\t-a: include dot files" << endl;
    cout << "\t-f: follow symbolic links" << endl;
    cout << "\t-d: only this directory" << endl;
    cout << "\t-l: long form" << endl;
    cout << "\t-h: prints this message" << endl;
}

// Based on these implementations of ls in C:
// https://stackoverflow.com/questions/20450556/linux-ls-l-implementation-using-file-descriptors-in-c-c
// https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
// Function for ls -l
// The -l option signifies the long list format.
void dirls::longListing(char* pathName) {
	DIR* dir; 

    // If there is no item, look at the current directory
	if (pathName == NULL || dflag) {
		dir = opendir("."); 
	} 
    // Else, use that item as a directory
    else {
		dir = opendir(pathName); 
	}

    // https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-readdir-read-entry-from-directory
	struct dirent *dirent; 

    // Listing info to append permissions, username, etc. to
    string listInfo;
	
    // While we are not at the end of the directory stream
	while((dirent = readdir(dir)) != NULL) {
        // ls -ld implementation
        if (dflag) {
            // Compare current item to the directory we're trying to output the name of
            // If the strings do not match, continue to the next loop
            if (strcmp(dirent->d_name, pathName) != 0) {
                continue;
            }
        }

        // Clear out string holding info for each loop
        listInfo.clear();

        struct stat info; 
        lstat(dirent->d_name, &info);

        // Getting the username and group name to be displayed
        // First, get the user id 
        // Then, convert to the actual names of the user and group
		struct passwd* userName = getpwuid(info.st_uid);
		struct group* groupName = getgrgid(info.st_gid); 

        // Determing type and permissions of item being listed

        // https://www.gnu.org/software/libc/manual/html_node/Testing-File-Type.html
        // Directory?
        if (S_ISDIR(info.st_mode)) {
            listInfo.push_back('d');
        } 
        // Character special file?
        else if (S_ISCHR(info.st_mode)) {
            listInfo.push_back('c');
        }
        // Block special file?
        else if (S_ISBLK(info.st_mode)) {
            listInfo.push_back('b');
        }
        // Regular file?
        else if (S_ISREG(info.st_mode)) {
            listInfo.push_back('-');
        }
        // FIFO special file or a pipe?
        else if (S_ISFIFO(info.st_mode)) {
            listInfo.push_back('f');
        }
        // Symbolic link?
        else if (S_ISLNK(info.st_mode)) {
            listInfo.push_back('l');
        }
        // Socket?
        else if (S_ISSOCK(info.st_mode)) {
            listInfo.push_back('s');
        }

        // https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
        // Read for owner
        if (info.st_mode & S_IRUSR) {
            listInfo.push_back('r');
        } else {
            listInfo.push_back('-');
        }

        // Write for owner
        if (info.st_mode & S_IWUSR) {
            listInfo.push_back('w');
        } else {
            listInfo.push_back('-');
        }

        // Execute for owner
        if (info.st_mode & S_IXUSR) {
            listInfo.push_back('x');
        } else {
            listInfo.push_back('-');
        }

        // Read for group
        if (info.st_mode & S_IRGRP) {
            listInfo.push_back('r');
        } else {
            listInfo.push_back('-');
        }

        // Write for group
        if (info.st_mode & S_IWGRP) {
            listInfo.push_back('w');
        } else {
            listInfo.push_back('-');
        }

        // Execute for group
        if (info.st_mode & S_IXGRP) {
            listInfo.push_back('x');
        } else {
            listInfo.push_back('-');
        }

        // Read for other
        if (info.st_mode & S_IROTH) {
            listInfo.push_back('r');
        } else {
            listInfo.push_back('-');
        }

        // Write for other
        if (info.st_mode & S_IWOTH) {
            listInfo.push_back('w');
        } else {
            listInfo.push_back('-');
        }

        // Execute for other
        if (info.st_mode & S_IXOTH) {
            listInfo.push_back('x');
        } else {
            listInfo.push_back('-');
        }

        // EXTRA CREDIT
        // Getting the last modified time
        // Referenced the following implementations:
        // https://stackoverflow.com/questions/13542345/how-to-convert-st-mtime-which-get-from-stat-function-to-string-or-char
        // https://cplusplus.com/reference/ctime/strftime/
        struct tm *time = localtime(&info.st_mtime);
        char timeStr[20];
        strftime(timeStr, sizeof(timeStr), "%b %e %R", time);

        // ls -ld option
        // ONLY output info about the directory
        if (dflag) {
            cout << listInfo << " " << info.st_nlink << " " << userName->pw_name << " " << groupName->gr_name << " " << info.st_size << " " << timeStr << "\t " << dirent->d_name << endl; 
            return;
        }

        // Default ls -l OR ls -la if aflag is set to true OR ls -lf if fflag is set to true
        // Default: Print all the info for items without a .
        // ls -la: Prints all info for all items
        // ls -lf: Prints arrows to signify symbolic links; uses realpath
        if (string(dirent->d_name)[0] != '.' || aflag) {
            cout << listInfo << " " << info.st_nlink << " " << userName->pw_name << " " << groupName->gr_name << " " << info.st_size << "\t " << timeStr << " " << dirent->d_name;
            
            // ls -lf implementation
            if (fflag && dirent->d_type == DT_LNK) {
                // EXTRA CREDIT
                // Getting the relative path from the absolute path
                // https://en.cppreference.com/w/cpp/filesystem
                // https://en.cppreference.com/w/cpp/filesystem/path/generic_string
                filesystem::path p(realpath(dirent->d_name, NULL));
                string relativePath = relative(p, filesystem::current_path());
                cout << " -> " << relativePath;
            } 

            cout << endl;
        }
	}

	closedir(dir); 
}

// With the -d option, ls lists the directory itself
void dirls::directoryInformation(char* pathName) {
	// If no directory is given, output a '.' for the current directory
    if (pathName == NULL) {
		cout << "." << endl;
		return; 
	}

    // If a directory is given, output that name
	cout << pathName << endl;
}

// ls -R option flag lists directory tree recursively
// Loosely based on the following implementations:
// https://github.com/arya-oss/OsLabCodes/blob/master/shellCmd/ls_cmd.cpp
// https://stackoverflow.com/questions/8436841/how-to-recursively-list-directories-in-c-on-linux
void dirls::recursiveListing (char* pathName) {
    queue<string> directoryList; // Queue of directories remaining to be listed out
    DIR *dir;

    // If there is no item, use the current directory 
	if (pathName == NULL) {
		pathName = (char*)".";
		dir = opendir("."); 
	} 
    // Else, use that item as the directory
    else {
		dir = opendir(pathName); 
	}	

    // Invalid directory
	if (dir == NULL) {
		cout << pathName << " cannot be accessed" << endl; 
		return; 
	}

    // https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-readdir-read-entry-from-directory
    struct dirent *dirent;

    // Output the path of the item currently being looked at
	cout << string(pathName) + ":" << endl; 

	struct stat info; 
	lstat(pathName, &info); 

    if(lflag) {
        longListing(pathName);
    }

    // While we are not at the end of the directory stream
	while ((dirent = readdir(dir)) != NULL) {
        // If it is a directory
		if (dirent->d_type == DT_DIR) {
			// NEVER follow '.' or '..' or else there will be an infinite loop
			if (string(dirent->d_name) == "." || string(dirent->d_name) == "..") {
                if(aflag && !lflag) { // If option a was specified, list '.' and '..'
                    cout << dirent->d_name << " ";
                }
				continue; // Continue to the next iteration of the loop
			}

            // Includes path of any directories inside the current directory
            string newPath = string(pathName) + "/" + dirent->d_name;
            // Push the path to the queue of directories
			directoryList.push(newPath); 

            if (!lflag){
                cout << dirent->d_name << " "; 
            }

		} else if (!lflag) {
            // Output the item in the directory if it is not a directory
			cout << dirent->d_name << "\t"; 
		}
	}

	// Formatting
    cout << endl;
    cout << endl;

    // Keep going recursively until the queue of directories is empty
	while (!directoryList.empty()) {
        // Convert the directoryList item into a char* so it can be used as an arg
		recursiveListing((char*)directoryList.front().c_str());

        // pop off the front element of the queue, as it has already been explored
		directoryList.pop();
	}

	closedir(dir);	
}

// ls -a option flag lists all files including hidden files starting with '.'
void dirls::includeDotFiles (char* pathName) {
    DIR* dir; 

    // If there is no item, use the current directory
	if (pathName == NULL) {
		dir = opendir("."); 
	} 
    // Else, use the item as a directory 
    else {
		// If dflag is set to true, just print the path 
		if (dflag) {
			cout << pathName << endl; 
			return; 
		}
		
		dir = opendir(pathName); 
	}

    // Invalid directory
	if (dir == NULL) {
		cout << pathName << " cannot be accessed" << endl; 
		return;  
	}

	struct dirent *dirent; 

	struct stat info; 
	lstat(pathName, &info); 

    // While we are not at the end of the directory stream
	while((dirent = readdir(dir)) != NULL) {
		cout << dirent->d_name << endl; 
	}

	closedir(dir); 
}

int main(int argc, char* argv[]) {
    // This implementation of parsing the command line arguments was based on these examples:
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
    // https://www.youtube.com/watch?v=SjyR74lbZOc

    // Bools for the different options
    bool a;
    bool f;
    bool d;
    bool l;
    bool h;

    int c;

    // The std::queue class is a container adapter that gives the programmer the functionality of a queue, specifically, a FIFO (first-in, first-out) data structure.
    // Makes management of non-option arguments easier
    // https://stackoverflow.com/questions/59013702/how-to-make-a-queue-of-strings-using-user-input
    queue<string> paths;

    // Loop through all arguments, attaching flags to only their respective directories
    while (optind < argc || (optind == 1 && argc == 1) ) {
        a = false;
        f = false;
        d = false;
        l = false;
        h = false;
        // When getopt returns -1, there are no more options present
        // A switch statement is used to dispatch on the return value from getopt
        while ((c = getopt (argc, argv, "+afdlh")) != -1) {
            // did I see a directory ?
            switch (c) {
                case 'a':
                    a = true;
                    break;
                case 'f':
                    f = true;
                    break;
                case 'd':
                    d = true;
                    break;
                case 'l':
                    l = true;
                    break;
                case 'h':
                    h = true;
                    break;
                case '?':
                    cout << "An unknown option was encountered." << endl;
                    return 1;
                    break;
                default:
                    break;
            }
        }

        // After getopt returns -1, optind then holds the index of the next argv element that is not an option
        // https://man7.org/linux/man-pages/man3/getopt.3.html

        if(optind < argc && argv[optind][0] != '-'){
            paths.push(argv[optind]);
        }

        if (paths.empty()) {
            paths.push(".");
        }

        dirls obj = dirls(paths, a, f, d, l, h);
        obj.execute();

        paths = queue<string>();

        optind++;
    }

    // // A second loop is used to process the remaining non-option arguments
    // // You can use this to grab all of the directories and files that are interleaved with the option arguments.
    // for (int i = optind; i < argc; i++) {
    //     // Add the non option arguments to the paths vector
    //     // if the first char is not a directory
    //     paths.push(argv[i]);
    // }

    // // No directories given
    // if (paths.empty()) {
    //     paths.push(".");
    // }

    // // Create an object of the 'dirls' class and initialize it
    // dirls obj = dirls (paths, a, f, d, l, h);
    
    // obj.execute(); // function to execute function based on options decided above

    // while (optind < argc && argv[optind][0] != '-') {
    //     paths.push(argv[optind]);
    //     optind++;
    // }
    // if (!paths.empty()) {
    //     // initialize the object
    //     obj = dirls (paths, a, f, d, l, h);
    //     obj.execute(); // function to execute function based on options decided above
    //     // Reset everything
    //     a = false;
    //     f = false;
    //     d = false;
    //     l = false;
    //     h = false;

    //     paths = queue<string>();
    // } 

    return 0;
}