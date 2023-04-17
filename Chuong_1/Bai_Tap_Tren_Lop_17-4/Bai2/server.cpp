#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char *argv[])
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(addr);

    int client = accept(listener, (struct sockaddr *)&clientAddr, (unsigned int *)&clientAddrLen);

    const char *msg = "Hello client!\n";
    send(client, msg, strlen(msg), 0);
    string s = "";
    while (true)
    {
        char buf[256];
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            break;
        }
        buf[ret] = '\0';
        s += buf;
    }
    int i, count = 0, j = 0;
    const char *c = "0123456789";
    for (i = 0; i < s.length(); i++)
    {
        if (s[i] != c[j])
        {
            j = 0;
            continue;
        }
        else
        {
            if (j < 9)
            {
                j++;
            }
            else
            {
                j = 0;
                count++;
            }
        }
    }
    cout << "Số lần xuất hiện của chuỗi là : " << count << endl;

    close(client);
    close(listener);
}