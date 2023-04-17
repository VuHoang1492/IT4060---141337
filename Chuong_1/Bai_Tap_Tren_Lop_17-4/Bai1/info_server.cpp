#include <bits/stdc++.h>
#include <sys/socket.h> // cấu trúc socket
#include <string.h>
#include <arpa/inet.h> // CHuyển đổi địa chỉ
#include <netdb.h>     //Phân giải tên miền
using namespace std;

#define BUFFER_SIZE 1024

struct SSD
{
    char name;
    int size;
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
    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        cout << "bind failed!\n";
        return 0;
    };
    if (listen(server, 5) == -1)
    {
        cout << "listen failed!\n";
        return 0;
    };

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int client = accept(server, (struct sockaddr *)&clientAddr, (unsigned int *)&clientAddrLen);
    if (client == -1)
    {
        cout << "Accept failed!\n";
    }
    // get hello from server
    const char *mes = "Hello client!";
    int send_hello = send(client, mes, strlen(mes), 0);
    if (send_hello <= 0)
    {
        return 0;
    }

    // receive data
    char *buf = new char[BUFFER_SIZE];
    char *p = buf;

    int data_rec = recv(client, buf, BUFFER_SIZE, 0);
    if (data_rec <= 0)
    {
        return 0;
    }

    Computer com;

    size_t namelen;

    memcpy(&namelen, p, sizeof(size_t));
    p += sizeof(size_t);

    memcpy(&com.name, p, namelen);
    p += namelen;

    int size_ds;

    memcpy(&size_ds, p, sizeof(int));
    p += sizeof(int);

    while (size_ds != 0)
    {
        SSD s;
        memcpy(&s.name, p, sizeof(char));
        p += sizeof(char);

        memcpy(&s.size, p, sizeof(int));
        p += sizeof(int);

        com.ds.push_back(s);

        size_ds--;
    }

    cout << "Tên máy tính: " << com.name << endl;
    cout << "Số lương ổ đĩa: " << com.ds.size() << endl;
    for (SSD s : com.ds)
    {
        cout << '\t' << s.name << "-" << s.size << "GB" << endl;
    }

    delete[] buf;

    close(client);
    close(server);
}
