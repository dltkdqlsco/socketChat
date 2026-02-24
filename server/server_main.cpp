#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

constexpr uint16_t PORT = 9000;
constexpr int BUFFER_SIZE = 1024;
std::vector<int> SERVER_LIST;
std::mutex LIST_LOCK;

void broadcast(int socket, char* buffer){
    std::lock_guard<std::mutex> lock(LIST_LOCK);
    for(int target_socket : SERVER_LIST){
        if(target_socket != socket){
            send(target_socket, buffer, BUFFER_SIZE, 0);
        }
    }
}

void manageClient(int socket, struct sockaddr_in* sock_addr, socklen_t* addrSize){
    char buffer[BUFFER_SIZE];
    while(1){
        // null 값을 넣기 위해서 -1를 해준다.
        ssize_t n = recv(socket, buffer, BUFFER_SIZE - 1, 0);
        if(n <= 0){
            strcpy(buffer, "누군가 연결이 끊겼습니다.");
            broadcast(socket, buffer);
            break;
        }
        buffer[n] = '\0';
        printf("%s\n", buffer);
        broadcast(socket, buffer);
    }

    printf("누군가 연결이 끊겼습니다.\n");

    std::lock_guard<std::mutex> lock(LIST_LOCK);
    for(auto it = SERVER_LIST.begin(); it != SERVER_LIST.end(); it++){
        if(*it == socket){
            SERVER_LIST.erase(it);
            break;
        }
    }
    close(socket);
    delete sock_addr;
    delete addrSize;

}

int main(void){
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){

        perror("socket failed");
        return 1;
    }

    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind()는 소켓과 주소를 하나로 묶어주는 과정이다. 
    // 그래서 이 과정이 성공하면 운영체제가 9000번으로 오는 데이터는 이 소켓으로 보내주면 되겠구나라고 인식하게 된다. 
    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){

        perror("bind failed");
        close(server_socket);
        return 1;
    }

    // listen는 한번만 하면 된다. 사용자가 들어올 수 있게 채팅 서버를 연다고 생각하면 된다.
    // SOMAXCONN는 대기 줄의 길이를 운영체제가 허용하는 최댓값으로 설정하겠다는 뜻이다. 
    if(listen(server_socket, SOMAXCONN) == -1){
        perror("listen failed");
        close(server_socket);
        return 1;
    }

    printf("Server is Running... \n");

    while(1){

        sockaddr_in* client_addr = new sockaddr_in;
        socklen_t* client_addr_size = new socklen_t(sizeof(sockaddr_in));

        // 누가 접속을 안하면 while문을 계속 도는게 아니라 accept 함수가 블로킹 함수이기 때문에 대기하고 있다가 누군가가 접속을 하면 다시 깨어나서 실행을 하게 된다. 
        int client_socket = accept(server_socket, (struct sockaddr*)client_addr, client_addr_size);

        if(client_socket == -1){

            perror("accetp failed");
            delete client_addr;
            delete client_addr_size;
            continue;
        }

        printf("누군가 접속했습니다.\n");
        char welcome_msg[BUFFER_SIZE] = "누군가 접속했습니다.";

        std::lock_guard<std::mutex> lock(LIST_LOCK);
        SERVER_LIST.push_back(client_socket);
        for(int sock : SERVER_LIST){
            send(sock, welcome_msg, BUFFER_SIZE, 0);
        }

        std::thread t(manageClient, client_socket, client_addr, client_addr_size);
        t.detach();
    }
    close(server_socket);
    return 0;
}

