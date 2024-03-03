#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#define UNSTOPPABLE_DURATION 50 // �����޵�״̬������ʱ��Ϊ10
#define DURATION 54141          // ������Ծ�Ͷ���״̬�ĳ���ʱ��Ϊ54141
int LEN;                        // �����ͼ���ȱ���
int MAX_SPEED;                  // ������Ϸ������ٶȱ���

typedef enum
{
    standing,
    jumping,
    ducking,
    unstoppable
} Playerstate; // �������״̬��ö�����ͣ�վ������Ծ�����¡��޵�

typedef enum
{
    empty,
    Primogem,
    obstacle_U,
    obstacle_D,
    blocked,
    star
} Cell_type; // ��Ԫ�����ͣ��ա���ҡ��ϰ�U���ϰ�D��X���ǣ��޵У���

typedef enum
{
    RUNNING,
    PAUSED,
    GAME_OVER,
    STOP
} Gamestate; // ������Ϸ״̬��ö�����ͣ������С���ͣ����Ϸ����

typedef struct
{
    Playerstate state;
    int score;
    int speed;
    int location;
    int UNSTOPPABLE_time;
    Cell_type rodes[20][20];
    Gamestate gamestate;
    int stateChanged; // ״̬�ı��־(�˴�ʹ����chatgpt�������Ƶ��������)
    int jumpTimer;
    int duckTimer;
} Game_State;
void Initialize_game(Game_State *state);         // ������ʼ����Ϸ�ĺ���
void born_obstacles_prop(Game_State *state);     // ���������ϰ���͵��ߵĺ���
void Input(Game_State *state, char input);       // ������������ĺ���
void check_collision(Game_State *state);         // ���������ײ�ĺ���
void update_Game(Game_State *state, char input); // ����������Ϸ״̬�ĺ���
void update_playerstate(Game_State *state);      // �����������״̬�ĺ���
void update_score_speed(Game_State *state);      // �����ƶ��ϰ���ĺ���
void update_UNSTOPPABLE(Game_State *state);      // ���������޵�״̬�ĺ���
void map(Game_State *state);                     // ������Ⱦ��Ϸ����ĺ���
void game_run();                                 // ������Ϸѭ���ĺ���
void obstacle(Game_State *state);
int main()
{
    printf("Please select your map length from 10-20\n");
    scanf("%d", &LEN);
    while (LEN < 10 || LEN > 20)
    {
        if (LEN < 10)
        {
            printf("The map you selected is too short, please re-enter it!\n");
            scanf("%d", &LEN);
        }
        else if (LEN > 20)
        {
            printf("The map you selected is too long, please re-enter it!\n");
            scanf("%d", &LEN);
        }
    }
    printf("Please choose the difficulty that suits you according to your level from 1-5\n");
    scanf("%d", &MAX_SPEED);
    while (MAX_SPEED < 1 || MAX_SPEED > 5)
    {
        if (MAX_SPEED < 1)
        {
            printf("You have overestimated your IQ, please choose again!\n");
            scanf("%d", &MAX_SPEED);
        }
        else if (MAX_SPEED > 5)
        {
            printf("You are a confident person, but not an honest person, please choose again!\n");
            scanf("%d", &MAX_SPEED);
        }
    }

    printf("The maiden is praying\n");
    Sleep(1145); // ��װ���أ���
    game_run();  // ������Ϸ
    return 0;
}
void Initialize_game(Game_State *state) // ��ʼ��
{
    state->score = 0;
    state->speed = 1;
    state->location = 1;
    state->state = standing;     // ��ʼ���״̬ΪӲֱ��̬
    state->UNSTOPPABLE_time = 0;
    state->jumpTimer = 0;
    state->duckTimer = 0;
    state->gamestate = RUNNING;  // ������Ϸ״̬Ϊ����
    state->stateChanged = 1;     // ����״̬�ı��־Ϊ1����ʾ״̬�Ѹı䣨ʹ����chatgpt����
    int i = 0;
    while (i < 3)
    {
        int j = 0;
        while (j < LEN)
        {
            state->rodes[i][j] = empty;
            j += 1;
        }
        i += 1;
    }
    srand(time(NULL));
}

