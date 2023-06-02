#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char *argv[])
{
    int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addr_send;
    addr_send.sin_family = AF_INET;
    addr_send.sin_addr.s_addr = inet_addr(argv[1]);
    addr_send.sin_port = htons(atoi(argv[2]));

    struct sockaddr_in addr_recv;
    addr_recv.sin_family = AF_INET;
    addr_recv.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_recv.sin_port = htons(atoi(argv[3]));

    bind(receiver, (struct sockaddr *)&addr_recv, sizeof(addr_recv));

    fd_set fdread, fdtest;
    char mes[1024];
    FD_ZERO(&fdread);
    FD_SET(STDIN_FILENO, &fdread);
    FD_SET(receiver, &fdread);
    while (true)
    {
        fdtest = fdread;
        int sel = select(receiver + 1, &fdtest, NULL, NULL, NULL);
        if (sel < 0)
        {
            cout << "select() failed\n";
        }
        if (FD_ISSET(STDIN_FILENO, &fdtest))
        {
            string buf;
            getline(cin, buf);
            sendto(sender, buf.c_str(), strlen(buf.c_str()), 0, (struct sockaddr *)&addr_send, sizeof(addr_send));
        }
        if (FD_ISSET(receiver, &fdtest))
        {
            int ret = recvfrom(receiver, mes, sizeof(mes), 0, NULL, NULL);
            if (ret == -1)
            {
                perror("recvfrom() failed\n");
                continue;
            }

            mes[ret] = '\0';
            cout << mes << endl;
        }
    }
}
