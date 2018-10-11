#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <time.h>
#include <math.h>


using namespace std;

string gen_nonce(){
    string out = "";
    for(int i = 0; i < 10; i++){
        out.push_back(char(rand() % 128));
    }
    return out;
}

int getprime(){
    bool done = false;
    int test;
    int i = 0;
    int w = 0;
    while (!done) {
        test = rand();
        if(test == 2 || test == 3){
            done = true;
        }
        else if(test % 2 == 0 || test % 3 == 0){
            continue;
        }
        else{
            i = 5;
            w = 2;
            while (i * i <= test){
                if(test % i == 0){
                    break;
                }
                i += w;
                w = 6 - w;
            }
            if(i * i > test){
                done = true;
            }
        }
    }
    return test;
}

int diffie(int p, int a, int r){
    long long int calc = pow(r, a);
    int out = calc % p;
    return out;
}

int main(int argc, char *argv[])
{
    //for the server, we only need to specify a port number
    srand(time(NULL));
    string s_ID = "A";
    int p = 23;
    int g = 9;
    int s1 = rand() % 9 + 1;
    
    //grab the IP address and port number, statically allocated
    char *serverIp = "127.0.0.1"; int port = 1234;
    
    //create a message buffer
    char msg[1500];
    
    //setup a socket and connection tools
    struct hostent* host = gethostbyname(serverIp);
    sockaddr_in sendSockAddr;
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr =
    inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect to KDC
    bool conn = false;
    while (!conn) {
        int status = connect(clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
        if(status >= 0)
        {
            conn = true;
        }

    }
    //Send messages for diffie and completing
    
    //Start of diffie
    int m1 = long(pow(g, s1)) % p;  //Needs to be sent
    int r = 0;
    int bytesRead, bytesWritten = 0;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);

    //Sending and receiving mod results
    string data = "A";
    memset(&msg, 0, sizeof(msg));//clear the buffer
    strcpy(msg, data.c_str());
    bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
    
    data = to_string(m1);
    memset(&msg, 0, sizeof(msg));//clear the buffer
    strcpy(msg, data.c_str());
    bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
    cout << "Awaiting server response..." << endl;
    memset(&msg, 0, sizeof(msg));//clear the buffer
    
    bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);
    cout << "Server: " << msg << endl;
    
    r = atoi(msg);
    int s2 = diffie(p, s1, r);
    cout << s2 << endl;
    data = "exit";
    memset(&msg, 0, sizeof(msg));//clear the buffer
    strcpy(msg, data.c_str());
    send(clientSd, (char*)&msg, strlen(msg), 0);
    
    //End initial server connection
    close(clientSd);
    //Obtained Kas
    
    //Alice now reaches out to KDC
    //Main Needham Section
    
    return 0;
}