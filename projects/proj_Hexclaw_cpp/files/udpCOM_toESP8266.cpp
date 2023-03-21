#include <stdio.h>
#include <sys/socket.h>

int main(int argc, char &argv[]) {
    int socket_desc;
    struct sockaddr_in server;
    char* message, server_reply[2000];

    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket == -1) printf("Could not create socket.");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(4210);

    if(bind(socket_desc,(struct sockaddr*)&server, sizeof(server)) < 0) puts("bind failed");
    puts("port bind done\n");


    message = "test, test, test. This a test";
    if(send(socket_desc, message, strlen(message), 0) < 0) {
        puts("Send failed");
        return 1;
    }
    puts("Data sent\n");

    if(recv(socket_desc, server_reply, 2000, 0) < 0) {
        puts("recv_failed");
    }
    puts("Reply received: ");
    puts(server_reply);

    return 0;
}
