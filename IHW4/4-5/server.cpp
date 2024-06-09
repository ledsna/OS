#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <vector>
#include <atomic>
#include <arpa/inet.h>
#include <cstring>
#include <mutex>

std::mutex hive_mutex;
std::atomic<bool> running(true); // Флаг для завершения всех процессов

int honey = 0;
const int max_honey = 30;
int total_bees = 0;
int collector_bees = 0;
int guard_bees = 0;
bool winnie_the_pooh_sleeping = true;

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len) {
    char buffer[256];
    int n;

    n = recvfrom(sockfd, buffer, 255, 0, (struct sockaddr *)&client_addr, &client_len);
    if (n <= 0) return;

    buffer[n] = '\0';
    std::string message(buffer);

    std::string response;
    bool log_event = false;
    bool exit_program = false;
    std::lock_guard<std::mutex> lock(hive_mutex);

    if (message == "collect_honey") {
        if (honey >= max_honey) {
            response = "Улей полон. Все пчёлы переходят к охране улья. Победа!";
            guard_bees += collector_bees;
            collector_bees = 0;
            running = false; // Остановка всех процессов
            log_event = true;
            exit_program = true;
        } else if (guard_bees > 1) {
            honey++;
            collector_bees++;
            guard_bees--;
            response = "Мёд собран. Количество мёда: " + std::to_string(honey) +
                       ", Пчёл-сборщиков: " + std::to_string(collector_bees) +
                       ", Пчёл-охранников: " + std::to_string(guard_bees);
            log_event = true;
        } else {
            response = "Нельзя собрать мёд. Не хватает охранников.";
        }
    } else if (message == "guard_hive") {
        if (collector_bees > 0) {
            collector_bees--;
            guard_bees++;
            response = "Пчела становится охранником. Количество мёда: " + std::to_string(honey) +
                       ", Пчёл-сборщиков: " + std::to_string(collector_bees) +
                       ", Пчёл-охранников: " + std::to_string(guard_bees);
            log_event = true;
        }
    } else if (message == "winnie_pooh") {
        if (winnie_the_pooh_sleeping) {
            if (honey >= max_honey / 2) {
                winnie_the_pooh_sleeping = false;
            } else {
                response = "Медведь спит.";
                sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr *)&client_addr, client_len);
                return;
            }
        }

        if (honey >= max_honey / 2 && guard_bees < 3) {
            honey = 0;
            winnie_the_pooh_sleeping = true;
            response = "Мёд украден. Количество мёда: " + std::to_string(honey) +
                       ", Пчёл-сборщиков: " + std::to_string(collector_bees) +
                       ", Пчёл-охранников: " + std::to_string(guard_bees);
            log_event = true;
        } else if (guard_bees >= 3) {
            response = "Медведя укусили пчёлы. Количество мёда: " + std::to_string(honey) +
                       ", Пчёл-сборщиков: " + std::to_string(collector_bees) +
                       ", Пчёл-охранников: " + std::to_string(guard_bees);
            log_event = true;
        } else {
            response = "Не удалось украсть мёд. Количество мёда: " + std::to_string(honey) +
                       ", Пчёл-сборщиков: " + std::to_string(collector_bees) +
                       ", Пчёл-охранников: " + std::to_string(guard_bees);
            log_event = true;
        }
    } else if (message == "check_hive") {
        response = "Количество мёда: " + std::to_string(honey) +
                   ", Пчёл-сборщиков: " + std::to_string(collector_bees) +
                   ", Пчёл-охранников: " + std::to_string(guard_bees) +
                   ", Медведь спит: " + (winnie_the_pooh_sleeping ? "Да" : "Нет");
    } else {
        response = "Неизвестная команда.";
    }

    if (log_event) {
        std::cout << response << std::endl;
    }

    sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr *)&client_addr, client_len);

    if (exit_program) {
        running = false;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Использование: " << argv[0] << " <порт> <количество пчёл>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    if (port <= 0) {
        std::cerr << "Ошибка: Порт должен быть положительным числом." << std::endl;
        return 1;
    }

    total_bees = std::stoi(argv[2]);
    if (total_bees < 4) {
        std::cerr << "Ошибка: Количество пчёл должно быть >= 4." << std::endl;
        return 1;
    }

    guard_bees = total_bees; // Все пчёлы начинают с охраны улья
    collector_bees = 0;

    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t client_len = sizeof(cli_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Ошибка при открытии сокета");
        return 1;
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Ошибка при привязке");
        return 1;
    }

    while (running) {
        client_len = sizeof(cli_addr);
        handle_client(sockfd, cli_addr, client_len);
    }

    close(sockfd);
    return 0;
}