void born_obstacles_prop(Game_State *state)
{
    // �е�����
    int rode = rand() % 3; // ���ѡ��һ��·��
    // Լ10%�ĸ��������·�������޵���
    if (rand() % 10 < 1)
    {
        state->rodes[rode][LEN - 1] = star; // ��·������λ�������޵���
    }
    if (rand() % 10 < 6) // �ٷ�֮��ʮ��������D
    {
        if (rand() % 10 < 5)
        {
            state->rodes[rode][LEN - 1] = obstacle_D;
        }
        else
        {
            state->rodes[rode][LEN - 1] = obstacle_U;
        }
    }
    if (rand() % 10 < 1) // �ٷ�֮ʮ�ĸ�������ԭʯ
    {
        int rode = rand() % 3;
        if (state->rodes[rode][LEN - 1] == empty)
        {
            state->rodes[rode][LEN - 1] = Primogem;
        }
    }

    // Լ�ٷ�֮10ʮ�ĸ���������һ��·������X�ϰ�
    if (rand() % 10 < 1)
    {
        int rode = rand() % 3; // ���ѡ��һ��·��
        state->rodes[rode][LEN - 1] = blocked;
    }
}
void Input(Game_State *state, char input) // ��������
{
    if (state->gamestate == RUNNING)
    {
        if (input == 'w' && state->state != unstoppable)
        {
            state->state = jumping;
            state->jumpTimer = DURATION;
        }
        else if (input == 's' && state->state != unstoppable)
        {
            state->state = ducking;
            state->duckTimer = DURATION;
        }
        else if (input == 'a')
        {
            if (state->location > 0)
            {
                state->location--;
            }
        }
        else if (input == 'd')
        {
            if (state->location < 2)
            {
                state->location++;
            }
        }
    }
    if (input == 'p')
    {
        state->gamestate = (state->gamestate == RUNNING) ? PAUSED : RUNNING;
    }
    if (state->gamestate == GAME_OVER && input == 'r')
    { // ��Ϸ����״̬��������Ϸ
        Initialize_game(state);
    }
}
void check_collision(Game_State *state)
{
    /*if (state->state == unstoppable)
    {
        return; // �����޵�״̬
    }*/
    if (state->rodes[state->location][0] == blocked && state->state != unstoppable)
    {
        state->gamestate = GAME_OVER;
    }
    Cell_type cell = state->rodes[state->location][0]; // ��ǰλ�õĵ�Ԫ�����ͼ��
    if (cell == star)
    {
        state->state = unstoppable;                     // ���޵У���
        state->UNSTOPPABLE_time = UNSTOPPABLE_DURATION; // �����޵�״̬�ĳ���ʱ��
    }
    else if ((cell == obstacle_U && state->state != jumping && state->state != unstoppable) ||
             (cell == obstacle_D && state->state != ducking && state->state != unstoppable))
    {
        state->gamestate = GAME_OVER; // �벻ͬ���͵��ϰ�����ײ
    }
    // ����ԭʯ���߼�
    else if (cell == Primogem)
    {
        state->score += 10;                       // ʰȡ��ң�����10��
        state->rodes[state->location][0] = empty; // ������
    }
}

void update_Game(Game_State *state, char input)
{
    if (state->gamestate == RUNNING)
    {
        Input(state, input); // �����Ϸ�������У������������
    }
    else if (input == 'p')
    {
        state->gamestate = (state->gamestate == RUNNING) ? PAUSED : RUNNING;
    }
    else if (state->gamestate == GAME_OVER && input == 'r')
    {
        Initialize_game(state);
    }
    else if (input == 'x')
    {
        state->gamestate = STOP;
    }
}
void update_playerstate(Game_State *state)
{
    if (state->jumpTimer > 0)
    {
        state->jumpTimer--; // ��Ծʱ�����
        if (state->jumpTimer == 0)
        {
            state->state = standing; // �������վ��
        }
    }

    if (state->duckTimer > 0)
    {
        state->duckTimer--;
        if (state->duckTimer == 0)
        {
            state->state = standing; // �������վ��
        }
    }
}
void obstacle(Game_State *state)
{
    int i = 0;
    while (i < 3)
    {
        int j = 0;
        while (j < LEN - 1)
        {
            state->rodes[i][j] = state->rodes[i][j + 1]; // ��ÿһ�еĶ�����ǰ�ƶ�һ��
            j += 1;
        }
        state->rodes[i][LEN - 1] = empty; // �����һ������Ϊ�գ���ʾ�ϰ����Ѿ��ƶ�����Ļ
        i += 1;
    }
}

