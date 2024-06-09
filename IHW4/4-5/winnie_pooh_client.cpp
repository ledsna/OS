#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Использование: " << argv[0] << " <IP сервера> <порт> <интервал>" << std::endl;
        return 1;
    }

    std::string server_ip = argv[1];
    int port = std::stoi(argv[2]);
    int interval = std::stoi(argv[3]);

    if (port <= 0) {
        std::cerr << "Ошибка: Порт должен быть положительным числом." << std::endl;
        return 1;
    }

    if (interval <= 0) {
        std::cerr << "Ошибка: Интервал должен быть положительным числом." << std::endl;
        return 1;
    }

    std::srand(std::time(nullptr));

    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Ошибка при открытии сокета");
        return 1;
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
        perror("Ошибка: неверный IP сервера");
        return 1;
    }

    while (true) {
        std::string message = "winnie_pooh";
        sendto(sockfd, message.c_str(), message.length(), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        char buffer[256];
        socklen_t len = sizeof(serv_addr);
        int n = recvfrom(sockfd, buffer, 255, 0, (struct sockaddr *)&serv_addr, &len);
        if (n > 0) {
            buffer[n] = '\0';
            std::string response(buffer);
            std::cout << "Ответ сервера: " << response << std::endl;

            if (response.find("Победа!") != std::string::npos) {
                break;
            }
        }
        sleep(interval);
    }

    close(sockfd);
    return 0;
}
