#include <bits/stdc++.h>
#include <sys/socket.h> // cấu trúc socket
#include <string.h>
#include <arpa/inet.h> // CHuyển đổi địa chỉ
#include <netdb.h>     //Phân giải tên miền
using namespace std;

int main(int argc, char *argv[])
{

    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1)
    {
        cout << "create failed\n";
        return (1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(stoi(argv[1]));

    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        cout << "Bind failed!";
        return 0;
    };
    if (listen(server, 5) == -1)
    {
        cout << "listen failed!";
        return 0;
    };

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int client = accept(server, (struct sockaddr *)&clientAddr, (unsigned int *)&clientAddrLen);
    if (client == -1)
    {
        cout << "accept failed!";
        return 0;
    }
    // send hello client
    fstream file;
    file.open(argv[2], ios::in);
    if (!file.is_open())
    {
        cout << "Open hello file failed!\n";
        return 0;
    }
    string line;
    getline(file, line);
    line += '\n';
    send(client, line.c_str(), strlen(line.c_str()), 0);
    file.close();

    // get mes from client and write to file
    ofstream out;
    out.open(argv[3], ios::app);
    if (!out.is_open())
    {
        cout << "Open write file failed!\n";
        return 0;
    }

    while (1)
    {
        char mes[256];
        int ret = recv(client, mes, sizeof(mes), 0);
        if (ret <= 0)
            break;
        cout << "Received: " << ret << "bytes" << endl;
        mes[ret] = '\0';
        out << mes << endl;
    }
    out.close();

    close(client);
    close(server);
}