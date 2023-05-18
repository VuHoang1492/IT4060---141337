#include <bits/stdc++.h>
#include <stdlib.h>
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

    vector<int> queueClients;
    vector<int> loggedClients;
    fd_set fdread;

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    char buf[2048];

    const char *logInReq = "Please Log In, format: username password\n";
    const char *formatErr = "Error: format : username password\n";
    const char *usedNotExistsErr = "This user is not exist. Check your username or password!!\n";
    const char *loggedInSuccess = "You was logged in! Enter your command (format: command > filename):\n";
    const char *formatCommandErr = "Format command error (format: command > filename):\n";
    const char *fileNameErr = "FileName error:\n";
    const char *successed = "Succesful!! This is your result:";
    const char *smtErr = "Have somethings was wrong. Try again!!\n";
    const char *hr = "\n--------------------------------------------------\n";

    while (true)
    {
        FD_ZERO(&fdread);
        FD_SET(listener, &fdread);
        int maxdp = listener + 1;

        for (int i = 0; i < queueClients.size(); i++)
        {
            FD_SET(queueClients[i], &fdread);
            if (maxdp < queueClients[i] + 1)
                maxdp = queueClients[i] + 1;
        }

        for (int i = 0; i < loggedClients.size(); i++)
        {
            FD_SET(loggedClients[i], &fdread);
            if (maxdp < loggedClients[i] + 1)
                maxdp = loggedClients[i] + 1;
        }

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int sel = select(maxdp, &fdread, NULL, NULL, &tv);
        if (sel < 0)
        {
            perror("select() failed.\n");
            return 1;
        }
        if (sel == 0)
        {
            cout << "Time out \n";
            continue;
        }
        if (FD_ISSET(listener, &fdread))
        {
            int client = accept(listener, NULL, NULL);
            send(client, logInReq, strlen(logInReq), 0);
            queueClients.push_back(client);
            continue;
        }
        for (int i = 0; i < loggedClients.size(); i++)
        {
            if (FD_ISSET(loggedClients[i], &fdread))
            {
                int ret = recv(loggedClients[i], buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    close(loggedClients[i]);
                    loggedClients.erase(loggedClients.begin() + i);
                    continue;
                }
                buf[ret - 1] = '\0';
                char *pch = strchr(buf, '>');

                if (pch == NULL)
                {
                    send(loggedClients[i], formatCommandErr, strlen(formatCommandErr), 0);
                    continue;
                }

                char fileName[64];

                pch++;
                while (true)
                {
                    if (pch[0] == ' ')
                        pch++;
                    else
                        break;
                }

                strcpy(fileName, pch);

                pch = strchr(fileName, ' ');

                if (pch != NULL)
                {
                    send(loggedClients[i], fileNameErr, strlen(fileNameErr), 0);
                    continue;
                }

                int res = system(buf);

                if (res == 0)
                {
                    send(loggedClients[i], successed, strlen(successed), 0);
                    send(loggedClients[i], hr, strlen(hr), 0);

                    fstream file;
                    char line[2048];
                    file.open(fileName, ios::in);
                    if (!file.is_open())
                    {
                        cout << "Open file failed!\n";
                        send(loggedClients[i], smtErr, strlen(smtErr), 0);
                        continue;
                    }
                    while (file.getline(line, 2048))
                    {
                        send(loggedClients[i], line, strlen(line), 0);
                    }
                    file.close();
                    send(loggedClients[i], hr, strlen(hr), 0);
                }
                else
                {
                    send(loggedClients[i], formatCommandErr, strlen(formatCommandErr), 0);
                }
            }
        }

        for (int i = 0; i < queueClients.size(); i++)
        {
            if (FD_ISSET(queueClients[i], &fdread))
            {
                int ret = recv(queueClients[i], buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    close(queueClients[i]);
                    queueClients.erase(queueClients.begin() + i);
                    continue;
                }
                buf[ret] = '\0';

                // Kiểm tra định dạng input
                char *pch;
                int count = 0;
                pch = strchr(buf, ' ');
                while (pch != NULL)
                {
                    count++;
                    pch = strchr(pch + 1, ' ');
                }

                if (count != 1)
                {
                    send(queueClients[i], formatErr, strlen(formatErr), 0);
                    continue;
                }

                char *userName = strtok(buf, " ");
                char *password = strtok(NULL, "\n");

                bool checkLogIn = false;

                fstream file;
                char line[64];
                file.open("user.txt", ios::in);
                if (!file.is_open())
                {
                    cout << "Open file failed!\n";
                    send(loggedClients[i], smtErr, strlen(smtErr), 0);
                    continue;
                }
                while (file.getline(line, 64))
                {
                    char *userNameCheck = strtok(line, " ");
                    char *passwordCheck = strtok(NULL, "\n");

                    if (!strcmp(userName, userNameCheck) && !strcmp(password, passwordCheck))
                    {
                        checkLogIn = true;
                        break;
                    }
                }
                file.close();

                if (!checkLogIn)
                {
                    send(queueClients[i], usedNotExistsErr, strlen(usedNotExistsErr), 0);
                    continue;
                }
                send(queueClients[i], loggedInSuccess, strlen(loggedInSuccess), 0);
                loggedClients.push_back(queueClients[i]);
                cout << queueClients[i] << " connected!!\n";
                queueClients.erase(queueClients.begin() + i);
            }
        }
    }

    close(listener);
}