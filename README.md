    # Darkside

For know it's small piece of software at mine beginning of writing FUD (full-undetectable) malware - limited reverse shell for Windows written in C/C++ and Python control panel.

## Getting Started

Download source from github


```
git clone https://github.com/Djkusik/Darkside.git
```

### Prerequisites

We will need cross compiler (if we're using Linux) - for these who use one based on Debian:

```
apt-get install mingw-w64-common mingw-w64-i686-dev mingw-w64-tools mingw-w64-x86-64-dev i686-w64-mingw32-g++
```

Helpful would be virtual machine with Windows for testing.

## Usage

First we need to open our server which will be used for controlling bots:

```
python3 botnet_server.py <LHOST> <LPORT>
```

Then run executable at Windows (for now it isn't run on startup). We can do it from cmd when being in the same folder:

```
.\darkside.exe
```

### Disclaimer

This software has been made for educational purposes and could be used for CyberSecurity Red Teams. I don't promote malicious practices and will not be responsible for any illegal activities. Use it at your own risk.