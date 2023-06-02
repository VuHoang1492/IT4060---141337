#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

using namespace std;

void signalHandler(int signo)
{
    int pid = wait(NULL);
    printf("Child %d terminated\n", pid);
}

bool checkCommand(char *buf)
{
    int check = strncmp(buf, "GET_TIME ", 9);
    if (check != 0)
        return false;
    else
        return true;
}

/*
format type:
    1: dd/mm/yyyy
    2:dd/mm/yy
    3:mm/dd/yyyy
    4:mm/dd/yy
*/
void process(int client, char *buf)
{
    int type =0;
    if (!strcmp(buf, "dd/mm/yyyy\n"))
    {
        type = 1;
    }
    if (!strcmp(buf, "dd/mm/yy\n"))
    {
        type = 2;
    }
    if (!strcmp(buf, "mm/dd/yyyy\n"))
    {
        type = 3;
    }
    if (!strcmp(buf, "mm/dd/yy\n"))
    {
        type = 4;
    }

    // get time
    time_t rawtime;
    struct tm *timeLocal;
    time(&rawtime);
    timeLocal = localtime(&rawtime);

    const char *Err = "Invalid format.\nSend --help for support.\n";
    const char *hr = "\n-------------\n";

    char buffer[20];

    switch (type)
    {
    case 1:
        strftime(buffer, 20, "%d/%m/%Y", timeLocal);
        send(client, buffer, strlen(buffer), 0);
        send(client, hr, strlen(hr), 0);
        break;
    case 2:
        strftime(buffer, 20, "%d/%m/%y", timeLocal);
        send(client, buffer, strlen(buffer), 0);
        send(client, hr, strlen(hr), 0);
        break;

    case 3:
        strftime(buffer, 20, "%m/%d/%Y", timeLocal);
        send(client, buffer, strlen(buffer), 0);
        send(client, hr, strlen(hr), 0);
        break;

    case 4:
        strftime(buffer, 20, "%x", timeLocal);
        send(client, buffer, strlen(buffer), 0);
        send(client, hr, strlen(hr), 0);
        break;

    default:
        send(client, Err, strlen(Err), 0);
        send(client, hr, strlen(hr), 0);
        break;
    }
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

    const char *mes = "To get time, send a command with for math: GET_TIME [format].\nTo get supported format send: --help.\n";
    const char *spFormat = "Supported format:\n -dd/mm/yyyy\n -dd/mm/yy\n -mm/dd/yyyy\n -mm/dd/yy\n";
    const char *formatErr = "Format error. Check your command (include space)\n";

    signal(SIGCHLD, signalHandler);
    while (true)
    {
        cout << "Wait for client connect!!\n";
        int client = accept(listener, NULL, NULL);
        cout << "New client connected: " << client << endl;
        if (fork() == 0)
        {
            char buf[256];
            close(listener);
            send(client, mes, strlen(mes), 0);

            while (true)
            {
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    close(client);
                    exit(0);
                }
                buf[ret] = '\0';
                if (!strcmp(buf, "--help\n"))
                {
                    send(client, spFormat, strlen(spFormat), 0);
                    continue;
                }

                if (checkCommand(buf))
                {
                    char format[20];
                    strcpy(format, buf + 9);
                    process(client, format);
                }
                else
                {
                    send(client, formatErr, strlen(formatErr), 0);
                }
            }
        }

        close(client);
    }

    close(listener);
}