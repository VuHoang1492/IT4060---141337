#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char *argv[])
{
    int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    char mes[1024];

    char *name = argv[3];

    const char *id = "Sender3";

    fstream file;
    file.open(argv[3], ios::in);
    if (!file.is_open())
    {
        cout << "Open file failed!\n";
        return 0;
    }
    cout << "Send data in file \n";
    while (file.getline(mes, 1024))
    {
      
        sendto(sender, id, strlen(id), 0, (struct sockaddr *)&addr, sizeof(addr));
        int ret = sendto(sender, mes, strlen(mes), 0, (struct sockaddr *)&addr, sizeof(addr));
    }
    file.close();
}