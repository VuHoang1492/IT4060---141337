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

    char *name[64] = {"******"};
    bool leader[64] = {false};

    while (1)
    {
        int client = accept(listener, NULL, NULL);
        cout << client << " Connected\n"
             << endl;

        if (fork() == 0)
        {
            // Tien trinh con
            close(listener);

            // Xu ly ket noi tu client

            char buf[1024];
            while (1)
            {
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    printf("Ket noi bi dong.\n");
                    close(client);
                    exit(0);
                }
                if (ret < sizeof(buf))
                    buf[ret] = 0;

                char *token;
                token = strtok(buf, " ");

                if (strcmp(token, "JOIN") == 0)
                {
                    cout << "A person join \n";
                    cout << "token: " << token << endl;

                    token = strtok(NULL, " ");
                    char c_name[64];
                    strcpy(c_name, token);
                    token = strtok(NULL, " ");

                    if (token != NULL)
                    {
                        send(client, "201 INVALID NICK NAME", 22, 0);
                        continue;
                    }

                    bool used = false;
                    for (char *n : name)
                    {
                        cout << n << endl;
                        cout << c_name << endl;
                        if (n == c_name)
                        {
                            used = true;
                            break;
                        }
                    }

                    if (used)
                    {
                        send(client, "200 NICKNAME IN USE", 20, 0);
                    }
                    else
                    {
                        name[client] = c_name;
                        if (client == 4)
                            leader[client] = true;
                        send(client, "200 OK", 7, 0);
                    }
                }
            }
        }

        // close(client);
    }

    close(listener);
}