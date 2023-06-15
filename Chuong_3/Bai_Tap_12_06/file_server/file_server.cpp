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
    const char *req = "\nInput file name:\n";
    const char *smtErr = "Cannot open file.\n";

    signal(SIGCHLD, signalHandler);
    while (true)
    {
        cout << "Waiting client connect....\n";
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
                close(client);
                exit(0);
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

            if (count == 0)
            {
                send(client, noFile, strlen(noFile), 0);
                close(client);
                exit(0);
            }
            else
            {
                string s = "OK " + to_string(count) + "\r\n";
                send(client, s.c_str(), strlen(s.c_str()), 0);
                string list_file;
                for (auto name : file_names)
                {
                    cout << name << endl;
                    list_file += name;
                    list_file += "\r\n";
                }
                list_file += "\r\n";
                send(client, list_file.c_str(), strlen(list_file.c_str()), 0);

                char buf[256];
                while (true)
                {
                    send(client, req, strlen(req), 0);
                    int ret = recv(client, buf, sizeof(buf), 0);

                    if (ret <= 0)
                    {
                        close(client);
                        exit(0);
                    }
                    buf[ret - 1] = '\0';
                    cout << buf << endl;

                    fstream file;
                    char line[2048];
                    file.open(buf, ios::in);
                    if (!file.is_open())
                    {
                        cout << "Open file failed!\n";
                        send(client, smtErr, strlen(smtErr), 0);
                        continue;
                    }
                    while (file.getline(line, 2048))
                    {
                        send(client, line, strlen(line), 0);
                    }
                    file.close();
                }
            }
            // close(client);
            // exit(0);
        }
        close(client);
    }

    close(listener);
}
