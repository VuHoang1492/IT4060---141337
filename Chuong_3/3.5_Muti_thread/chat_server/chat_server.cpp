#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

void *thread_proc(void *listener);
string formatDateMes();
bool checkLogin(int client);

map<int, string> clients; // các client trong room
pthread_mutex_t client_mutex;

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

    const char *nameReq = "You need a name to join chat box: (format : client_id: name -- spaces are not included) \n";
    pthread_mutex_init(&client_mutex, NULL);

    while (true)
    {
        cout << "Wait client connect...\n";
        int client = accept(listener, NULL, NULL);
        if (client == -1)
            continue;
        cout << "New connecting: " << client << endl;

        send(client, nameReq, strlen(nameReq), 0);

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, &thread_proc, (void *)&client);
        pthread_detach(thread_id);
    }
    pthread_mutex_destroy(&client_mutex);
    close(listener);
    return 0;
}

void *thread_proc(void *param)
{
    int client = *(int *)param;
    const char *formatErr = "Error: format : client_id: name -- spaces are not included\n";
    char buf[1024];
    while (true)
    {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            if (checkLogin(client))
            {
                pthread_mutex_lock(&client_mutex);
                map<int, string>::iterator out_client;

                cout << clients[client] << " disconnected!\n";

                string noti = clients[client] + " left this chat!!\n";

                for (map<int, string>::iterator recvClient = clients.begin(); recvClient != clients.end(); recvClient++)
                {
                    if (recvClient->first != client)
                    {
                        send(recvClient->first, noti.c_str(), strlen(noti.c_str()), 0);
                    }
                    else
                    {
                        out_client = recvClient;
                    }
                }
                clients.erase(out_client);
                pthread_mutex_unlock(&client_mutex);

                close(client);
                pthread_exit(NULL);
            }
            else
            {
                close(client);
                pthread_exit(NULL);
            }
        }
        buf[ret] = '\0';
        if (checkLogin(client))
        {
            string s = formatDateMes() + " " + clients[client] + ": " + buf;
            pthread_mutex_lock(&client_mutex);
            for (map<int, string>::iterator recvClient = clients.begin(); recvClient != clients.end(); recvClient++)
            {
                if (recvClient->first != client)
                    send(recvClient->first, s.c_str(), strlen(s.c_str()), 0);
            }
            pthread_mutex_unlock(&client_mutex);
        }
        else
        {
            char name[64];
            if (strncmp(buf, "client_id: ", 11) != 0)
            {
                send(client, formatErr, strlen(formatErr), 0);
                continue;
            }

            strncpy(name, buf + 11, strlen(buf) - 12);
            name[strlen(buf) - 12] = '\0';

            char *ptr = strchr(name, ' ');
            if (ptr != NULL)
            {
                send(client, formatErr, strlen(formatErr), 0);
                continue;
            }
            string s = name;
            clients.insert(make_pair(client, s));
            cout << clients[client] << " connected!!" << endl;

            pthread_mutex_lock(&client_mutex);
            string noti = clients[client] + " joined this chat!!\n";
            for (map<int, string>::iterator recvClient = clients.begin(); recvClient != clients.end(); recvClient++)
            {
                if (recvClient->first != client)
                    send(recvClient->first, noti.c_str(), strlen(noti.c_str()), 0);
            }
            pthread_mutex_unlock(&client_mutex);

            noti = "You were added!!\n";
            send(client, noti.c_str(), strlen(noti.c_str()), 0);
        }
    }

    return NULL;
}

string formatDateMes()
{
    time_t curr_time;
    curr_time = time(NULL);
    tm *tm_local = localtime(&curr_time);
    string s;
    int year = tm_local->tm_year + 1900;
    int month = tm_local->tm_mon + 1;
    int day = tm_local->tm_mday;
    int hour = tm_local->tm_hour;
    int min = tm_local->tm_min;
    int sec = tm_local->tm_sec;

    string hourType;

    if (hour <= 12)
    {
        hourType = "AM";
    }
    else
    {
        hourType = "PM";
        hour -= 12;
    }

    string yearStr = to_string(year);
    string monthStr = to_string(month);
    if (monthStr.length() == 1)
    {
        monthStr = "0" + monthStr;
    }
    string dayStr = to_string(day);
    if (dayStr.length() == 1)
    {
        dayStr = "0" + dayStr;
    }
    string hourStr = to_string(hour);
    if (hourStr.length() == 1)
    {
        hourStr = "0" + hourStr;
    }
    string minStr = to_string(min);
    if (minStr.length() == 1)
    {
        minStr = "0" + minStr;
    }
    string secStr = to_string(sec);
    if (secStr.length() == 1)
    {
        secStr = "0" + secStr;
    }

    s = yearStr + "/" + monthStr + "/" + dayStr + " " + hourStr + ":" + minStr + ":" + secStr + hourType;
    return s;
}

bool checkLogin(int cli)
{
    pthread_mutex_lock(&client_mutex);
    for (map<int, string>::iterator client = clients.begin(); client != clients.end(); client++)
    {
        if (client->first == cli)
        {
            pthread_mutex_unlock(&client_mutex);
            return true;
        }
    }
    pthread_mutex_unlock(&client_mutex);
    return false;
}