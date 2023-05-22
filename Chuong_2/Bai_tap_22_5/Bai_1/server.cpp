#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

int main()
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
    addr.sin_port = htons(9000);

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

    fd_set fdread;

    vector<int> clients;
    int maxdp;

    char buf[1024];
    while (true)
    {
        FD_ZERO(&fdread);
        FD_SET(listener, &fdread);
        maxdp = listener + 1;
        for (int i : clients)
        {
            FD_SET(i, &fdread);
            if (i + 1 > maxdp)
                maxdp = i + 1;
        }

        int sel = select(maxdp, &fdread, NULL, NULL, NULL);
        if (sel < 0)
        {
            perror("select() failed\n");
            continue;
        }
        if (FD_ISSET(listener, &fdread))
        {
            int client = accept(listener, NULL, NULL);
clients.push_back(client);
            char* numclient = itoa(clients.size());
            send(client, );
                

            continue;
        }
        for (int i = 0; i <= clients.size(); i++)
        {
            if (FD_ISSET(clients[i], &fdread))
            {
                int ret = recv(clients[i], buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    clients.erase(clients.begin() + i);
                    break;
                }
                else
                {
                    buf[ret - 1] = '\0';
                    if (strcmp(buf, "exit") != 0)
                    {
                        cout << buf << endl;
                    }
                    else
                    {
                        clients.erase(clients.begin() + i);
                        break;
                    }
                }
            }
        }
    }

    close(listener);
}