#include <bits/stdc++.h>
#include <sys/socket.h> // cấu trúc socket
#include <string.h>
#include <arpa/inet.h> // CHuyển đổi địa chỉ
#include <netdb.h>     //Phân giải tên miền
using namespace std;

struct Date
{
    int day;
    int month;
    int year;
};
struct SinhVien
{
    char MSSV[20];
    char name[50];
    Date NgaySinh;
    float Diem;
};

int main(int argc, char *argv[])
{

    int sv_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sv_client == -1)
    {
        cout << "create socket failed\n";
        return (1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(stoi(argv[2]));

    cout << "Waiting connecting..." << endl;
    int res = connect(sv_client, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1)
    {
        cout << "Falied to connect!\n";
        return (1);
    }

    // get hello from server
    char mes[256];
    int ret = recv(sv_client, mes, sizeof(mes), 0);
    cout << mes;

    // send data to server
    while (true)
    {
        // nhập thông tin
        int choose;
        cout << "Nhập 1 để sinh viên." << endl;
        cout << "Nhập bất kì để thoát" << endl;
        cin >> choose;
        if (choose == 1)
        {
            // Nhập dữ liệu
            SinhVien sv;
            cin.ignore();
            cout << "Nhập MSSV: ";
            cin.getline(sv.MSSV, 20);
            cout << endl;
            cout << "Nhập họ và tên: ";
            cin.getline(sv.name, 50);
            cout << endl;
            cout << "Nhập ngày sinh: ";
            cin >> sv.NgaySinh.day;
            cout << endl;
            cout << "Nhập tháng sinh: ";
            cin >> sv.NgaySinh.month;
            cout << endl;
            cout << "Nhập năm sinh: ";
            cin >> sv.NgaySinh.year;
            cout << endl;
            cout << "Nhập điểm trung bình các môn: ";
            cin >> sv.Diem;
            cout << endl;

            // Đóng gói dữ liệu

            size_t name_len = strlen(sv.name);
            size_t MSSV_len = strlen(sv.MSSV);
            size_t total_len = sizeof(size_t) * 2 + name_len + MSSV_len + sizeof(int) * 3 + sizeof(float);

            char *data = new char[total_len];
            char *p = data;

            memcpy(p, &name_len, sizeof(name_len));
            p += sizeof(name_len);

            memcpy(p, sv.name, name_len);
            p += name_len;

            memcpy(p, &MSSV_len, sizeof(MSSV_len));
            p += sizeof(MSSV_len);

            memcpy(p, sv.MSSV, MSSV_len);
            p += MSSV_len;

            memcpy(p, &(sv.NgaySinh.day), sizeof(sv.NgaySinh.day));
            p += sizeof(sv.NgaySinh.day);
            memcpy(p, &(sv.NgaySinh.month), sizeof(sv.NgaySinh.month));
            p += sizeof(sv.NgaySinh.month);
            memcpy(p, &(sv.NgaySinh.year), sizeof(sv.NgaySinh.year));
            p += sizeof(sv.NgaySinh.year);
            memcpy(p, &(sv.Diem), sizeof(sv.Diem));

            int ret = send(sv_client, data, total_len, 0);
            if (ret == -1)
            {
                break;
            }
            else
            {
                cout << "Gửi thành công!" << endl;
                cout << "-------------------------" << endl;
            }
        }
        else
        {
            break;
        }
    }

    close(sv_client);
}
