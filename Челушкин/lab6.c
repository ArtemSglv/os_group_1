#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>




int main(int argc , char *argv[])
{

int socket_desc;
struct sockaddr_in server;
char *message, server_reply[2000];

socket_desc = socket(AF_INET , SOCK_STREAM , 0);
if (socket_desc == -1)
{
printf("сокет не создался");
}
server.sin_addr.s_addr = inet_addr("127.0.0.1");
server.sin_family = AF_INET;
server.sin_port = htons( 11211 );
if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
{
puts("ошибка подключения");
return 1;
}
puts("Подключено\n");
message= "add loveosi 2 60 4\r\ntest\r\n";
if( send(socket_desc , message , strlen(message) , 0) < 0)
{
puts("Не отправлено");
return 1;
}
puts("Переданно\n");
if(recv(socket_desc,server_reply, 2000,0)<0)
{
puts("не удача");
}
puts("Ответ\n");
puts(server_reply);


return 0;

}
