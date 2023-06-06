#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

void *thread_proc(void *listener);
bool checkLogin(char *buf);
bool processCommand(int client, char *buf);

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

    const char *logInReq = "Please Log In, format: username password\n";
    const char *logInErr = "Log in failed. Check your user name, password or format\n";
    const char *loggedInSuccess = "You was logged in! Enter your command (format: command > filename):\n";
    bool checkLogIn = false;
    while (true)
    {
        char buf[256];
        send(client, logInReq, strlen(logInReq), 0);

        // Xử lý đăng nhập
        while (!checkLogIn)
        {
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0)
            {
                close(client);
                pthread_exit(NULL);
            }
            buf[ret] = '\0';
            printf("Received: %s\n", buf);
            checkLogIn = checkLogin(buf);
            if (!checkLogIn)
            {
                send(client, logInErr, strlen(logInErr), 0);
            }
            else
            {
                send(client, loggedInSuccess, strlen(loggedInSuccess), 0);
            }
        }

        // Xử lý command
        while (true)
        {
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0)
            {
                close(client);
                pthread_exit(NULL);
            }
            buf[ret - 1] = '\0';
            printf("Received: %s\n", buf);
            processCommand(client, buf);
        }
    }
    return NULL;
}

bool checkLogin(char *buf)
{

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
        return false;
    }

    char *userName = strtok(buf, " ");
    char *password = strtok(NULL, "\n");

    fstream file;
    char line[64];
    file.open("user.txt", ios::in);
    if (!file.is_open())
    {
        cout << "Open file failed!\n";
        return false;
    }
    while (file.getline(line, 64))
    {
        char *userNameCheck = strtok(line, " ");
        char *passwordCheck = strtok(NULL, "\n");

        if (!strcmp(userName, userNameCheck) && !strcmp(password, passwordCheck))
        {
            return true;
        }
    }
    file.close();
    return false;
}

bool processCommand(int client, char *buf)
{
    const char *formatCommandErr = "Format command error (format: command > filename):\n";
    const char *fileNameErr = "FileName error:\n";
    const char *successed = "Succesful!! This is your result:";
    const char *smtErr = "Have somethings was wrong. Try again!!\n";
    const char *hr = "\n--------------------------------------------------\n";

    char *pch = strchr(buf, '>');

    if (pch == NULL)
    {
        send(client, formatCommandErr, strlen(formatCommandErr), 0);
        return false;
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
        send(client, fileNameErr, strlen(fileNameErr), 0);
        return false;
    }

    int res = system(buf);

    if (res == 0)
    {
        send(client, successed, strlen(successed), 0);
        send(client, hr, strlen(hr), 0);

        fstream file;
        char line[2048];
        file.open(fileName, ios::in);
        if (!file.is_open())
        {
            cout << "Open file failed!\n";
            send(client, smtErr, strlen(smtErr), 0);
            return false;
        }
        while (file.getline(line, 2048))
        {
            send(client, line, strlen(line), 0);
        }
        file.close();
        send(client, hr, strlen(hr), 0);
        return true;
    }
    else
    {
        send(client, smtErr, strlen(smtErr), 0);
        return false;
    }
}