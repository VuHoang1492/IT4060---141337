#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctime>
#include <poll.h>

using namespace std;

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

bool isLogIn(pollfd client, vector<int> queueClients, map<int, string> Clients)
{
    for (int i = 0; i < queueClients.size(); i++)
    {
        if (queueClients[i] == client.fd)
            return false;
    }
    for (map<int, string>::iterator i = Clients.begin(); i != Clients.end(); i++)
    {
        if (i->first == client.fd)

            return true;
    }
    return false;
}

int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    cout << listener << endl;
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

    vector<int> queueClients; // các client chờ đặt tên
    map<int, string> Clients; // các client trong room

    char buf[2048];

    const char *nameReq = "You need a name to join chat box: (format : client_id: name -- spaces are not included) \n";
    const char *formatErr = "Error: format : client_id: name -- spaces are not included\n";
    const char *nameUsedErr = "This name was used by another user. Choose a different name!!\n";
    const char *showListUser = "--lu\n";
    const char *hr = "_____________________\n";
    const char *sendPriMes = "To send a private message, this is syntax: --pri nameReceiver message \n";
    const char *userExistErr = "This user is not exist!!\n";

    while (true)
    {
        vector<pollfd> pollvec;
        struct pollfd server;
        server.fd = listener;
        server.events = POLLIN;

        pollvec.push_back(server);

        for (map<int, string>::iterator i = Clients.begin(); i != Clients.end(); i++)
        {
            struct pollfd Client;
            Client.fd = i->first;
            Client.events = POLLIN;

            pollvec.push_back(Client);
        }

        for (int i = 0; i < queueClients.size(); i++)
        {
            struct pollfd queueclient;
            queueclient.fd = queueClients[i];
            queueclient.events = POLLIN;
            pollvec.push_back(queueclient);
        }

        struct pollfd *pollArr = pollvec.data();

        nfds_t nfds = pollvec.size();

        int timeout = 5000;

        int pol = poll(pollArr, nfds, timeout);
        if (pol < 0)
        {
            perror("poll() failed.\n");
            return 1;
        }
        if (pol == 0)
        {
            cout << "Time out \n";
            continue;
        }
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);

        for (int i = 0; i < nfds; i++)
        {
            if (pollArr[i].fd == listener && pollArr[i].revents & POLLIN)
            {

                int client = accept(listener, (struct sockaddr *)&clientAddr, (unsigned int *)&clientAddrLen);
                cout << inet_ntoa(clientAddr.sin_addr) << endl;
                send(client, nameReq, strlen(nameReq), 0);
                queueClients.push_back(client);
            }
            else if (pollArr[i].revents & POLLIN)
            {
                int ret = recv(pollArr[i].fd, buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    if (!isLogIn(pollArr[i], queueClients, Clients))
                    {
                        close(pollArr[i].fd);
                        int index = 0;
                        for (int p : queueClients)
                        {
                            if (p != pollArr[i].fd)
                                ++index;
                            else
                                break;
                        }

                        queueClients.erase(queueClients.begin() + index);
                    }
                    else
                    {
                        cout << "Client " << Clients[pollArr[i].fd] << " disconnected!\n";

                        string noti = Clients[pollArr[i].fd] + " left this chat!!\n";
                        for (map<int, string>::iterator recvClient = Clients.begin(); recvClient != Clients.end(); recvClient++)
                        {
                            if (recvClient->first != pollArr[i].fd)
                                send(recvClient->first, noti.c_str(), strlen(noti.c_str()), 0);
                        }

                        close(pollArr[i].fd);

                        for (map<int, string>::iterator cli = Clients.begin(); cli != Clients.end(); cli++)
                        {
                            if (cli->first == pollArr[i].fd)
                            {
                                Clients.erase(cli);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    buf[ret] = '\0';
                    if (!isLogIn(pollArr[i], queueClients, Clients))
                    {
                        char name[64];
                        if (strncmp(buf, "client_id: ", 11) != 0)
                        {
                            send(pollArr[i].fd, formatErr, strlen(formatErr), 0);
                            continue;
                        }

                        strncpy(name, buf + 11, strlen(buf) - 12);
                        name[strlen(buf) - 12] = '\0';

                        char *ptr = strchr(name, ' ');
                        if (ptr != NULL)
                        {
                            send(pollArr[i].fd, formatErr, strlen(formatErr), 0);
                            continue;
                        }

                        bool checkName = false;
                        for (map<int, string>::iterator cli = Clients.begin(); cli != Clients.end(); cli++)
                        {
                            if (strcmp(name, (cli->second).c_str()) == 0)
                            {
                                checkName = true;
                                break;
                            }
                        }

                        if (checkName)
                        {
                            send(pollArr[i].fd, nameUsedErr, strlen(nameUsedErr), 0);
                            continue;
                        }

                        // chuyển client từ hàng đợi vào room
                        string s = name;
                        Clients.insert(make_pair(pollArr[i].fd, s));
                        cout << Clients[pollArr[i].fd] << " connected!!" << endl;

                        string noti = Clients[pollArr[i].fd] + " joined this chat!!\n";
                        for (map<int, string>::iterator recvClient = Clients.begin(); recvClient != Clients.end(); recvClient++)
                        {
                            if (recvClient->first != pollArr[i].fd)
                                send(recvClient->first, noti.c_str(), strlen(noti.c_str()), 0);
                        }

                        noti = "You were added!!\n";
                        send(pollArr[i].fd, noti.c_str(), strlen(noti.c_str()), 0);

                        int index = 0;
                        for (int p : queueClients)
                        {
                            if (p != pollArr[i].fd)
                                ++index;
                            else
                                break;
                        }

                        queueClients.erase(queueClients.begin() + index);
                    }
                    else
                    {
                        if (!strcmp(buf, showListUser))
                        {
                            send(pollArr[i].fd, hr, strlen(hr), 0);
                            for (map<int, string>::iterator cli = Clients.begin(); cli != Clients.end(); cli++)
                            {
                                if (cli->first == pollArr[i].fd)
                                {
                                    cout << "This is user\n";
                                    string s = cli->second + "(You)\n";
                                    send(pollArr[i].fd, s.c_str(), strlen(s.c_str()), 0);
                                }
                                else
                                {
                                    string s = cli->second + "\n";
                                    send(pollArr[i].fd, s.c_str(), strlen(s.c_str()), 0);
                                }
                            }
                            send(pollArr[i].fd, hr, strlen(hr), 0);
                            send(pollArr[i].fd, sendPriMes, strlen(sendPriMes), 0);
                            send(pollArr[i].fd, hr, strlen(hr), 0);
                            continue;
                        }

                        if (!strncmp(buf, "--pri", 5))
                        {
                            char name[64];
                            char priMes[2048];

                            int index = 6;
                            while (buf[index] != ' ')
                            {
                                index++;
                            }

                            // index là vị trí của ' ' thứ 2

                            strncpy(name, buf + 6, index - 6);
                            name[index - 6] = '\0';

                            strcpy(priMes, buf + index + 1);
                            priMes[strlen(buf) - index - 1] = '\0';

                            bool checkMes = false;

                            if (strlen(priMes))
                            {
                                int k = 0;
                                while (k < strlen(priMes))
                                {
                                    if (priMes[k] != ' ')
                                    {
                                        checkMes = true;

                                        break;
                                    }
                                    k++;
                                }
                            }

                            if (checkMes)
                            {
                                bool checkName = false;
                                map<int, string>::iterator recvClient;

                                for (recvClient = Clients.begin(); recvClient != Clients.end(); recvClient++)
                                {
                                    if (!strcmp(name, (recvClient->second).c_str()))
                                    {
                                        checkName = true;
                                        break;
                                    }
                                }
                                if (checkName)
                                {
                                    string s = formatDateMes() + " " + "Private message from " + Clients[pollArr[i].fd] + ": " + priMes;
                                    send(recvClient->first, s.c_str(), strlen(s.c_str()), 0);
                                }
                                else
                                {
                                    send(pollArr[i].fd, userExistErr, strlen(userExistErr), 0);
                                }
                            }
                            continue;
                        }

                        string s = formatDateMes() + " " + Clients[pollArr[i].fd] + ": " + buf;
                        for (map<int, string>::iterator recvClient = Clients.begin(); recvClient != Clients.end(); recvClient++)
                        {
                            if (recvClient->first != pollArr[i].fd)
                                send(recvClient->first, s.c_str(), strlen(s.c_str()), 0);
                        }
                    }
                }
            }
        }
    }

    close(listener);
}