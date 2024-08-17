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
#define BOSS_WIDTH 50
#define BOSS_HEIGHT 22
#define PHASE_CHANGE_TIME 5
#define PHASE2_OBSTACLE_SIZE 4
#define PHASE2_OBSTACLE_SPEED 1

const char boss_shape[BOSS_HEIGHT][BOSS_WIDTH] = {
    "                 ..-+++--.----...                 ",
    "            ..-++-..   ........--+#+-.            ",
    "           .#+..        .......... .-#-.          ",
    "        ..###++#+-..  .-#-......-##.. -#..        ",
    "      .-#+..-+. ..+#-.++    .-+.  .++. .+.        ",
    "      -#+.  .+.    .+-#.      +    .+- .-+.       ",
    "     .++           .-##.           .-+. .+-.      ",
    "     .#+           ..##.            -+.---#.      ",
    "     .##.          .---#.          .-+ ++-#.      ",
    "     .+.++..    ...++. .++-... ....++. .-.#-      ",
    "     .++..++#####+-.    ...-++#+++-.-. .#-#-      ",
    "      .#..+. .-    ..+..-.      .-..-.   .#.      ",
    "      .++.-..-.  ..+######+-..   .. -.   -#.      ",
    "       .#-..... .+#+.......###.     .. .-#.       ",
    "        .+-.    -#-  .....  .+#-.     .-+.        ",
    "        ..-#-...+#. .+-.--.  .##.  .-++..         ",
    "            .-####+...-##-....#####--+.           ",
    "                ..+##--...--+###+.  .+-+.         ",
    "                  ..-+######+#-.+.. .+-           ",
    "                               .--..-#..          ",
    "                                .+--..#.          ",
    "                                .-.   ..          "
};

// 화살표 모양
const char attack_shape[4][4] = {
    "----",
    "<===",
    "<===",
    "----"
};

const char player_attack_shape[2][2] = {
    "**"
    "**"
};

typedef struct {
    int x, y; // 보스의 위치
    int isAttacking;
    int attackTimer; // 공격 타이머
} Boss;

typedef struct {
    int x, y;
    int isActive;
} BossAttack;

typedef struct {
    int x, y;
    int isActive;
} PlayerAttack;

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
    int phase;
} Dino;

