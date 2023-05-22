// #include <bits/stdc++.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <arpa/inet.h>

// using namespace std;

// int main(int argc, char *argv[])
// {
//     int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

//     int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

//     struct sockaddr_in addr_send;
//     addr_send.sin_family = AF_INET;
//     addr_send.sin_addr.s_addr = inet_addr(argv[1]);
//     addr_send.sin_port = htons(atoi(argv[2]));

//     struct sockaddr_in addr_recv;
//     addr_recv.sin_family = AF_INET;
//     addr_recv.sin_addr.s_addr = htonl(INADDR_ANY);
//     addr_recv.sin_port = htons(atoi(argv[3]));

//     bind(receiver, (struct sockaddr *)&addr_recv, sizeof(addr_recv));

//     fd_set fdread;
//     char mes[1024];
//     FD_ZERO(&fdread);
//     FD_SET(STDIN_FILENO, &fdread);
//     FD_SET(receiver, &fdread);
//     while (true)
//     {
//         int sel = select(receiver + 1, &fdread, NULL, NULL, NULL);
//         if (sel < 0)
//         {
//             cout << "select() failed\n";
//         }
//         if (FD_ISSET(STDIN_FILENO, &fdread))
//         {
//             string buf;
//             getline(cin, buf);
//             sendto(sender, buf.c_str(), strlen(buf.c_str()), 0, (struct sockaddr *)&addr_send, sizeof(addr_send));
//         }
//         if (FD_ISSET(receiver, &fdread))
//         {
//             int ret = recvfrom(receiver, mes, sizeof(mes), 0, NULL, NULL);
//             if (ret == -1)
//             {
//                 perror("recvfrom() failed\n");
//                 break;
//             }
//             else
//             {
//                 mes[ret] = '\0';
//                 cout << mes;
//             }
//         }
//     }
// }

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>

int main(int argc, char *argv[])
{
    // Khai bao socket sender
    int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(argv[1]);
    saddr.sin_port = htons(atoi(argv[2]));

    // Khai bao socket receiver
    int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in raddr;
    raddr.sin_family = AF_INET;
    raddr.sin_addr.s_addr = htonl(INADDR_ANY);
    raddr.sin_port = htons(atoi(argv[3]));

    bind(receiver, (struct sockaddr *)&raddr, sizeof(raddr));

    fd_set fdread, fdtest;
    FD_ZERO(&fdread);
    FD_SET(STDIN_FILENO, &fdread);
    FD_SET(receiver, &fdread);

    char buf[256];

    while (1)
    {
        fdtest = fdread;
        int ret = select(receiver + 1, &fdtest, NULL, NULL, NULL);
        if (ret < 0)
        {
            perror("select() failed");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &fdtest))
        {
            fgets(buf, sizeof(buf), stdin);
            sendto(sender, buf, strlen(buf), 0,
                   (struct sockaddr *)&saddr, sizeof(saddr));
        }

        if (FD_ISSET(receiver, &fdtest))
        {
            ret = recvfrom(receiver, buf, sizeof(buf), 0, NULL, NULL);
            if (ret < sizeof(buf))
                buf[ret] = 0;
            printf("Received: %s\n", buf);
        }
    }

    return 0;
}