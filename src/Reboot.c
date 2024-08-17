#include <stdio.h>
#include <stdlib.h>
#include <sys/reboot.h>
#include <unistd.h>
#include <errno.h>

int main() {
    // 시스템 종료
    if (reboot(RB_POWER_OFF) == -1) {
            perror("reboot");
        return 1;
    }
    return 0;
}
