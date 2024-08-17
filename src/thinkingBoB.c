#include <stdio.h>
#include <string.h>

// ANSI 색상 코드 정의
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

void print_colorful_string(char *str) {
    const char *colors[] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};
    int len = strlen(str);
    printf("\n");
    printf("\n");
    printf("\n");
    
    // 말풍선 윗부분 출력 (오른쪽으로 이동)
    printf("           ");
    for (int i = 0; i < len + 4; i++) {
        printf("%s-", colors[i % 6]);
    }
    printf(RESET "\n");

    // 말풍선 안에 문자열 출력 (오른쪽으로 이동)
    printf("           %s| " RESET, colors[0]);
    for (int i = 0; i < len; i++) {
        printf("%s%c", colors[i % 6], str[i]);
    }
    printf("%s |\n", colors[0]);

    // 말풍선 아랫부분 출력 (오른쪽으로 이동)
    printf("           ");
    for (int i = 0; i < len + 4; i++) {
        printf("%s-", colors[i % 6]);
    }
    printf(RESET "\n");

    // 말풍선 꼬리 출력 (오른쪽으로 이동)
    printf("               %s\\\n", GREEN);
    printf("                `%s\\\n", YELLOW);

    // 추가 아스키 아트 출력 (오른쪽으로 이동, BoB만 색 적용)
    printf(RESET);
    printf("                          \\\\\\\\\\\\\n");
    printf("                        \\\\\\\\\\\\\\\\\\\n");
    printf("                      \\\\\\\\\\\\\\\\\\\\\\\\\\\n");
    printf("                      |C>   // )\\\\| \n");
    printf("                     /    || ,'/////| \n");
    printf("                    (,    ||   ///// \n");
    printf("                     \\\\  ||||//''''| \n");
    printf("                      |||||||     _| \n");
    printf("     |\\                `````\\____/ \\ \n");
    printf("     | \\                       \\____/ \\ \n");
    printf("     |  \\                      /       \\ \n");
    printf("     |   \\                    |         \\ \n");
    printf("      \\   \\              ____ /          | \n");
    printf("       \\   \\         __/ |   |  ");
    printf("%sBoB", MAGENTA);  // BoB에만 색 적용
    printf("%s      | \n", RESET);
    printf("        \\   |-------///_/----|      |    | \n");
    printf("_________\\  |            _|__|_____/    /| \n");
    printf("          \\ |        __/ |             / | \n");
    printf("___________\\|       ///_/-------------/  | \n");
    printf("            =============,'  |            | \n");
    printf("                          |  |            |\n");
    printf("|=========================| \n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s \"Your string here\"\n", argv[0]);
        return 1;
    }

    print_colorful_string(argv[1]);

    return 0;
}

