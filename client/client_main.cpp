#include <iostream>
#include <string>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_pton 사용을 위해 추가
#include <unistd.h>
#include <cstring>

constexpr uint16_t PORT = 9000;
constexpr int BUFFER_SIZE = 1024;
constexpr char ip[] = "127.0.0.1";

void receive(int socket){

    char buffer[BUFFER_SIZE];
    while(1){
        ssize_t n = recv(socket, buffer, BUFFER_SIZE - 1, 0);
        if(n <=0){
            printf("서버가 종료되었습니다.\n");
            break;
        }
        printf("%s\n", buffer);
    }
}

int main(void){
    char name[100];
    char buffer[BUFFER_SIZE];
    char send_buffer[BUFFER_SIZE + 110];

    printf("RUN Client...\n");
    printf("이름 : ");
    scanf("%s", name);

    while(getchar() != '\n');

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket == -1){

        perror("socket error");
        return 1;
    }

    struct sockaddr_in clientSocket_addr = {};
    clientSocket_addr.sin_family = AF_INET;
    clientSocket_addr.sin_port = htons(PORT);
    // ip를 바이트 순서로 변환하는 함수는 htonl이라고 했는데 왜 여기서는 inet_addr()를 사용했는가?
    // 그 이유는 이 코드에서 ip가 문자열이기 때문이다. 이 함수는 문자열을 읽어서 컴퓨터가 이해할 수 있는 32비트
    // 정수 형태로 바꾸고, 동시에 네트워크 바이트 순서로 변환까지 해준다.
    // 반면 htonl()는 이미 정수로 된 ㄴ데이터를 네트워크술서로 바꿀 때 사용된다.
    clientSocket_addr.sin_addr.s_addr = inet_addr(ip);

    if(connect(clientSocket, (struct sockaddr*)&clientSocket_addr, sizeof(clientSocket_addr)) == -1){
        perror("connect error");
        close(clientSocket);
        return 1;
    }

    std::thread receiveThread(receive, clientSocket);
    receiveThread.detach();

    while(1){

        if(fgets(buffer, BUFFER_SIZE, stdin) == NULL) break;
        buffer[strcspn(buffer, "\n")] = 0;

        sprintf(send_buffer, "%s >> %s", name, buffer);

        if(send(clientSocket, send_buffer, strlen(send_buffer), 0) <= 0){
            printf("Exit\n");
            break;
        }
    }
    printf("END Client...\n");
    close(clientSocket);
    return 0;
}