typedef struct {
    int x, y;
    int dx, dy;
    int type; // 장애물 유형
    int size;
    int speed;
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

void initialize_game(Dino *dino, Obstacle *obstacle, Boss *boss, BossAttack *bossAttack, PlayerAttack *playerAttack, int win_width, int win_height) {
    dino->x = 5;
    dino->y = win_height - DINO_SIZE;
    dino->initialY = dino->y;
    dino->isJumping = 0;
    dino->jumpHeight = 0;
    dino->jumpTimer = 0;
    dino->phase = 1;

    obstacle->x = win_width - OBSTACLE_SIZE;
    obstacle->y = win_height - OBSTACLE_SIZE;
    obstacle->shapeIndex = rand() % NUM_OBSTACLE_SHAPES;

    score = 0;
    score_start_time = clock();

    obstacle_speed = INITIAL_OBSTACLE_SPEED;
    game_speed = INITIAL_GAME_SPEED;
    last_speed_increase_time = clock();

    boss->x = win_width - BOSS_WIDTH;
    boss->y = win_height - BOSS_HEIGHT;
    boss->isAttacking = 0;
    boss->attackTimer = 0;

    bossAttack->isActive = 0; // 초기화 시 보스 공격 비활성화
    bossAttack->x = boss->x;  // 보스 위치와 같은 위치에 공격을 시작하도록 설정
    bossAttack->y = win_height - 5; // 화면 하단에 위치 설정

    playerAttack->isActive = 0; // 플레이어 공격 초기화
}

void draw_game(WINDOW *win, Dino *dino, Obstacle *obstacle, Boss *boss, PlayerAttack *playerAttack, BossAttack *bossAttack, int win_width, int win_height) {
    werase(win);

    // Draw Dino
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 8; j++) {
            if (dino->y + i < win_height && dino->x + j < win_width) {
                mvwaddch(win, dino->y + i, dino->x + j, gamza[i][j]);
            }
        }
    }

    // Draw Obstacle
    if (dino->phase == 1) {
        for (int i = 0; i < OBSTACLE_SIZE; i++) {
            for (int j = 0; j < OBSTACLE_SIZE; j++) {
                if (obstacle->y + i < win_height && obstacle->x + j < win_width)
                    mvwaddch(win, obstacle->y + i, obstacle->x + j, obstacle_shapes[obstacle->shapeIndex][i][j]);
            }
        }
        mvwprintw(win, 0, 0, "Score: %d", score);
        mvwprintw(win, 1, win_width - 20, "High Score: %d", high_score);
    }

    // Draw Boss (if in Phase 2)
    if (dino->phase == 2) {
        int boss_y = win_height - BOSS_HEIGHT;
        if (boss_y < 0) boss_y = 0;  

        for (int i = 0; i < BOSS_HEIGHT; i++) {
            for (int j = 0; j < BOSS_WIDTH; j++) {
                if (boss_y + i < win_height && boss->x + j < win_width && boss_shape[i][j] != ' ') {
                    mvwaddch(win, boss_y + i, boss->x + j, boss_shape[i][j]);
                }
            }
        }
    }

    // Draw Boss Attack (if in Phase 2 and boss is attacking)
    if (dino->phase == 2 && boss->isAttacking) {
        if (bossAttack->isActive) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (bossAttack->y + i < win_height && bossAttack->x + j < win_width) {
                        mvwaddch(win, bossAttack->y + i, bossAttack->x + j, attack_shape[i][j]);
                    }
                }
            }
        }
    }

    // Draw Player Attack
    if (playerAttack->isActive) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                if (playerAttack->y + i < win_height && playerAttack->x + j < win_width) {
                    mvwaddch(win, playerAttack->y + i, playerAttack->x + j, player_attack_shape[i][j]);
                }
            }
        }
    }

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

void update_obstacle(Obstacle *obstacle, int win_width, int phase) {
    if (phase == 1) {
        obstacle->x -= (int)obstacle_speed;
        if (obstacle->x < -OBSTACLE_SIZE) {
            obstacle->x = win_width;
            //obstacle->shapeIndex = rand() % NUM_OBSTACLE_SHAPES; // 랜덤 모양 재설정
        }
    }
}

void update_boss(Boss *boss, Dino *dino, BossAttack *bossAttack, int win_height) {
    if (dino->phase == 2) {
        if (boss->attackTimer == 0) {
            boss->isAttacking = 1;
            boss->attackTimer = 10; // 공격 간격 (임의 설정)
            bossAttack->isActive = 1; // 보스 공격 활성화
            bossAttack->x = boss->x + BOSS_WIDTH / 2 - 2; // 보스 위치에서 시작
            bossAttack->y = win_height - 10; // 화면 하단에서 시작
        }

        if (boss->attackTimer > 0) {
            boss->attackTimer--;
        } else {
            boss->isAttacking = 0;
            boss->attackTimer = 10;
        }
    }
}

