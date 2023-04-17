#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char *argv[])
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }

    char buf[256];
    ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0)
    {
        return 0;
    }
    cout << buf;

    fstream file;

    file.open("data.txt", ios::in);
    if (!file.is_open())
    {
        cout << "Failed to open file!\n";
        return 0;
    }
    string line;
    while (getline(file, line))
    {
        ret = send(client, line.c_str(), strlen(line.c_str()), 0);
        if (ret <= 0)
        {
            return 0;
        }
    }
    file.close();

    close(client);
}