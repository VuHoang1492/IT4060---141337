#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

void *thread_proc_1(void *param);
void *thread_proc_2(void *param);

int user[10] = {-1};

struct Couple
{
    int user_1;
    int user_2;
    Couple(int a, int b)
    {
        user_1 = a;
        user_2 = b;
    }
};

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
    int number_user = 0;
    const char *waitUser = "Wait another user connect...\n";
    const char *beginChat = "Your chat room is ready!!\n";
    while (true)
    {
        cout << "Wait client connect...\n";
        int client = accept(listener, NULL, NULL);
        if (client == -1)
            continue;
        cout << "New connecting: " << client << endl;
        user[number_user] = client;
        number_user++;
        if (number_user == 2)
        {
            send(user[0], beginChat, strlen(beginChat), 0);
            send(user[1], beginChat, strlen(beginChat), 0);
            struct Couple couple = Couple(user[0], user[1]);
            user[0] = -1;
            user[1] = -1;
            number_user = 0;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, &thread_proc_1, (void *)&couple);
            pthread_detach(thread_id);
            pthread_create(&thread_id, NULL, &thread_proc_2, (void *)&couple);
            pthread_detach(thread_id);
        }
        else
        {
            send(client, waitUser, strlen(waitUser), 0);
        }
    }
    close(listener);
    return 0;
}

void *thread_proc_1(void *param)
{
    struct Couple couple = *(Couple *)param;
    cout << "Thread_1: " << couple.user_1 << " " << couple.user_2 << endl;
    char buf[256];
    const char *disconnect = "Your friend disconnect.\n";
    while (true)
    {
        int ret = recv(couple.user_1, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            send(couple.user_2, disconnect, strlen(disconnect), 0);
            break;
        }
        buf[ret] = '\0';
        string s = "Your friend: ";
        s += buf;
        send(couple.user_2, s.c_str(), strlen(s.c_str()), 0);
    }
    close(couple.user_1);
    close(couple.user_2);
    return NULL;
}

void *thread_proc_2(void *param)
{
    struct Couple couple = *(Couple *)param;
    cout << "Thread_2: " << couple.user_1 << " " << couple.user_2 << endl;

    char buf[256];
    const char *disconnect = "Your friend disconnect.\n";
    while (true)
    {
        int ret = recv(couple.user_2, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            send(couple.user_1, disconnect, strlen(disconnect), 0);
            break;
        }
        buf[ret] = '\0';
        string s = "Your friend: ";
        s += buf;
        send(couple.user_1, s.c_str(), strlen(s.c_str()), 0);
    }
    close(couple.user_1);
    close(couple.user_2);
    return NULL;
}
