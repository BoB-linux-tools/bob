#include <ncurses.h>
#include <unistd.h> // sleep()
#include <time.h>   // clock()

#define OBSTACLE_CHAR '#'
#define DINO_CHAR 'O'
#define OBSTACLE_SIZE 4
#define DINO_SIZE 7
#define OBSTACLE_SPEED 1
#define GAME_SPEED 50000 // 마이크로초 단위
#define JUMP_DURATION 20 // 점프와 착지 동안의 프레임 수 (늘림)
#define FRAME_RATE 30 // 초당 프레임 수
#define NUM_OBSTACLE_SHAPES 3 // 장애물 모양의 개수
#define SCORE_UPDATE_INTERVAL 1
#define INITIAL_OBSTACLE_SPEED 1
#define INITIAL_GAME_SPEED 50000 // 마이크로초 단위
#define OBSTACLE_SPEED_INCREMENT 0.01
#define GAME_SPEED_DECREASE 500 // 마이크로초 단위
#define SPEED_INCREASE_INTERVAL 100 // 시간 간격 (ms 단위)

double obstacle_speed = INITIAL_OBSTACLE_SPEED;
int game_speed = INITIAL_GAME_SPEED;
clock_t last_speed_increase_time;

const char *obstacle_shapes[NUM_OBSTACLE_SHAPES][4] = {
    {
        "#  #",
        "####",
        " ##",
        " ## "
    },
    {
        " ## ",
        "####",
        "####",
        " ## "
    },
    {
        "#  #",
        "#  #",
        "#  #",
        "####"
    }
};

const char gamza[7][8] = {
    "********",
    "*      *",
    "* **** *",
    "* *  * *",
    "* **** *",
    "  ****  ",
    "  ****  "
};

typedef struct {
    int x, y;
    int isJumping;
    int jumpHeight;
    int initialY; // 초기 Y 위치 저장
    int jumpTimer; // 점프 및 착지 타이머
} Dino;

typedef struct {
    int x, y;
    int dx, dy;
    int type; // 장애물 유형
    int shapeIndex; // 모양 인덱스
} Obstacle;

int score = 0;
int high_score; // 최고 기록
clock_t score_start_time;

void update_score() {
    clock_t current_time = clock();
    double elapsed_time = (double)(current_time - score_start_time) / CLOCKS_PER_SEC * 1000; // ms 단위로 변환

    if (elapsed_time >= SCORE_UPDATE_INTERVAL) {
        score++;
        score_start_time = current_time; // 점수 시작 시간을 현재 시간으로 재설정
    }

    if (score > high_score) {
        high_score = score; // 최고 기록 갱신
    }
}

void initialize_game(Dino *dino, Obstacle *obstacle, int win_width, int win_height) {
    dino->x = 5;
    dino->y = win_height - DINO_SIZE;
    dino->initialY = dino->y; // 초기 위치 저장
    dino->isJumping = 0;
    dino->jumpHeight = 0;
    dino->jumpTimer = 0;

    obstacle->x = win_width - OBSTACLE_SIZE;
    obstacle->y = win_height - OBSTACLE_SIZE;
    obstacle->shapeIndex = rand() % NUM_OBSTACLE_SHAPES; // 랜덤 모양 선택

    score = 0; // 점수 초기화
    score_start_time = clock(); // 점수 시작 시간 설정

    obstacle_speed = INITIAL_OBSTACLE_SPEED; // 장애물 속도 초기화
    game_speed = INITIAL_GAME_SPEED; // 게임 속도 초기화
    last_speed_increase_time = clock(); // 마지막 속도 증가 시간 초기화
}

void draw_game(WINDOW *win, Dino *dino, Obstacle *obstacle, int win_width, int win_height) {
    werase(win);

       for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 8; j++) {
            if (dino->y + i < win_height && dino->x + j < win_width) {
                mvwaddch(win, dino->y + i, dino->x + j, gamza[i][j]);
            }
        }
    }

    // Draw Obstacle
    for (int i = 0; i < OBSTACLE_SIZE; i++) {
        for (int j = 0; j < OBSTACLE_SIZE; j++) {
            if (obstacle->y + i < win_height && obstacle->x + j < win_width)
                mvwaddch(win, obstacle->y + i, obstacle->x + j, obstacle_shapes[obstacle->shapeIndex][i][j]);
        }
    }

    mvwprintw(win, 0, 0, "Score: %d", score);
    mvwprintw(win, 1, win_width - 20, "High Score: %d", high_score);
    wrefresh(win);
}

void update_dino(Dino *dino, int win_height) {
    if (dino->isJumping) {
        if (dino->jumpTimer < JUMP_DURATION) {
            if (dino->jumpTimer < JUMP_DURATION / 2) {
                if (dino->y > 0) { // 캐릭터가 화면 밖으로 나가지 않도록 조정
                    dino->y--;
                }
            } else {
                if (dino->y < dino->initialY) {
                    dino->y++;
                }
            }
            dino->jumpTimer++;
        } else {
            dino->isJumping = 0;
            dino->jumpTimer = 0;
            dino->y = dino->initialY; // 점프 후 초기 위치로 복귀
        }
    }
}

