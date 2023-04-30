#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctime>

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

    fd_set fdread;
    vector<int> queueClients; // các client chờ đặt tên
    map<int, string> Clients; // các client trong room

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    char buf[2048];

    const char *nameReq = "You need a name to join chat box: (format : client_id: name -- spaces are not included) \n";
    const char *formatErr = "Error: format : client_id: name -- spaces are not included\n";

    while (true)
    {
        FD_ZERO(&fdread);
        FD_SET(listener, &fdread);
        int maxdp = listener + 1;
        for (map<int, string>::iterator i = Clients.begin(); i != Clients.end(); i++)
        {
            FD_SET(i->first, &fdread);
            if (i->first + 1 > maxdp)
            {
                maxdp = i->first + 1;
            }
        }

        for (int i = 0; i < queueClients.size(); i++)
        {
            FD_SET(queueClients[i], &fdread);
            if (maxdp < queueClients[i] + 1)
                maxdp = queueClients[i] + 1;
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
            send(client, nameReq, strlen(nameReq), 0);
            queueClients.push_back(client);
            continue;
        }

        map<int, string>::iterator cli = Clients.begin();

        while (cli != Clients.end())
        {
            if (FD_ISSET(cli->first, &fdread))
            {
                int ret = recv(cli->first, buf, sizeof(buf), 0);

                buf[ret] = '\0';

                if (ret == 0)
                {
                    cout << "Client " << cli->first << " disconnected!\n";

                    string noti = cli->second + " left this chat!!\n";
                    for (map<int, string>::iterator recvClient = Clients.begin(); recvClient != Clients.end(); recvClient++)
                    {
                        if (recvClient->first != cli->first)
                            send(recvClient->first, noti.c_str(), strlen(noti.c_str()), 0);
                    }

                    close(cli->first);
                    Clients.erase(cli++);
                    continue;
                }

                string s = formatDateMes() + " " + Clients[cli->first] + ": " + buf;
                for (map<int, string>::iterator recvClient = Clients.begin(); recvClient != Clients.end(); recvClient++)
                {
                    if (recvClient->first != cli->first)
                        send(recvClient->first, s.c_str(), strlen(s.c_str()), 0);
                }
            }
            ++cli;
        }
        char name[64];
        for (int i = 0; i < queueClients.size(); i++)
        {
            if (FD_ISSET(queueClients[i], &fdread))
            {
                int ret = recv(queueClients[i], buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    queueClients.erase(queueClients.begin() + i);
                    continue;
                }
                buf[ret] = '\0';

                if (strncmp(buf, "client_id: ", 11) != 0)
                {
                    send(queueClients[i], formatErr, strlen(formatErr), 0);
                    continue;
                }

                strncpy(name, buf + 11, strlen(buf) - 12);
                name[strlen(buf) - 12] = '\0';

                char *ptr = strchr(name, ' ');
                if (ptr != NULL)
                {
                    send(queueClients[i], formatErr, strlen(formatErr), 0);
                    continue;
                }
                string s = name;
                Clients.insert(make_pair(queueClients[i], s));
                cout << Clients[queueClients[i]] << " connected!!" << endl;

                string noti = Clients[queueClients[i]] + " joined this chat!!\n";
                for (map<int, string>::iterator recvClient = Clients.begin(); recvClient != Clients.end(); recvClient++)
                {
                    if (recvClient->first != queueClients[i])
                        send(recvClient->first, noti.c_str(), strlen(noti.c_str()), 0);
                }

                noti = " You were added!!\n";
                send(queueClients[i], noti.c_str(), strlen(noti.c_str()), 0);

                queueClients.erase(queueClients.begin() + i);
            }
        }
    }

    close(listener);
}