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
    int ret = recv(info_client, mes, sizeof(mes), 0);
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
            cin.ignore();
            cout << "Nhập tên ổ: \n";
            s.name = getchar();
            cout << "Nhập dung lượng: \n";
            cin.ignore();
            cin.getline(s.DungLuong, 10);
            com.ds.push_back(s);
        }
    }
    cout << com.name << endl;
    cout << com.ds.size() << endl;
    for (SSD s : com.ds)
    {
        cout << s.name << "-" << s.DungLuong << endl;
    }

    size_t namelen = strlen(com.name);
    size_t dslen = 0;
    for (SSD s : com.ds)
    {
        string data(1, s.name);
        data += "-" + s.DungLuong;
        size_t datalen = 
        dslen += strlen(data.c_str());
    }

    close(info_client);
}
