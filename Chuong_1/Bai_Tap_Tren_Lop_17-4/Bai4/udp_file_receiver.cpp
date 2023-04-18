#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char *argv[])
{
    int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    int b = bind(receiver, (struct sockaddr *)&addr, sizeof(addr));
    if (b == -1)
    {
        cout << "bind failed\n";
        return 0;
    }

    char buf[1024];
    fstream file;
    while (true)
    {

        char id[20];
        int id_len = recvfrom(receiver, id, sizeof(id), 0, NULL, NULL);
        id[id_len] = '\0';

        int ret = recvfrom(receiver, buf, sizeof(buf), 0, NULL, NULL);
        if (ret <= 0)
        {
            break;
        }
        buf[ret] = '\0';
        string s_id = id;
        string s = s_id + "-receive.txt";

        file.open(s, ios::app);
        if (!file.is_open())
        {
            cout << "open file failed!\n";
            return 0;
        }
        file << buf << endl;
        file.close();
    }
}
