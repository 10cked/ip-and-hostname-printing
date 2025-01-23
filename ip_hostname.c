#include <stdio.h> // Предоставляет функции ввода-вывода, такие как printf для вывода текста и perror для отображения ошибок
#include <stdlib.h> // Содержит функции управления программой, например, exit для завершения с кодом ошибки
#include <unistd.h> // Используется для работы с функцией gethostname, которая возвращает имя хоста
#include <string.h> // Содержит функции работы со строками, например strncpy
#include <sys/socket.h> // Обеспечивает доступ к функциям работы с сокетами (например, создание сокета с помощью socket)
#include <netinet/in.h> // Определяет структуры и функции для работы с сетевыми адресами (например, struct sockaddr_in для IPv4)
#include <arpa/inet.h> // Содержит функции преобразования IP-адресов (например, inet_ntoa для преобразования адреса из бинарного в строковый формат)
#include <sys/ioctl.h> // Позволяет управлять устройствами и интерфейсами, предоставляя доступ к функциям ioctl (например, для получения IP-адреса интерфейса)
#include <net/if.h> // Определяет структуру ifreq, используемую для получения информации о сетевом интерфейсе

#define HOSTNAME_LEN 256 // Переменная определяющая максимальную длину имени хоста
#define INTERFACE_NAME "eth0" // Переменная определяющая имя интерфейса

void get_hostname(char *hostname, size_t size) { // Функция, получающая имя хоста и проверяющая успешность получения
    if (gethostname(hostname, size) != 0) {
        perror("Ошибка при получении имени хоста"); // Вывод ошибки
        exit(EXIT_FAILURE); // Завершение программы в случае ошибки
    }
}

void get_interface_ip(const char *interface, char *ip_address, size_t size) {  // Функция, получающая ip-адрес и проверяющая успешность получения
    int fd;
    struct ifreq ifr;

    // Создание сокета для обращения к интерфейсу:
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) { // Проверка успешности создания сокета
        perror("Ошибка создания сокета"); // Вывод ошибки
        exit(EXIT_FAILURE); // Завершение программы в случае ошибки
    }

    // Копирование имени интерфейса в структуру ifreq:
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    
    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) { // Получение ip-адреса и проверка успешности получения
        perror("Ошибка получения IP-адреса интерфейса"); // Вывод ошибки
        close(fd); // Закрытие сокета для освобождения памяти
        exit(EXIT_FAILURE); // Завершение программы в случае ошибки
    }

    // Извлечение адреса и его преобразование в строковый формат:
    struct sockaddr_in *ip = (struct sockaddr_in *)&ifr.ifr_addr;
    strncpy(ip_address, inet_ntoa(ip->sin_addr), size);

    close(fd);  // Закрытие сокета для освобождения памяти
}

int main() {
    char hostname[HOSTNAME_LEN];
    char ip_address[INET_ADDRSTRLEN];

    // Получение имени хоста и его вывод:
    get_hostname(hostname, sizeof(hostname));
    printf("Имя хоста: %s\n", hostname);

    // Получение IP-адреса указанного интерфейса и его вывод:
    get_interface_ip(INTERFACE_NAME, ip_address, sizeof(ip_address));
    printf("IP-адрес интерфейса %s: %s\n", INTERFACE_NAME, ip_address);

    return 0;
}
