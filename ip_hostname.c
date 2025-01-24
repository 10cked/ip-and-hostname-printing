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
#define MAX_INTERFACES 64 // Переменная определяющая максимальное количество интерфейсов

void get_hostname(char *hostname, size_t size) { // Функция, получающая имя хоста и проверяющая успешность получения
    if (gethostname(hostname, size) != 0) {
        perror("Ошибка при получении имени хоста"); // Вывод ошибки
        exit(EXIT_FAILURE); // Завершение программы в случае ошибки
    }
}

void get_all_interfaces_IPs() { // Функция, получающая все интерфейсы системы, а так же их адреса и выводящая их
    int sockfd;
    struct ifconf ifc;
    struct ifreq ifr[MAX_INTERFACES];
    int interfaces_count;

    // Создание сокета для получения информации об интерфейсах:
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { // Проверка успешности создания сокета
        perror("Ошибка создания сокета"); // Вывод ошибки
        exit(EXIT_FAILURE); // Завершение программы в случае ошибки
    }

    // Инициализация структуры ifconf для работы с ioctl:
    ifc.ifc_len = sizeof(ifr); // Размер буфера для хранения списка интерфейсов
    ifc.ifc_req = ifr;  // Назначение переменной, в которую будет передаваться информация об интерфейсах

    // Вызов ioctl для получения списка интерфейсов:
    if (ioctl(sockfd, SIOCGIFCONF, &ifc) == -1) {  // Получение списка интерфейсов и проверка успешности получения
        perror("Ошибка вызова ioctl SIOCGIFCONF"); // Вывод ошибки
        close(sockfd); // Закрытие сокета для освобождения памяти
        exit(EXIT_FAILURE); // Завершение программы в случае ошибки
    }

    // Подсчёт и вывод количества интерфейсов:
    interfaces_count = ifc.ifc_len / sizeof(struct ifreq);
    printf("Найдено интерфейсов: %d\n", interfaces_count);

    // Перебор и вывод интерфейсов с ip-адресами:
    for (int i = 0; i < interfaces_count; i++) {
        struct sockaddr_in *ip = (struct sockaddr_in *)&ifr[i].ifr_addr; // Извлечение адреса и его преобразование в строковый формат:
        printf("Интерфейс %d:\n Имя: %s, IP-адрес: %s\n",
               i + 1, ifr[i].ifr_name, inet_ntoa(ip->sin_addr));
    }

    close(sockfd); // Закрытие сокета для освобождения памяти
}

int main() {
    char hostname[HOSTNAME_LEN];

    // Получение имени хоста и его вывод:
    get_hostname(hostname, sizeof(hostname));
    printf("Имя хоста: %s\n", hostname);

    // Вывод списка интерфейсов и их IP-адресов
    get_all_interfaces_IPs();

    return 0;
}
