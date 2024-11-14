#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd = open("/dev/kmsg", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open /dev/kmsg");
        return 1;
    }

    char buffer[256];
    while (1) {
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
        if (bytesRead < 0) {
            perror("Failed to read from /dev/kmsg");
            break;
        }

        buffer[bytesRead] = '\0'; // Null-terminate the string
        if (bytesRead > 0) {
            printf("Read %zd bytes: %s", bytesRead, buffer);
            fflush(stdout);
        }

        sleep(1); // Sleep for a second between reads
    }

    close(fd);
    return 0;
}

