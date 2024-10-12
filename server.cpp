#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")  // Link with Ws2_32.lib for networking
int where{0};
const int new_buf_size = 4096;

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = WSAGetLastError();  // Windows-specific function to get error code
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

const size_t k_max_msg = 500;

void parse(char *big, SOCKET connfd);


static int32_t recv_full(SOCKET fd, char *buf, size_t n) {

    cout<<"at recv"<<endl;
    while (n > 0) {
        cout<<"n is "<<n<<endl;
        int rv = recv(fd, &buf[where], n,0);  // Use recv() in Windows
        // rv = recv(fd, buf, n, 0);
         
        cout<<"here enter"<<endl;
        if (rv <= 0) {
            return -1;  // error or unexpected EOF
        }
        parse(buf,fd);
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t send_all(SOCKET fd, const char *buf, size_t n) {
    while (n > 0) {
        int rv = send(fd, buf, n, 0);  // Use send() in Windows
        if (rv <= 0) {
            return -1;  // error
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}


void parse(char *big, SOCKET connfd) 
{
    cerr<<"where=  "<<where<<endl;
    fprintf(stderr, "%s\n","Reached");
    uint32_t len = 0;
    char ms[k_max_msg];
    if(where+4<new_buf_size)
    {
        cerr<<"where= "<<where<<endl;
        cerr<<"big[where]= "<<big[where]<<endl;
        memcpy(&len,&big[where], 4);
        where+=4;
    }
    cerr<<"len= "<<len<<endl;
    cerr<<"big[where]= "<<big[where]<<endl;

    if(where+len<new_buf_size)
    {
        memcpy(&ms,&big[where],len);
        ms[len]='\0';
        where+=len;
    }

    cerr<<"where= "<<where<<endl;

    printf("client says: %s\n", &ms);

    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    uint32_t rlen = (uint32_t)strlen(reply);
    cout<<"atleast here"<<endl;
    memcpy(wbuf, &rlen, 4);
    memcpy(&wbuf[4], reply, rlen);
    cout<<"hrere in send "<<endl;
    send_all(connfd, wbuf, 4 + rlen);
    //parse(big, where);

    //where=0;

}


static int32_t one_request(SOCKET connfd) {
    std::cerr<<"hereonerequest"<<std::endl;
    char rbuf[4+ k_max_msg + 1];
    errno = 0;

    int32_t err = recv_full(connfd, rbuf, k_max_msg);
     
     std::cout<<"err= "<<err<<std::endl;

    if (err) {
        if (errno == 0) {
            //parse(rbuf,0);
            msg("EOF");
        } else {
            msg("recv() error");
        }
        return err;
    }


   /*  uint32_t len = 0;
    memcpy(&len, rbuf, 4);  // assume little endian
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }

    // receive body
    err = recv_full(connfd, &rbuf[4], len);
    if (err) {
        msg("recv() error");
        return err;
    }

    rbuf[4 + len] = '\0';
    printf("client says: %s\n", &rbuf[4]);

    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);
    return send_all(connfd, wbuf, 4 + len); */
    return 1;
}


int main() {
    WSADATA wsaData;

    // Initialize Winsock (this is necessary in Windows)
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        die("WSAStartup failed");
    }

    // Create the socket
    SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == INVALID_SOCKET) {
        die("socket()");
    }

    // Allow address reuse
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));

    // Bind the socket
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 0.0.0.0 wildcard address

    int rv = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (rv == SOCKET_ERROR) {
        die("bind()");
    }

    // Listen for incoming connections
    rv = listen(fd, SOMAXCONN);
    if (rv == SOCKET_ERROR) {
        die("listen()");
    }

    std::cout<<"listening to requests "<<std::endl;
    while (1) {
        // Accept an incoming connection
        struct sockaddr_in client_addr = {};
        int client_len = sizeof(client_addr);
        SOCKET connfd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
        if (connfd == INVALID_SOCKET) {
            continue;  // accept failed, ignore and try again
        }


        while (1) {
            int32_t err = one_request(connfd);
            if (err) {
                break;  // error, break out of the loop
            }
        }

        // Close the connection
        closesocket(connfd);
    }

    // Clean up Winsock
    WSACleanup();

    return 0;
}
