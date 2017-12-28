/*
Отправить сообщение на сервер [StatsD](https://github.com/etsy/statsd/) `тест:1|g` 
и убедиться в его получении через консоль сервера (сервер должен быть сконфигурирован с бэкэндом Console).
*/

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char message[] = ""; //Запрос, который отправится на statsd сервер

int main(int argc, char *argv[])
{
    if ( argc != 3 ) //Если количество аргументов не равно 3
    {
        printf( "usage: %s <metricname> <value>|<type>\n", argv[0] );
        exit(1);
    }
    int sock; //Сокет
    struct sockaddr_in addr; //Адрес statsd сервера

    //Инициализация сокета
    //AF_INET для сетевого протокола IPv4
    //SOCK_DGRAM ~ UDP
    //PROTOCOL = 0 (стандартное значение)
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }
    printf("Socket open\n");

    strcat(message, argv[1]);
    strcat(message, ":");
    strcat(message, argv[2]);
    printf("Message: %s\n",message);

    addr.sin_family = AF_INET; //Указываем тип адреса
    addr.sin_port = htons(8125); //Указываем порт
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    printf("Connecting to Statsd...\n");
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }
    printf("Connected to Statsd\n");
    
    send(sock, message, strlen(message), 0);

    close(sock);//Закрываем сокет
    printf("Socket is closed\n");

    return 0;
}
