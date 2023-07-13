#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

void *thread_proc(void *listener);

map<int, string> clients; // các client trong room
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
int boss = -1; // chủ phòng
string topic;  // chủ đề room chat

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

    pthread_mutex_init(&client_mutex, NULL);

    while (1)
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

    pthread_mutex_destroy(&client_mutex);
    close(listener);
    return 0;
}

void *thread_proc(void *param)
{
    int client = *(int *)param;

    char buf[1024];
    while (1)
    {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            if (clients[client].length() == 0)
            {
            }
            else
            {
                if (boss != client)
                {
                    map<int, string>::iterator it, left_client;

                    string msg = "QUIT " + clients[client];
                    pthread_mutex_lock(&client_mutex);
                    for (it = clients.begin(); it != clients.end(); it++)
                    {
                        if (it->first == client)
                        {
                            left_client = it;
                        }
                        else
                        {
                            send(it->first, msg.c_str(), msg.length(), 0);
                        }
                    }
                    clients.erase(left_client);
                    pthread_mutex_unlock(&client_mutex);
                }
                else
                {
                    map<int, string>::iterator it, left_client;

                    string msg = "QUIT " + clients[client];
                    pthread_mutex_lock(&client_mutex);

                    for (it = clients.begin(); it != clients.end(); it++)
                    {
                        if (it->first == client)
                        {
                            left_client = it;
                        }
                        else
                        {
                            send(it->first, msg.c_str(), msg.length(), 0);
                        }
                    }
                    clients.erase(left_client);

                    if (!clients.empty())
                    {
                        boss = clients.begin()->first;
                    }
                    msg = "OP " + clients.begin()->second;
                    for (it = clients.begin(); it != clients.end(); it++)
                    {
                        send(it->first, msg.c_str(), msg.length(), 0);
                    }

                    pthread_mutex_unlock(&client_mutex);
                }
            }
            close(client);
            pthread_exit(NULL);
        }
        if (ret < sizeof(buf))
            buf[ret] = 0;

        char *token;
        token = strtok(buf, " ");

        if (strcmp(token, "JOIN") == 0)
        {
            // xử lý JOIN
            cout << "A person join \n";

            token = strtok(NULL, " ");
            char c_name[64];
            strcpy(c_name, token);
            token = strtok(NULL, " ");

            if (token != NULL)
            {
                send(client, "201 INVALID NICK NAME\n", 22, 0);
                continue;
            }

            bool used = false;
            pthread_mutex_lock(&client_mutex);

            map<int, string>::iterator it;
            for (it = clients.begin(); it != clients.end(); it++)
            {

                if (it->second == c_name)
                {
                    used = true;
                    break;
                }
            }

            if (used)
            {
                send(client, "200 NICKNAME IN USE\n", 20, 0);
            }
            else
            {
                clients[client] = c_name;
                send(client, "200 OK\n", 7, 0);
            }
            if (boss == -1 || clients.empty())
            {
                boss = client;
            }

            string msg = "JOIN " + clients[client];
            for (it = clients.begin(); it != clients.end(); it++)
            {
                if (it->first != client)
                    send(it->first, msg.c_str(), msg.length(), 0);
            }
            pthread_mutex_unlock(&client_mutex);
        }
        else if (strcmp(token, "MSG") == 0)
        {
            if (clients[client].length() == 0)
            {
                send(client, "999 UNKNOWN ERROR\n", 18, 0);
                continue;
            }
            token = strtok(NULL, "\n");

            map<int, string>::iterator it;

            string name = clients[client];
            if (!name.empty())
            {
                name.pop_back();
            }

            string msg = "MSG " + name + " " + token + "\n";

            pthread_mutex_lock(&client_mutex);
            for (it = clients.begin(); it != clients.end(); it++)
            {
                if (it->first != client)
                {
                    send(it->first, msg.c_str(), msg.length(), 0);
                }
            }
            pthread_mutex_unlock(&client_mutex);
            send(client, "100 OK\n", 7, 0);
        }
        else if (strcmp(token, "PMSG") == 0)
        {

            if (clients[client].length() == 0)
            {
                send(client, "999 UNKNOWN ERROR\n", 18, 0);
                continue;
            }
            token = strtok(NULL, " ");

            string recv_name = token;

            string send_name = clients[client];
            if (!send_name.empty())
            {
                send_name.pop_back();
            }

            map<int, string>::iterator it, recv_client;
            bool recv_true = false;
            pthread_mutex_lock(&client_mutex);
            for (it = clients.begin(); it != clients.end(); it++)
            {
                string name = it->second;
                name.pop_back();
                if (name == recv_name)
                {
                    recv_client = it;
                    recv_true = true;
                    break;
                }
            }
            if (recv_true)
            {
                token = strtok(NULL, "\n");
                string msg = "PMSG " + send_name + " " + token + "\n";
                send(recv_client->first, msg.c_str(), msg.length(), 0);
                send(client, "100 OK\n", 7, 0);
            }
            else
            {
                send(client, "202 UNKNOWN NICKNAME\n", 22, 0);
            }
            pthread_mutex_unlock(&client_mutex);
        }
        else if (strcmp(token, "OP") == 0)
        {
            if (clients[client].length() == 0)
            {
                send(client, "999 UNKNOWN ERROR\n", 18, 0);
                continue;
            }

            if (boss != client)
            {
                send(client, "203 DENIED\n", 12, 0);
                continue;
            }
            token = strtok(NULL, "\n");

            string new_op_name = token;

            map<int, string>::iterator it, new_op;

            bool op_true = false;
            pthread_mutex_lock(&client_mutex);
            for (it = clients.begin(); it != clients.end(); it++)
            {
                string name = it->second;
                name.pop_back();
                if (name == new_op_name)
                {
                    new_op = it;
                    op_true = true;
                    break;
                }
            }
            if (!op_true)
            {
                send(client, "202 UNKNOWN NICKNAME\n", 22, 0);
            }
            else
            {
                //  cout << "before boss: " << clients[boss] << endl;
                boss = new_op->first;
                //   cout << "after boss: " << clients[boss] << endl;
                send(client, "100 OK\n", 8, 0);
                string msg = "OP " + clients[boss];
                for (it = clients.begin(); it != clients.end(); it++)
                {
                    if (it->first != client)
                    {
                        send(it->first, msg.c_str(), msg.length(), 0);
                    }
                }
            }
            pthread_mutex_unlock(&client_mutex);
        }
        else if (strcmp(token, "KICK") == 0)
        {
            if (clients[client].length() == 0)
            {
                send(client, "999 UNKNOWN ERROR\n", 18, 0);
                continue;
            }

            if (boss != client)
            {
                send(client, "203 DENIED\n", 12, 0);
                continue;
            }
            token = strtok(NULL, "\n");

            string kicked_name = token;
            map<int, string>::iterator it, kicked_user;

            bool kick_name_true = false;
            pthread_mutex_lock(&client_mutex);
            for (it = clients.begin(); it != clients.end(); it++)
            {
                string name = it->second;
                name.pop_back();
                if (name == kicked_name)
                {
                    kicked_user = it;
                    kick_name_true = true;
                    break;
                }
            }
            if (!kick_name_true)
            {
                send(client, "202 UNKNOWN NICKNAME\n", 23, 0);
            }
            else
            {
                send(client, "100 OK\n", 8, 0);
                string msg = "KICK " + kicked_name + " " + clients[client];
                for (it = clients.begin(); it != clients.end(); it++)
                {

                    send(it->first, msg.c_str(), msg.length(), 0);
                }
                clients.erase(kicked_user);
            }
            pthread_mutex_unlock(&client_mutex);
        }
        else if (strcmp(token, "TOPIC") == 0)
        {
            if (clients[client].length() == 0)
            {
                send(client, "999 UNKNOWN ERROR\n", 18, 0);
                continue;
            }

            if (boss != client)
            {
                send(client, "203 DENIED\n", 12, 0);
                continue;
            }
            token = strtok(NULL, "\n");
            pthread_mutex_lock(&client_mutex);
            topic = token;
            string name = clients[client];
            name.pop_back();
            map<int, string>::iterator it;
            string msg = "TOPIC " + name + " " + topic;
            for (it = clients.begin(); it != clients.end(); it++)
            {

                send(it->first, msg.c_str(), msg.length(), 0);
            }
            pthread_mutex_unlock(&client_mutex);
        }
        else if (strcmp(buf, "QUIT\n") == 0)
        {
            if (clients[client].length() == 0)
            {
                send(client, "999 UNKNOWN ERROR\n", 18, 0);
                continue;
            }
            if (boss != client)
            {
                map<int, string>::iterator it, left_client;

                string msg = "QUIT " + clients[client];
                pthread_mutex_lock(&client_mutex);
                for (it = clients.begin(); it != clients.end(); it++)
                {
                    if (it->first == client)
                    {
                        left_client = it;
                    }
                    else
                    {
                        send(it->first, msg.c_str(), msg.length(), 0);
                    }
                }
                clients.erase(left_client);
                pthread_mutex_unlock(&client_mutex);
            }
            else
            {
                map<int, string>::iterator it, left_client;

                string msg = "QUIT " + clients[client];
                pthread_mutex_lock(&client_mutex);

                for (it = clients.begin(); it != clients.end(); it++)
                {
                    if (it->first == client)
                    {
                        left_client = it;
                    }
                    else
                    {
                        send(it->first, msg.c_str(), msg.length(), 0);
                    }
                }
                clients.erase(left_client);

                if (!clients.empty())
                {
                    boss = clients.begin()->first;
                }
                msg = "OP " + clients.begin()->second;
                for (it = clients.begin(); it != clients.end(); it++)
                {
                    send(it->first, msg.c_str(), msg.length(), 0);
                }

                pthread_mutex_unlock(&client_mutex);
            }
        }
    }

    return NULL;
}