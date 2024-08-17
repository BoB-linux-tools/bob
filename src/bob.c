#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 프로그램 실행 함수
void run_program(const char *program, const char *arg) {
    // 경로를 조합하여 실행
    char command[256];

    // /usr/local/bin 경로에서 프로그램을 실행
    if (strlen(arg) > 0) {
        snprintf(command, sizeof(command), "/usr/local/bin/%s \"%s\"", program, arg);
    } else {
        snprintf(command, sizeof(command), "/usr/local/bin/%s", program);
    }

    system(command);
}

int main(int argc, char *argv[]) {
    // 명령어와 옵션이 올바르게 제공되었는지 확인
    if (argc < 2) {
        fprintf(stderr, "Usage: bob <program> [args]\n");
        return 1;
    }

    // 사용자가 입력한 프로그램 이름과 인수를 실행
    if (argc == 2) {
        run_program(argv[1], "");
    } else {
        run_program(argv[1], argv[2]);
    }

    return 0;
}

