#include<bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


using namespace std;


void *thread_proc(void *listener);

int main()
{
     int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(9000); 

    if(bind(listener,(sockaddr*)&addr,sizeof(addr))){
        perror("bind() failed\n");
    }
    if(listen(listener,5)){
        perror("listen() failed\n");
    }
    
    int num_threads = 8;
    pthread_t thread_id;
    for (int i = 0; i < num_threads; i++) {
        int ret = pthread_create(&thread_id, NULL, thread_proc, (void*)&listener);
            if (ret != 0)
                printf("Could not create new thread.\n");
                sched_yield();
            }
    pthread_join(thread_id, NULL);
    close(listener);
    return 0;

}

void *thread_proc(void *param){
    int listener = *(int*)param;
    while (true)
    {
        int client = accept(listener,NULL,NULL);
        cout << "From client: " << client << endl;
        char buf[256];
        int ret = recv(client, buf, sizeof(buf), 0);
        buf[ret] = '\0';
        cout << buf << endl;
        // Trả lại kết quả cho client
        const char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Xin chao cac ban</ h1></ body></ html> ";
        send(client, msg, strlen(msg), 0);
        // Đóng kết nối
         close(client);

    }
    return NULL;
    
}
