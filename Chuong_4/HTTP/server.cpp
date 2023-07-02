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

#include <sys/stat.h>

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

bool isImageFile(const char *filename)
{
    const char *extension = strrchr(filename, '.');

    if (extension != NULL)
    {
        if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0 ||
            strcmp(extension, ".png") == 0 || strcmp(extension, ".gif") == 0)
        {
            return true;
        }
    }

    return false;
}

void *client_thread(void *param)
{
    int client = *(int *)param;
    char buf[2048];

    int len = recv(client, buf, sizeof(buf) - 1, 0);

    buf[len] = 0;
    char *method = strtok(buf, " ");
    char *path = strtok(NULL, " ");

    if (path == NULL)
        return NULL;

    printf("Method: %s\n", method);
    printf("Path: %s\n", path);
    char src[100] = "/mnt/e/workspaces/C_C++/C_Ubuntu/LTM/Chuong_4/HTTP";
    strcat(src, path);

    string data = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>";

    struct stat st;
    if (stat(src, &st) == 0)
    {
        if (S_ISREG(st.st_mode))
        {
            printf("%s is a regular file.\n", path);

            if (isImageFile(path))
            {
                data += "<img src=\"";
                data += path;

                data += "\" style=\"width: 500px; height: auto; border: 1px solid black;\"";
                data += "alt='";
                data += path;
                data += "'";
                data += ">";
            }
            else
            {
                data += "<p>";
                FILE *fp = fopen(src, "rb");

                while (true)
                {
                    int ret = fread(buf, 1, sizeof(buf), fp);
                    if (ret <= 0)
                        break;
                    data += buf;
                }
                fclose(fp);
                data += "</p>";
            }
        }
        else if (S_ISDIR(st.st_mode))
        {
            printf("%s is a directory.\n", path);
            DIR *dir = opendir(src);
            struct dirent *entry;

            if (dir == NULL)
            {
                perror("opendir");
                close(client);
                exit(0);
            }

            while ((entry = readdir(dir)) != NULL)
            {

                if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                {
                    continue;
                }
                else
                {
                    data += "<a href='";
                    if (strcmp(path, "/"))
                    {
                        data += path;
                        data += "/";
                    }

                    data += entry->d_name;
                    data += "'>";
                    data += entry->d_name;
                    data += "</a><br/>";
                }
            }
            closedir(dir);
        }
        else
        {
            printf("%s is neither a regular file nor a directory.\n", path);
        }
    }
    else
    {
        perror("stat");
        return NULL;
    }

    data += "</body></html>";
    send(client, data.c_str(), data.size(), 0);

    close(client);

    return NULL;
}