# Required steps

## Step 1 (Operating system dependent)

### Linux
* Open a terminal
* Install Bazel and clang-tools
```bash
  sudo apt-get install bazel clang-tools
  bazel version # Testing if bazel works
```
* Ensure GDB is installed
```bash
  sudo apt-get install gdb
  gdb --version
```

### Windows
* If you haven't already, install a debian-based WSL for Windows
  * E.G. https://www.microsoft.com/en-au/p/ubuntu-1804-lts/9n9tngvndl3q?rtc=1&activetab=pivot:overviewtab
* Go to the start menu and open "Ubuntu". You will be given a terminal. NOTE: THIS IS THE TERMINAL YOU WILL BE USING, NOT the windows terminal.
* Install Bazel
```bash
  sudo apt-get install bazel clang-tools
  bazel version # Testing if bazel works
```
* Ensure GDB is installed
```bash
  sudo apt-get install gdb
  gdb --version
```

### Mac OSX
* Install Bazel and clang-tools
```bash
  brew install bazel llvm
  bazel version # Testing if bazel works
```
* Ensure GDB is installed
```bash
  brew install gdb
  gdb --version
```

## Steps 2-5 (All operating systems)
* Step 2: Clone the repository
```bash
  git clone https://github.com/cs6771/comp6771 ~/comp6771
```  

* Step 3: Test you can build & run your code
```bash
  bazel build //assignments/wl:main
  bazel run //assignments/wl:main # Option 1
  ./bazel-bin/assignments/wl/word_ladder_test # Option 2
```  

* Step 4: Test you can build & run your tests
```bash
  bazel build //assignments/wl:word_ladder_test
  ./bazel-bin/assignments/wl/word_ladder_test
```  

* Step 5: Using GDB
```bash
  bazel build -c dbg //assignments/wl:main
  gdb ./bazel-bin/assignments/wl/main
```

* Step 6: IDE
  * Make sure you're using a capable IDE, e.g.
    * VSCode
    * CLion
  * If you struggle with the setup, use a text editor in the interim (Atom, Sublime)

# Optional (Setting up an IDE)

## Option 1: CLion - via the provided Linux Virtual Machine
* Install virtualbox (exact installation instructions depend on your OS)
* [Download our virtual machine](http://tiny.cc/comp6771vm) we've created for you. It has everything set up. Run it with virtualbox. The password is "comp6771".
* In virtualbox, file > import appliance > the file you downloaded. Make sure you set the CPU and RAM to something appropriate for your machine.
* Run `chmod a+x ~/.CLion2019.*/config/plugins/clwb/gdb/gdbserver`
* Restore [the settings](http://tiny.cc/comp6771clionsettings) (file > import settings)
* [Create a jetbrains account](https://www.jetbrains.com/shop/eform/students) (username/password) for free to obtain a Clion license
* Modify the line starting with "url" in ~/Documents/6771/.git/config


## Option 2: CLion - Linux 

Download and install clion from [jetbrains website](https://www.jetbrains.com/clion/download/). Sign up using your student email to get a free copy.

After that is complete:

* Open Clion
* Configure plugins
  * Clion menu: Configure > plugins
  * Install bazel, clang-tidy, and clang-format.
* Restart clion
* Configure Clion for Bazel
  * Clion menu: Configure > settings > bazel settings > bazel binary
  * Set location to your bazel you downloaded and installed (likely /usr/bin/bazel)
* Restore [my settings](http://tiny.cc/comp6771clionsettings)
  * Clion menu: File > import settings
* Run `chmod a+x ~/.CLion2019.*/config/plugins/clwb/gdb/gdbserver`
* Import your project:
  * Clion menu: File > import bazel project > course repository

## Option 3: VSCode - Windows & OSX

### Setting Up Environment
* Install Visual Studio Code - Insiders
* Install these VS Code Extensions:
  * C/C++
  * vscode-bazel
  * Remote Development
* Follow these getting started guide:
  * https://code.visualstudio.com/docs/cpp/config-wsl
    * Up until the 'Set up your Linux environment', but feel free to read the whole thing
  * https://code.visualstudio.com/docs/remote/wsl
    * Just the 'Getting Started' section should be sufficient.
* Install Bazel on WSL:
  * https://docs.bazel.build/versions/master/install-ubuntu.html
### Editing
  * Clone this git repo
  * cd to cloned repo directory
  * code-insiders ./
### Building
* Open command palette (Ctrl + Shift + P), type "bazel"
* Select "Bazel: Build Package" if you want to build a package.
  * Example: building //lectures/week1 will build every single target listed in the BUILD file
* Select "Bazel: Build Package Recursively" if you want to build a package recursively.
  * Example: building //lectures will build //lectures/week1, //lectures/week2, etc.
* Select "Bazel: Build Target" if you want to build single targets.
### Running
* Open terminal
* Navigate to the project root folder
* Example: 
  ```bash
    bazel build //lectures/week1:factorial
    ./bazel-bin/lectures/week1/factorial
  ```


  
### Using Clang-Tools outside of CLion

To use tools such as `clang-tidy`, `clang-format` outside of CLion, 
or to use alternative IDEs relying on tools such as `clangd` or `rtags`, 
you will need a compilation database. To set one up, just run

    ./gen_compile_commands.sh

from inside the repository directory,
which should result in a `compile_commands.json` file in the root of the project.

This should work on most POSIX systems, but it needs bash to run.