void update_player_attack(PlayerAttack *playerAttack) {
    if (playerAttack->isActive) {
        playerAttack->x += 1; // 공격이 오른쪽으로 이동
        if (playerAttack->x > COLS) {
            playerAttack->isActive = 0; // 화면 밖으로 나가면 비활성화
        }
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

int check_attack_collision(PlayerAttack *playerAttack, Boss *boss) {
    if (playerAttack->isActive) {
        // Assuming boss's width and height are known, e.g., BOSS_WIDTH and BOSS_HEIGHT
        if (playerAttack->x < boss->x + BOSS_WIDTH &&
            playerAttack->x + 4 > boss->x &&
            playerAttack->y < boss->y + BOSS_HEIGHT &&
            playerAttack->y + 4 > boss->y) {
            return 1;
        }
    }
    return 0;
}

int check_boss_attack_collision(BossAttack *bossAttack, Dino *dino) {
    if (bossAttack->isActive) {
        if (bossAttack->x < dino->x + DINO_SIZE &&
            bossAttack->x + 4 > dino->x &&
            bossAttack->y < dino->y + DINO_SIZE &&
            bossAttack->y + 4 > dino->y) {
            return 1;
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

void game_loop(WINDOW *win, Dino *dino, Obstacle *obstacle, Boss *boss, PlayerAttack *playerAttack, BossAttack *bossAttack, int win_width, int win_height) {
    int ch;
    int inputBlocked = 0;
    clock_t startTime, endTime;
    double elapsedTime;
    double frameTime = 1.0 / FRAME_RATE;
    double phaseChangeInterval = PHASE_CHANGE_TIME;
    double phaseStartTime = 0;
    int phase = 1;

    initialize_game(dino, obstacle, boss, bossAttack, playerAttack, win_width, win_height);

    startTime = clock();
    phaseStartTime = startTime;

    while (1) {
        endTime = clock();
        elapsedTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
        double phaseElapsedTime = (double)(endTime - phaseStartTime) / CLOCKS_PER_SEC;

        if (phaseElapsedTime >= PHASE_CHANGE_TIME && phase == 1) {
            dino->phase = 2;
            obstacle->size = PHASE2_OBSTACLE_SIZE;
            obstacle->speed = PHASE2_OBSTACLE_SPEED;
            phase = 2;
            phaseStartTime = endTime;
        }

        if (elapsedTime >= frameTime) {
            update_score();
            adjust_speed();
            draw_game(win, dino, obstacle, boss, playerAttack, bossAttack, win_width, win_height);
            update_boss(boss, dino, bossAttack, win_height);
            update_player_attack(playerAttack);

            if (!inputBlocked) {
                ch = getch();
                if (ch == 'q') {
                    return;
                }

                if (ch == ' ' && dino->y >= dino->initialY && !dino->isJumping) {
                    dino->isJumping = 1;
                    flushinp();
                }

                if (ch == 'a' && !playerAttack->isActive) { // 플레이어 공격
                    playerAttack->isActive = 1;
                    playerAttack->x = dino->x + DINO_SIZE;
                    playerAttack->y = dino->y + DINO_SIZE / 2 - 2;
                }
            }

            update_dino(dino, win_height);
            update_obstacle(obstacle, win_width, dino->phase);
            update_boss(boss, dino, bossAttack, win_height);

            if (check_collision(dino, obstacle)) {
                display_game_over(win, win_width, win_height);
                while (1) {
                    ch = getch();
                    if (ch == '\n') {
                        return;
                    } else if (ch == 'q') {
                        return;
                    }
                }
            }

            if (check_attack_collision(playerAttack, boss)) {
                display_game_over(win, win_width, win_height);
                while (1) {
                    ch = getch();
                    if (ch == '\n') {
                        return;
                    } else if (ch == 'q') {
                        return;
                    }
                }
            }

            if (check_boss_attack_collision(bossAttack, dino)) {
                display_game_over(win, win_width, win_height);
                while (1) {
                    ch = getch();
                    if (ch == '\n') {
                        return;
                    } else if (ch == 'q') {
                        return;
                    }
                }
            }

            if (dino->jumpTimer == 0) {
                inputBlocked = 0;
            }

            startTime = clock();
        }
    }
}

int main() {
    Dino dino;
    Obstacle obstacle;
    Boss boss;
    BossAttack bossAttack;
    PlayerAttack playerAttack;
    int win_width, win_height;

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

    while (1) {
        initialize_game(&dino, &obstacle, &boss, &bossAttack, &playerAttack, win_width, win_height);
        game_loop(win, &dino, &obstacle, &boss, &playerAttack, &bossAttack, win_width, win_height);
        if (getch() == 'q') break; // 게임 종료 여부 확인
    }

    endwin();
    return 0;
}