void update_obstacle(Obstacle *obstacle, int win_width) {
    
    obstacle->x -= (int)obstacle_speed;
    if (obstacle->x < -OBSTACLE_SIZE) {
        obstacle->x = win_width;
        //obstacle->y = rand() % (LINES - OBSTACLE_SIZE); // 장애물 위치 랜덤화
        obstacle->shapeIndex = rand() % NUM_OBSTACLE_SHAPES; // 랜덤 모양 재설정
    }
}

void adjust_speed() {
    clock_t current_time = clock();
    double elapsed_time = (double)(current_time - last_speed_increase_time) / CLOCKS_PER_SEC * 1000; // ms 단위로 변환

    if (elapsed_time >= SPEED_INCREASE_INTERVAL) {
        obstacle_speed += OBSTACLE_SPEED_INCREMENT; // 장애물 속도 증가
        game_speed = (game_speed > GAME_SPEED_DECREASE) ? game_speed - GAME_SPEED_DECREASE : 0; // 게임 속도 감소
        last_speed_increase_time = current_time; // 마지막 속도 증가 시간을 현재 시간으로 재설정
    }
}

int check_collision(Dino *dino, Obstacle *obstacle) {
    // Simple bounding box collision detection
    const char **shape = obstacle_shapes[obstacle->shapeIndex];
    
    for (int i = 0; i < OBSTACLE_SIZE; i++) {
        for (int j = 0; j < OBSTACLE_SIZE; j++) {
            if (shape[i][j] != ' ') {
                // 장애물의 모양에서 공백이 아닌 부분만 충돌 체크
                if (dino->x < obstacle->x + OBSTACLE_SIZE &&
                    dino->x + DINO_SIZE > obstacle->x + j &&
                    dino->y < obstacle->y + OBSTACLE_SIZE &&
                    dino->y + DINO_SIZE > obstacle->y + i) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void display_game_over(WINDOW *win, int win_width, int win_height) {
    werase(win);
    mvwprintw(win, win_height / 2 - 1, win_width / 2 - 5, "GAME OVER");
    mvwprintw(win, win_height / 2 + 1, win_width / 2 - 12, "Press ENTER to Restart");
    mvwprintw(win, win_height / 2 + 3, win_width / 2 - 8, "Press Q to Exit");
    wrefresh(win);
}

int main() {
    Dino dino;
    Obstacle obstacle;
    int ch;
    int win_width, win_height;
    int inputBlocked = 0; // 입력 차단 상태
    int gameRunning = 1; // 게임 실행 여부

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    timeout(0); // non-blocking getch()
    srand(time(NULL)); // 랜덤 시드 설정

    // Get terminal size
    getmaxyx(stdscr, win_height, win_width);

    // Set game window size
    WINDOW *win = newwin(win_height, win_width, 0, 0);

    while (gameRunning) {
        initialize_game(&dino, &obstacle, win_width, win_height);

        clock_t startTime, endTime;
        double elapsedTime;
        double frameTime = 1.0 / FRAME_RATE; // 초당 프레임 수에 따라 각 프레임의 시간
        startTime = clock();

        while (1) {
            endTime = clock();
            elapsedTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;

            if (elapsedTime >= frameTime) {
                update_score(); // 점수 업데이트 
                adjust_speed();
                draw_game(win, &dino, &obstacle, win_width, win_height);

                if (!inputBlocked) {
                    ch = getch();
                    if (ch == 'q') {
                        gameRunning = 0;
                        break;
                    }

                    if (ch == ' ' && dino.y >= dino.initialY && !dino.isJumping) { // 점프할 수 있는 경우에만 점프
                        dino.isJumping = 1;
                        // 점프가 시작되면 입력 버퍼를 지운다
                        flushinp(); // ncurses 입력 버퍼를 비운다
                    }
                }

                update_dino(&dino, win_height);
                update_obstacle(&obstacle, win_width);

                if (check_collision(&dino, &obstacle)) {
                    display_game_over(win, win_width, win_height);
                    while (1) {
                        ch = getch();
                        if (ch == '\n') {
                            break; // 엔터키를 눌러서 게임 재시작
                        } else if (ch == 'q') {
                            gameRunning = 0;
                            break; // 'q' 키를 눌러서 게임 종료
                        }
                    }
                    break; // 게임 루프 종료 후 재시작
                }

                // 점프가 완료된 후 입력 차단 해제
                if (dino.jumpTimer == 0) {
                    inputBlocked = 0;
                }

                startTime = clock(); // 프레임 시간 재설정
            }
        }
    }

    endwin();
    return 0;
}
