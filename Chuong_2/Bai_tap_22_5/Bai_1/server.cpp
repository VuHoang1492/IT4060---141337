#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
using namespace std;

char *formatText(char *src, char *des)
{
    int length = strlen(src);
    des = new char[length];
    int j = 0;
    for (int i = 0; i < length; i++)
    {

        // ki tu dau tien
        if (i == 0 && src[i] == ' ')
        {
            continue;
        }
        if (i == 0 && src[i] != ' ')
        {
            des[j] = toupper(src[i]);
            j++;
            continue;
        }

        // ki tu tiep theo
        if (src[i] != ' ' && src[i - 1] == ' ')
        {
            des[j] = toupper(src[i]);
            j++;
            continue;
        }
        if (src[i] != ' ' && src[i - 1] != ' ')
        {
            des[j] = tolower(src[i]);
            j++;
            continue;
        }

        // kiem tra khoang trang
        if (src[i] == ' ' && src[i - 1] != ' ')
        {
            des[j] = ' ';
            j++;
            continue;
        }
        if (src[i] == ' ' && src[i - 1] == ' ')
        {
            continue;
        }

        // Ki tu cuoi cung
        if (i == length - 1)
        {
            des[j] = '\0';
            continue;
        }
    }
    return des;
}

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
            string numclient = to_string(clients.size());
            string mes = "Xin chao. Hien co " + numclient + " dang ket noi \n";
            send(client, mes.c_str(), strlen(mes.c_str()), 0);
            cout << client << " ket noi!\n";
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
                    buf[ret] = '\0';
                    if (strcmp(buf, "exit\n") != 0)
                    {
                        char *des;
                        des = formatText(buf, des);
                        send(clients[i], des, strlen(des), 0);
                        delete[] des;
                    }
                    else
                    {
                        cout << clients[i] << " ngat ket noi!\n";
                        clients.erase(clients.begin() + i);
                        break;
                    }
                }
            }
        }
    }

    close(listener);
}