void update_UNSTOPPABLE(Game_State *state)
{
    if (state->UNSTOPPABLE_time > 0)
    {
        state->UNSTOPPABLE_time--;
        if (state->UNSTOPPABLE_time == 0)
        {
            state->state = standing; // ���״̬��Ϊվ��
        }
    }
}

void update_score_speed(Game_State *state)
{
    state->score += 1;
    if (state->score % 10 == 0 && state->speed < MAX_SPEED)
    {
        state->speed++; // �÷�Ϊ10�ı��������ٶ�δ�ﵽ����ٶȣ������ٶ�
    }
}

void map(Game_State *state)
{
    if (state->stateChanged == 0)
    {
        return; // �����Ϸ״̬û�иı䣬��������Ⱦ��Ϸ����
    }
    system("cls"); // ����
    int i = 0;
    while (i < LEN)
    {
        int j = 0;
        while (j < 3)
        {
            if (state->location == j && i == 0)
            {
                printf("| 0 "); // ��ӡ��ҵ�λ��
            }
            else
            {
                switch (state->rodes[j][i])
                {
                case empty:
                    printf("|   "); // ���ϰ�
                    break;
                case Primogem:
                    printf("| $ "); // ԭʯ
                    break;
                case obstacle_U:
                    printf("| U "); // ��Ծ�ϰ�
                    break;
                case obstacle_D:
                    printf("| D "); // �׷��ϰ�
                    break;
                case blocked:
                    printf("| x "); // �޷�ͨ�����ϰ�
                    break;
                case star:
                    printf("| * "); // �޵�
                    break;
                }
            }
            j += 1;
        }
        printf("|\n");
        i += 1;
    }
    printf("Score: %d, Speed: %d, State: ", state->score, state->speed);
    switch (state->state)
    {
    case standing:
        printf("Standing\n"); // վ��
        break;
    case jumping:
        printf("Jumping\n"); // ��Ծ
        break;
    case ducking:
        printf("Ducking\n"); // ����
        break;
    case unstoppable:
        printf("UNSTOPPABLE!!!\n");
        printf("The UNSTOPPABLE time left : %d\n", state->UNSTOPPABLE_time); // �޵�
        break;
    }

    if (state->gamestate == PAUSED)
    {
        printf("\nGame Paused.\nPress 'p' again to continue or Press 'x' to end the game\n"); // ��Ϸ��ͣ
    }
    else if (state->gamestate == GAME_OVER)
    {
        printf("\nGame Over.\nPress 'r' to restart or Press 'x' to end the game\n"); // ��Ϸ����
    }
    state->stateChanged = 0; // ����״̬�ı��־
}
void game_run()
{
    Game_State state;
    Initialize_game(&state);    // ��ʼ����Ϸ״̬
    clock_t lastTime = clock(); // ��¼��һ�θ���ʱ��

    while (1) // ��Ϸ����ѭ��
    {
        if (_kbhit()) // ����Ƿ��м�������
        {
            char input = _getch();      // ��ȡ�����ַ�
            update_Game(&state, input); // �����������
            state.stateChanged = 1;     // �����Ϸ״̬�Ѹı�
        }

        if (state.gamestate == RUNNING) // �����Ϸ���ڽ���
        {
            clock_t currentTime = clock();                                        // ��ȡ��ǰʱ��
            double elapsed = ((double)(currentTime - lastTime)) / CLOCKS_PER_SEC; // ����ʱ���
            if (elapsed >= (1.0 / state.speed))                                   // ����Ƿ�ﵽ����Ƶ��
            {
                lastTime = currentTime;      // �������һ�ε�ʱ���¼
                born_obstacles_prop(&state); // �����ϰ���͵���
                check_collision(&state);     // �����ײ
                obstacle(&state);            // �ƶ��ϰ���
                update_UNSTOPPABLE(&state);  // �����޵�״̬
                update_score_speed(&state);  // ���·������ٶ�
                state.stateChanged = 1;      // �����Ϸ״̬�Ѹı�
            }

            update_playerstate(&state); // �������״̬
        }
        if (state.gamestate == STOP)
        {
            printf("The game is closing\n");
            Sleep(1145);
            return;
        }

        map(&state); // ��Ⱦ��Ϸ����
    }
}
