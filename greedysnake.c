#include <stdio.h>
#include <termios.h>
#include <unistd.h>//sleep函數使用
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define STAGE_WIDTH 20
#define STAGE_HEIGHT 10

void render(int (*snake_list)[2], int snake_length, int rand_x, int rand_y)
{
    system("clear");

    int list[STAGE_WIDTH][STAGE_HEIGHT] = {0};
    for (int i = 0; i < snake_length; i++)
    {
        list[snake_list[i][0]][snake_list[i][1]] = 1;
    }
    for (int i = 0; i < STAGE_WIDTH + 2; i++)
    {
        printf("*");
    }
    printf("\n");
    while (1)
    {
        int flag = 0;
        for (int i = 0; i < snake_length; i++)
        {
            if (rand_x == snake_list[i][0] && rand_y == snake_list[i][1])
            {
                rand_x = rand() % STAGE_WIDTH;
                rand_y = rand() % STAGE_HEIGHT;
                flag = 1;
            }
        }
        if (!flag)
            break;
    }

    for (int y = 0; y < STAGE_HEIGHT; y++)
    {
        printf("*");
        for (int x = 0; x < STAGE_WIDTH; x++)
        {
            if (list[x][y] == 1 || (rand_x == x && rand_y == y))
            {
                printf("*");
            }
            else
            {
                printf(" ");
            }
        }
        printf("*\n");
    }
    for (int i = 0; i < STAGE_WIDTH + 2; i++)
    {
        printf("*");
    }
    printf("\n");
}

void init_termios()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(unsigned long)(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void snake_move(int key, int (*snake_list)[2], int snake_length)
{
    for (int i = snake_length; i > 0; i--)
    {
        snake_list[i][0] = snake_list[i - 1][0];
        snake_list[i][1] = snake_list[i - 1][1];
    }
    switch (key)
    {
    case 'w':
        snake_list[0][1]--;
        break;
    case 's':
        snake_list[0][1]++;
        break;
    case 'a':
        snake_list[0][0]--;
        break;
    case 'd':
        snake_list[0][0]++;
        break;
    }
}

int is_reverse(int key, int last_key)
{
    if ((key == 'a' && last_key == 'd') || (key == 'd' && last_key == 'a') || (key == 'w' && last_key == 's') || (key == 's' && last_key == 'w'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int is_collision(int (*snake_list)[2], int snake_length)
{
    int list[STAGE_WIDTH][STAGE_HEIGHT] = {0};
    for (int i = 0; i < snake_length; i++)
    {
        if (
            snake_list[i][0] < 0 || snake_list[i][0] >= STAGE_WIDTH || snake_list[i][1] < 0 || snake_list[i][1] >= STAGE_HEIGHT || list[snake_list[i][0]][snake_list[i][1]])
        {
            return 1;
        }
        list[snake_list[i][0]][snake_list[i][1]] = 1;
    }
    return 0;
}

void over()
{
    system("clear");
    printf("**************\n* Game over! *\n**************\n");
}

void *move_event_callback(void *arg)
{
    int *last_key_ptr = (int *)arg;
    while (1)
    {
        int key = getchar();
        if (!is_reverse(key, *last_key_ptr))
        {
            *last_key_ptr = key;
        }
    }
    return NULL;
}

void body_lengthing(int (*snake_list)[2], int *snake_length, int rand_x, int rand_y)
{

    if (snake_list[(*snake_length) - 1][0] - snake_list[(*snake_length) - 2][0] == 0)
    { // 左右
        if (snake_list[(*snake_length) - 2][1] - snake_list[(*snake_length) - 1][1] < 0)
        { // 向左
            snake_list[(*snake_length)][0] = snake_list[(*snake_length) - 1][0];
            snake_list[(*snake_length)][1] = snake_list[(*snake_length) - 1][1] + 1;
        }
        else if (snake_list[(*snake_length) - 2][1] - snake_list[(*snake_length) - 1][1] > 0)
        { // 向右
            snake_list[(*snake_length)][0] = snake_list[(*snake_length) - 1][0];
            snake_list[(*snake_length)][1] = snake_list[(*snake_length) - 1][1] - 1;
        }
    }
    if (snake_list[(*snake_length) - 1][1] - snake_list[(*snake_length) - 2][1] == 0)
    { // 上下
        if (snake_list[(*snake_length) - 2][0] - snake_list[(*snake_length) - 1][0] < 0)
        { // 向上
            snake_list[(*snake_length)][0] = snake_list[(*snake_length) - 1][0] + 1;
            snake_list[(*snake_length)][1] = snake_list[(*snake_length) - 1][1];
        }
        else if (snake_list[(*snake_length) - 2][0] - snake_list[(*snake_length) - 1][0] > 0)
        { // 向下
            snake_list[(*snake_length)][0] = snake_list[(*snake_length) - 1][0] - 1;
            snake_list[(*snake_length)][1] = snake_list[(*snake_length) - 1][1];
        }
    }
    (*snake_length)++;
}

int main()
{
    int snake[100][2] = {{4, 4}, {4, 5}, {4, 6}, {4, 7}, {4, 8}, {4, 9}};
    int length = 6;
    int last_key = 'w';

    srand(time(NULL));
    int rand_x = rand() % STAGE_WIDTH;
    int rand_y = rand() % STAGE_HEIGHT;

    render(snake, length, rand_x, rand_y);

    init_termios();

    pthread_t move_event_thread;
    pthread_create(&move_event_thread, NULL, move_event_callback, (void *)&last_key);

    while (1)
    {
        usleep(500000);
        snake_move(last_key, snake, length);
        if (!is_collision(snake, length))
        {
            if (snake[0][0] == rand_x && snake[0][1] == rand_y)
            {
                rand_x = rand() % STAGE_WIDTH;
                rand_y = rand() % STAGE_HEIGHT;
                body_lengthing(snake, &length, rand_x, rand_y);
            }
            render(snake, length, rand_x, rand_y);
        }
        else
        {
            over();
        }
    }
}