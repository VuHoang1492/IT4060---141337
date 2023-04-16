#include <bits/stdc++.h>
#include <sys/socket.h> // cấu trúc socket
#include <string.h>
#include <arpa/inet.h> // CHuyển đổi địa chỉ
#include <netdb.h>     //Phân giải tên miền
using namespace std;

int main(int argc, char *argv[])
{

    int tcp_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tcp_client == -1)
    {
        cout << "create socket failed\n";
        return (1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(stoi(argv[2]));
    cout << "Waiting connecting..." << endl;
    int res = connect(tcp_client, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1)
    {
        cout << "Falied to connect!\n";
        return 0;
    }

    // get hello from server
    char mes[256];
    int ret = recv(tcp_client, mes, sizeof(mes), 0);
    cout << mes;

    // send data to server
    while (true)
    {
        string buf;
        getline(cin, buf);
        if (buf.empty())
            break;
        send(tcp_client, buf.c_str(), strlen(buf.c_str()), 0); // c_str chuyen string thanh char*
    }

    close(tcp_client);
}
