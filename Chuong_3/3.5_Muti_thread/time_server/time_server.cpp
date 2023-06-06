#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

void *thread_proc(void *listener);
bool checkCommand(char *buf);
void process(int client, char *buf);

int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed\n");
    }
    if (listen(listener, 5))
    {
        perror("listen() failed\n");
    }

    while (true)
    {
        cout << "Wait client connect...\n";
        int client = accept(listener, NULL, NULL);
        if (client == -1)
            continue;
        cout << "New connecting: " << client << endl;
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, &thread_proc, (void *)&client);
        pthread_detach(thread_id);
    }
    close(listener);
    return 0;
}

void *thread_proc(void *param)
{
    int client = *(int *)param;

    const char *mes = "To get time, send a command with for math: GET_TIME [format].\nTo get supported format send: --help.\n";
    const char *spFormat = "Supported format:\n -dd/mm/yyyy\n -dd/mm/yy\n -mm/dd/yyyy\n -mm/dd/yy\n";
    const char *formatErr = "Format error. Check your command (include space)\n";
    char buf[256];
    send(client, mes, strlen(mes), 0);

    while (true)
    {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            close(client);
            pthread_exit(NULL);
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
    return NULL;
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
    int type = 0;
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