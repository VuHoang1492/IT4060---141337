#include <bits/stdc++.h>
#include <sys/socket.h> // cấu trúc socket
#include <string.h>
#include <arpa/inet.h> // CHuyển đổi địa chỉ
#include <netdb.h>     //Phân giải tên miền
using namespace std;

struct SSD
{
    char name;
    char DungLuong[10];
};

struct Computer
{
    char name[50];
    vector<SSD> ds;
};

int main(int argc, char *argv[])
{

    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1)
    {
        cout << "create socket failed\n";
        return (1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(stoi(argv[1]));
    bind(server, (struct sockaddr *)&addr, sizeof(addr));
    listen(server, 5);

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int client = accept(server, (struct sockaddr *)&clientAddr, (unsigned int *)&clientAddrLen);
    // get hello from server
    const char *mes = "Hello client!";
    int ret = send(client, mes, strlen(mes), 0);
    cout << ret << endl;

    // receive data
    Computer com;

    recv(client, &com, 80, 0);

    cout << com.name;

    close(client);
    close(server);
}
