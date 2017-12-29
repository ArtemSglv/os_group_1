#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>


int main() {

    char *filename = "/var/run/collectd-unixsock";

    int sock = socket(PF_UNIX, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_un addr;
    strncpy(addr.sun_path, filename, sizeof(addr.sun_path));
    addr.sun_family = AF_UNIX;
    int size = sizeof(addr);

    if (connect(sock, (struct sockaddr *)&addr, size) < 0) {
        perror("connect");
        exit(2);
    }

    char *message = "PUTVAL localhost.localdomain/interface-lo/if_octets interval=10 1179574444:123:456\n";
    char buf[1024];

    send(sock, message, strlen(message), 0);
    int received = recv(sock, buf, sizeof(buf), 0);
    buf[received] = '\0';
    printf(buf);

    close(sock);

    return 0;
}
