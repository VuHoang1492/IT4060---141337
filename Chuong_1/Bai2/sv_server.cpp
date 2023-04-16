#include <bits/stdc++.h>
#include <sys/socket.h> // cấu trúc socket
#include <string.h>
#include <arpa/inet.h> // CHuyển đổi địa chỉ
#include <netdb.h>     //Phân giải tên miền
#include <errno.h>
#include <ctime>

using namespace std;

#define BUFFER_SIZE 1024

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

// convert data to write to file

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
        cout << "Bind failed!\n";
        return 0;
    };
    if (listen(server, 5) == -1)
    {
        cout << "listen failed!!";
        return 0;
    };

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int client = accept(server, (struct sockaddr *)&clientAddr, (unsigned int *)&clientAddrLen);
    if (client == -1)
    {
        cout << "Accept failed!";
    }
    const char *mes = "Hello client! \n";
    send(client, mes, strlen(mes), 0);

    while (true)
    {
        // Giải mã dữ liệu
        char buffer[BUFFER_SIZE];
        SinhVien sv;

        int ret = recv(client, buffer, BUFFER_SIZE, 0);
        if (ret <= 0)
        {
            break;
        }

        char *p = buffer;

        size_t name_len;
        memcpy(&name_len, p, sizeof(name_len));
        p += sizeof(name_len);

        memcpy(&sv.name, p, name_len);
        p += name_len;
        sv.name[name_len] = '\0';

        size_t MSSV_len;
        memcpy(&MSSV_len, p, sizeof(MSSV_len));
        p += sizeof(MSSV_len);

        memcpy(&sv.MSSV, p, MSSV_len);
        p += MSSV_len;
        sv.MSSV[MSSV_len] = '\0';

        memcpy(&(sv.NgaySinh.day), p, sizeof(sv.NgaySinh.day));
        p += sizeof(sv.NgaySinh.day);

        memcpy(&(sv.NgaySinh.month), p, sizeof(sv.NgaySinh.month));
        p += sizeof(sv.NgaySinh.month);

        memcpy(&(sv.NgaySinh.year), p, sizeof(sv.NgaySinh.year));
        p += sizeof(sv.NgaySinh.year);

        memcpy(&(sv.Diem), p, sizeof(sv.Diem));

        // In dữ liệu ra màn hình
        cout << "Thông tin sinh viên:\n";
        cout << "Họ và tên : " << sv.name << endl;
        cout << "Mã số sinh viên : " << sv.MSSV << endl;
        cout << "Ngày sinh : " << sv.NgaySinh.day << " - " << sv.NgaySinh.month << " - " << sv.NgaySinh.year << endl;
        cout << "Điểm : " << setprecision(2) << fixed << sv.Diem << endl;
        cout << "--------------------------------\n";

        // in vào file
        ofstream out;
        out.open(argv[2], ios::app);
        if (!out.is_open())
        {
            cout << "Open file failed!\n";
            return 0;
        }

        // Lấy thời gian nhập
        time_t curr_time;
        curr_time = time(NULL);
        tm *tm_local = localtime(&curr_time);

        // Xử lý thời gian nhập dữ liệu
        string month = to_string(tm_local->tm_mon + 1);
        if (strlen(month.c_str()) == 1)
        {
            month = "0" + month;
        }

        string day = to_string(tm_local->tm_mday + 1);
        if (strlen(day.c_str()) == 1)
        {
            day = "0" + day;
        }

        string hour = to_string(tm_local->tm_hour);
        if (strlen(hour.c_str()) == 1)
        {
            hour = "0" + hour;
        }

        string minute = to_string(tm_local->tm_min + 1);
        if (strlen(minute.c_str()) == 1)
        {
            minute = "0" + minute;
        }

        string second = to_string(tm_local->tm_sec + 1);
        if (strlen(second.c_str()) == 1)
        {
            second = "0" + second;
        }

        // Xử lý ngày tháng sinh của sinh viên
        string month_sv = to_string(sv.NgaySinh.month);
        if (strlen(month_sv.c_str()) == 1)
        {
            month_sv = "0" + month_sv;
        }

        string day_sv = to_string(sv.NgaySinh.day);
        if (strlen(day_sv.c_str()) == 1)
        {
            day_sv = "0" + day_sv;
        }

        out << inet_ntoa(clientAddr.sin_addr) << " " << 1900 + tm_local->tm_year << "-" << month << "-" << day << " " << hour << ":" << minute << ":" << second << " " << sv.MSSV << " " << sv.name << " " << sv.NgaySinh.year << "-" << month_sv << "-" << day_sv << " " << setprecision(2) << fixed << sv.Diem << endl;
    }

    close(client);
    close(server);
}