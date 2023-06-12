#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <wait.h>
#include <dirent.h>

using namespace std;

void signalHandler(int signo)
{ // Xử lý sự kiện tiến trình con kết thúc
    int stat;
    printf("signo = %d\n", signo);
    int pid = wait(&stat);
    printf("child %d terminated.\n", pid);
    return;
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

    const char *noFile = "ERROR No files to download\r\n";

    signal(SIGCHLD, signalHandler);
    while (true)
    {
        cout << "Waiting clien connect....\n";
        int client = accept(listener, NULL, NULL);
        cout << "New connect: " << client << endl;
        if (fork() == 0)
        {
            close(listener);

            DIR *dir;
            struct dirent *entry;
            int count = 0; // số lượng file
            vector<char *> file_names;
            dir = opendir(".");
            if (dir == NULL)
            {
                perror("opendir");
                return 1;
            }

            while ((entry = readdir(dir)) != NULL)
            {
                if (entry->d_type == DT_REG)
                {
                    count++;
                    file_names.push_back(entry->d_name);
                }
            }
            closedir(dir);

            cout << count << endl;
            if (count == 0)
            {
                send(client, noFile, strlen(noFile), 0);
                close(client);
                exit(0);
            }
            else
            {
                string s = "OK " + to_string(count);
                for (char *fileName : file_names)
                {
                    s += "\r\n";
                    s += fileName;
                }
                s += "\r\n\r\n";
                cout << s << endl;
            }

            closedir(dir);
        }
        close(client);
    }

    close(listener);
}