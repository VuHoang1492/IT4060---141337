#include <bits/stdc++.h>
#include <sys/socket.h> // cấu trúc socket
#include <string.h>
#include <arpa/inet.h> // CHuyển đổi địa chỉ
#include <netdb.h>     //Phân giải tên miền
using namespace std;

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

    int info_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (info_client == -1)
    {
        cout << "create socket failed\n";
        return (1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(stoi(argv[2]));
    cout << "Waiting connecting..." << endl;
    int res = connect(info_client, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1)
    {
        cout << "Falied to connect!\n";
        return 0;
    }

    // get hello from server
    char mes[256];
    recv(info_client, mes, sizeof(mes), 0);
    cout << mes << endl;

    // input data to server
    Computer com;
    cout << "Nhập tên máy tính: \n";
    cin.getline(com.name, 50);
    while (true)
    {
        int choose;
        cout << "1.Thêm ổ cứng.\n";
        cout << "Bất kì để thoát.\n";
        cin >> choose;
        if (choose != 1)
        {
            break;
        }
        else
        {
            SSD s;
            cout << "Nhập tên ổ: \n";
            cin.ignore();
            s.name = getchar();
            cout << "Nhập dung lượng: \n";
            cin.ignore();
            cin >> s.size;
            com.ds.push_back(s);
        }
    }

    size_t namelen = strlen(com.name);

    size_t total = sizeof(size_t) + namelen + sizeof(int) + sizeof(char) * com.ds.size() + sizeof(int) * com.ds.size();

    char *data = new char[total];

    char *p = data;

    memcpy(p, &namelen, sizeof(size_t));
    p += sizeof(size_t);

    memcpy(p, &com.name, namelen);
    p += namelen;

    int size_ds = com.ds.size();
    memcpy(p, &size_ds, sizeof(int));
    p += sizeof(int);

    for (SSD s : com.ds)
    {
        memcpy(p, &s.name, sizeof(char));
        p += sizeof(char);

        memcpy(p, &s.size, sizeof(int));
        p += sizeof(int);
    }

    int ret = send(info_client, data, total, 0);
    if (ret <= 0)
    {
        return 0;
    }

    delete[] data;

    close(info_client);
}
