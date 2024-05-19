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

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Ошибка при открытии сокета");
        return 1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
        perror("Ошибка: неверный IP сервера");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Ошибка при подключении");
        return 1;
    }

    while (true) {
        std::string message;

        // Определение действия с учетом вероятности
        int random_value = std::rand() % 100;
        if (random_value < 40) {
            message = "collect_honey";
        } else if (random_value < 70) {
            message = "guard_hive";
        } else {
            continue; // Ничего не делать
        }

        write(sockfd, message.c_str(), message.length());
        char buffer[256];
        int n = read(sockfd, buffer, 255);
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
