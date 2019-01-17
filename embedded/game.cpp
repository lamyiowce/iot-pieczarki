#include "mbed.h"
#include <time.h>
#include <stdlib.h>

#define WIDTH 60
#define HEIGHT 20
#define LINES 35
#define MAX_DISTANCE 30
#define MIN_DISTANCE 5

Serial pc(USBTX, USBRX);

DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);

InterruptIn echo(PB_3);
DigitalOut trigger(PA_10);

Timer timer;

char field[HEIGHT][WIDTH + 1];

void init_field() {
    for (int i = 0; i < WIDTH; i++) {
        field[0][i] = '*';
        field[HEIGHT - 1][i] = '*';
    }
    for (int i = 0; i < HEIGHT; i++) {
        field[i][0] = '*';
        field[i][WIDTH - 1] = '*';
        field[i][WIDTH] = '\0';
    }
    for (int i = 1; i < HEIGHT - 1; i++)
        for (int j = 1; j < WIDTH - 1; j++)
            field[i][j] = ' ';

    field[HEIGHT / 2][1] = '>';
}


void print_field() {
    pc.printf("\033[2J");
    for (int i = 0; i < HEIGHT; i++)
        pc.printf("%s\r\n", field[i]);
}

bool died = false;
bool restart_request = false;
int survived = 0;

void restart_game() {
    restart_request = true;
}

bool timer_start = false;
bool timer_stop = false;

void echo_up() {
    timer_start = true;
}

void echo_down() {
    timer_stop = true;
}

int get_distance() {
    int time;
    trigger = 1;
    wait_us(10);
    trigger = 0;

    while(1) {
        if (timer_start) {
            timer.start();
            timer_start = false;
        } else if (timer_stop) {
            timer.stop();
            time = timer.read_us();
            timer.reset();
            timer_stop = false;
            break;
        }
    }
    return time * 17 / 1000;
}

void check_collisions() {
    for (int i = 1; i < HEIGHT - 1; i++) {
        if (field[i][1] != ' ' && field[i][2] != ' ') {
            died = true;
            pc.printf("\r\nKoniec gry...\r\nPrzezyles/as %d tickow\r\n", survived);
            break;
        }
    }
}

void change_position() {
    int distance = get_distance();
    if (distance > MAX_DISTANCE)
        distance = MAX_DISTANCE;
    if (distance < MIN_DISTANCE)
        distance = MIN_DISTANCE;

    int new_position = 1 + ((distance - MIN_DISTANCE) * (HEIGHT - 3) / (MAX_DISTANCE - MIN_DISTANCE));

    for (int i = 1; i < HEIGHT - 1; i++)
        field[i][1] = ' ';

    field[(HEIGHT - 1) - new_position][1] = '>';
}

bool enemy() {
    return rand() % 20 == 0;
}

bool enemy_2() {
    return rand() % 5 == 0;
}

void move_enemies() {
    for (int i = 2; i < WIDTH - 2; i++)
        for (int j = 1; j < HEIGHT - 1; j++)
            field[j][i] = field[j][i + 1];

    for (int i = 2; i < HEIGHT - 2; i++)
        field[i][WIDTH - 2] = enemy() ? '<' : ' ';

    field[1][WIDTH - 2] = enemy_2() ? '<' : ' ';
    field[HEIGHT - 2][WIDTH - 2] = enemy_2() ? '<' : ' ';
}

int main() {
    pc.baud(115200);
    pc.printf("Wlaczam gre...\r\n");
    wait(2.0);

    srand(time(NULL));

    trigger = 0;

    echo.rise(&echo_up);
    echo.fall(&echo_down);

    button.rise(&restart_game);

    init_field();
    print_field();
    int state = 0;
    while (1) {
        if (restart_request) {
            restart_request = false;
            died = false;
            pc.printf("\r\nRestartuje gre...\r\n");
            survived = 0;
            init_field();
            wait(2.0);
        }
        if (died) {
            continue;
        }
        print_field();
        check_collisions();
        survived++;
        wait(0.1);
        change_position();
        state++;
        if (state == 2) {
            state = 0;
            move_enemies();
        }
    }
}
