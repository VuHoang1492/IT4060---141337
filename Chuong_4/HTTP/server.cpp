#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <dirent.h>

using namespace std;

void *client_thread(void *);

void signal_handler(int signo)
{
    wait(NULL);
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

    signal(SIGPIPE, signal_handler);
    cout << "Waiting for client connect...\n";
    while (1)
    {
        int client = accept(listener, NULL, NULL);
        if (client == -1)
        {
            perror("accept() failed");
            continue;
            ;
        }
        printf("New client connected: %d\n", client);

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_thread, &client);
        pthread_detach(thread_id);
    }

    close(listener);

    return 0;
}

void *client_thread(void *param)
{
    int client = *(int *)param;

    DIR *dir;
    struct dirent *entry;
    dir = opendir(".");
    if (dir == NULL)
    {
        perror("opendir");
        close(client);
        return NULL;
    }
    string p = "<div>";
    while ((entry = readdir(dir)) != NULL)
    {
        p += "<a href ='http://localhost:9000/";
        p += entry->d_name;
        p += "'> <h4>";
        p += entry->d_name;
        p += "</h4></a>\n ";
    }
    closedir(dir);

    p += "</div>";

    string data = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>";
    data += p;
    data += "</ body></ html> ";
    send(client, data.c_str(), strlen(data.c_str()), 0);
    char buf[1024];
    while (true)
    {
        int n = recv(client, buf, sizeof(buf), 0);
        if (n == 0)
        {
            break;
        }
        cout << buf << endl;
    }
    return NULL;
}