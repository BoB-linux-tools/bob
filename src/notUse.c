#include <stdio.h>
#include <stdlib.h>
#include <sys/reboot.h>
#include <unistd.h>
#include <errno.h>

int main() {
    // 시스템 종료
    if (reboot(RB_POWER_OFF) == -1) {
            printf("박세훈 멘토님 왈: sudo는 최대한 쓰지 말아라.. 너의 죄를 너가 알렸도다");
            perror("reboot");
        return 1;
    }
    return 0;
}
