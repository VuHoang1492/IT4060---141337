#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
using namespace std;
int main(int argc, char *argv[])
{
    cout << argv[1] << endl;
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(21);

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("connect() failed");
        return 1;
    }

    char buf[2048];

    // Nhan xau chao
    int len = recv(client, buf, sizeof(buf), 0);
    buf[len] = 0;

    puts(buf);

    char username[64], password[64];

    while (1)
    {
        printf("Nhap username: ");
        scanf("%s", username);
        printf("Nhap password: ");
        scanf("%s", password);

        // Gui lenh USER
        sprintf(buf, "USER %s\r\n", username);
        send(client, buf, strlen(buf), 0);

        len = recv(client, buf, sizeof(buf), 0);
        buf[len] = 0;
        // puts(buf);

        // Gui lenh PASS
        sprintf(buf, "PASS %s\r\n", password);
        send(client, buf, strlen(buf), 0);

        len = recv(client, buf, sizeof(buf), 0);
        buf[len] = 0;
        // puts(buf);

        if (strncmp(buf, "230", 3) == 0)
        {
            printf("Dang nhap thanh cong.\n");
            break;
        }
        else
        {
            printf("Dang nhap that bai. Hay nhap lai.\n");
        }
    }

    // Lay noi dung cua thu muc hien tai
    if (!strcmp(argv[1], "EPSV"))
    {
        // Gui lenh EPSV
        send(client, "EPSV\r\n", 6, 0);

        len = recv(client, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);

        // Xac dinh gia tri cong cua kenh du lieu
        char *pos1 = strstr(buf, "|||") + 3;
        char *pos2 = strchr(pos1, '|');

        int value_length = pos2 - pos1;
        char value[16];
        memcpy(value, pos1, value_length);
        value[value_length] = 0;

        int port = atoi(value);

        // Mo ket noi moi den kenh du lieu
        int data_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        struct sockaddr_in data_addr;
        data_addr.sin_family = AF_INET;
        data_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        data_addr.sin_port = htons(port);

        if (connect(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr)))
        {
            perror("connect() failed");
            return 1;
        }

        // Nhan du lieu o kenh du lieu
        FILE *f = fopen(argv[2], "rb");
        if (f == NULL)
        {
            perror("Failed: ");

            // Gui lenh QUIT
            send(client, "QUIT\r\n", 6, 0);

            close(data_socket);

            // Ket thuc, dong socket
            close(client);
            return 1;
        }

        // Gui lenh STOR ten_file
        sprintf(buf, "STOR %s\r\n", argv[2]);
        // strcpy(buf, "STOR test_upload_file.mp3\r\n");
        send(client, buf, strlen(buf), 0);

        // Phan hoi so 1 cua lenh STOR
        len = recv(client, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);

        while (!feof(f))
        {
            fread(buf, 1, sizeof(buf), f);
            send(data_socket, buf, sizeof(buf), 0);
        }

        fclose(f);

        close(data_socket);

        // Phan hoi so 2 cua lenh RETR
        len = recv(client, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);
    }

    if (!strcmp(argv[1], "PASV"))
    {
        // Gui lenh PASV
        send(client, "PASV\r\n", 6, 0);

        len = recv(client, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);
        char *addr[7];
        int i = 1;

        char *pos2 = strchr(buf, '(');
        char *token;
        token = strtok(pos2 + 1, ",");
        while (token != NULL)
        {
            addr[i] = token;
            token = strtok(NULL, ",");
            i++;
        }
        char ip[64];
        sprintf(ip, "%s.%s.%s.%s", addr[1], addr[2], addr[3], addr[4]);
        addr[6][strlen(addr[6]) - 3] = '\0';
        int port = atoi(addr[5]) * 256 + atoi(addr[6]);

        cout << "Ip Address: " << ip << " Port: " << port << endl;

        // Mo ket noi moi den kenh du lieu
        int data_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        struct sockaddr_in data_addr;
        data_addr.sin_family = AF_INET;
        data_addr.sin_addr.s_addr = inet_addr(ip);
        data_addr.sin_port = htons(port);

        if (connect(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr)))
        {
            perror("connect() failed");
            return 1;
        }

        // Nhan du lieu o kenh du lieu
        FILE *f = fopen(argv[2], "rb");
        if (f == NULL)
        {
            perror("Failed: ");

            // Gui lenh QUIT
            send(client, "QUIT\r\n", 6, 0);

            close(data_socket);

            // Ket thuc, dong socket
            close(client);
            return 1;
        }

        // Gui lenh STOR ten_file
        sprintf(buf, "STOR %s\r\n", argv[2]);
        send(client, buf, strlen(buf), 0);

        // Phan hoi so 1 cua lenh STOR
        len = recv(client, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);

        while (!feof(f))
        {
            fread(buf, 1, sizeof(buf), f);
            send(data_socket, buf, sizeof(buf), 0);
        }

        fclose(f);
        close(data_socket);

        // Phan hoi so 2 cua lenh RETR
        len = recv(client, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);
    }

    // Gui lenh QUIT
    send(client, "QUIT\r\n", 6, 0);

    // Ket thuc, dong socket
    close(client);

    return 0;
}