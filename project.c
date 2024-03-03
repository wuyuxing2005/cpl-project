#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#define UNSTOPPABLE_DURATION 50 // 定义无敌状态持续的时间为10
#define DURATION 54141          // 定义跳跃和蹲下状态的持续时间为54141
int LEN;                        // 定义地图长度变量
int MAX_SPEED;                  // 定义游戏的最大速度变量

typedef enum
{
    standing,
    jumping,
    ducking,
    unstoppable
} Playerstate; // 定义玩家状态的枚举类型：站立、跳跃、蹲下、无敌

typedef enum
{
    empty,
    Primogem,
    obstacle_U,
    obstacle_D,
    blocked,
    star
} Cell_type; // 单元格类型：空、金币、障碍U、障碍D、X、星（无敌）；

typedef enum
{
    RUNNING,
    PAUSED,
    GAME_OVER,
    STOP
} Gamestate; // 定义游戏状态的枚举类型：运行中、暂停、游戏结束

typedef struct
{
    Playerstate state;
    int score;
    int speed;
    int location;
    int UNSTOPPABLE_time;
    Cell_type rodes[20][20];
    Gamestate gamestate;
    int stateChanged; // 状态改变标志(此处使用了chatgpt解决画面频闪的问题)
    int jumpTimer;
    int duckTimer;
} Game_State;
void Initialize_game(Game_State *state);         // 声明初始化游戏的函数
void born_obstacles_prop(Game_State *state);     // 声明生成障碍物和道具的函数
void Input(Game_State *state, char input);       // 声明处理输入的函数
void check_collision(Game_State *state);         // 声明检查碰撞的函数
void update_Game(Game_State *state, char input); // 声明更新游戏状态的函数
void update_playerstate(Game_State *state);      // 声明更新玩家状态的函数
void update_score_speed(Game_State *state);      // 声明移动障碍物的函数
void update_UNSTOPPABLE(Game_State *state);      // 声明更新无敌状态的函数
void map(Game_State *state);                     // 声明渲染游戏界面的函数
void game_run();                                 // 声明游戏循环的函数
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
    Sleep(1145); // 假装加载（）
    game_run();  // 进入游戏
    return 0;
}
void Initialize_game(Game_State *state) // 初始化
{
    state->score = 0;
    state->speed = 1;
    state->location = 1;
    state->state = standing;     // 初始玩家状态为硬直姿态
    state->UNSTOPPABLE_time = 0;
    state->jumpTimer = 0;
    state->duckTimer = 0;
    state->gamestate = RUNNING;  // 设置游戏状态为运行
    state->stateChanged = 1;     // 设置状态改变标志为1，表示状态已改变（使用了chatgpt）；
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
    // 有点问题
    int rode = rand() % 3; // 随机选择一个路面
    // 约10%的概率在这个路面生成无敌星
    if (rand() % 10 < 1)
    {
        state->rodes[rode][LEN - 1] = star; // 在路面的最后位置生成无敌星
    }
    if (rand() % 10 < 6) // 百分之三十概率生成D
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
    if (rand() % 10 < 1) // 百分之十的概率生成原石
    {
        int rode = rand() % 3;
        if (state->rodes[rode][LEN - 1] == empty)
        {
            state->rodes[rode][LEN - 1] = Primogem;
        }
    }

    // 约百分之10十的概率在任意一个路面生成X障碍
    if (rand() % 10 < 1)
    {
        int rode = rand() % 3; // 随机选择一个路面
        state->rodes[rode][LEN - 1] = blocked;
    }
}
void Input(Game_State *state, char input) // 处理输入
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
    { // 游戏结束状态，重启游戏
        Initialize_game(state);
    }
}
void check_collision(Game_State *state)
{
    /*if (state->state == unstoppable)
    {
        return; // 处于无敌状态
    }*/
    if (state->rodes[state->location][0] == blocked && state->state != unstoppable)
    {
        state->gamestate = GAME_OVER;
    }
    Cell_type cell = state->rodes[state->location][0]; // 当前位置的单元格类型检测
    if (cell == star)
    {
        state->state = unstoppable;                     // （无敌）；
        state->UNSTOPPABLE_time = UNSTOPPABLE_DURATION; // 设置无敌状态的持续时间
    }
    else if ((cell == obstacle_U && state->state != jumping && state->state != unstoppable) ||
             (cell == obstacle_D && state->state != ducking && state->state != unstoppable))
    {
        state->gamestate = GAME_OVER; // 与不同类型的障碍物碰撞
    }
    // 处理原石的逻辑
    else if (cell == Primogem)
    {
        state->score += 10;                       // 拾取金币，增加10分
        state->rodes[state->location][0] = empty; // 清除金币
    }
}

void update_Game(Game_State *state, char input)
{
    if (state->gamestate == RUNNING)
    {
        Input(state, input); // 如果游戏正在运行，处理玩家输入
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
        state->jumpTimer--; // 跳跃时间减少
        if (state->jumpTimer == 0)
        {
            state->state = standing; // 玩家重新站立
        }
    }

    if (state->duckTimer > 0)
    {
        state->duckTimer--;
        if (state->duckTimer == 0)
        {
            state->state = standing; // 玩家重新站立
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
            state->rodes[i][j] = state->rodes[i][j + 1]; // 将每一行的东西向前移动一格
            j += 1;
        }
        state->rodes[i][LEN - 1] = empty; // 将最后一格设置为空，表示障碍物已经移动出屏幕
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
            state->state = standing; // 玩家状态变为站立
        }
    }
}

void update_score_speed(Game_State *state)
{
    state->score += 1;
    if (state->score % 10 == 0 && state->speed < MAX_SPEED)
    {
        state->speed++; // 得分为10的倍数，且速度未达到最大速度，增加速度
    }
}

void map(Game_State *state)
{
    if (state->stateChanged == 0)
    {
        return; // 如果游戏状态没有改变，则不重新渲染游戏画面
    }
    system("cls"); // 清屏
    int i = 0;
    while (i < LEN)
    {
        int j = 0;
        while (j < 3)
        {
            if (state->location == j && i == 0)
            {
                printf("| 0 "); // 打印玩家的位置
            }
            else
            {
                switch (state->rodes[j][i])
                {
                case empty:
                    printf("|   "); // 无障碍
                    break;
                case Primogem:
                    printf("| $ "); // 原石
                    break;
                case obstacle_U:
                    printf("| U "); // 跳跃障碍
                    break;
                case obstacle_D:
                    printf("| D "); // 蹲伏障碍
                    break;
                case blocked:
                    printf("| x "); // 无法通过的障碍
                    break;
                case star:
                    printf("| * "); // 无敌
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
        printf("Standing\n"); // 站立
        break;
    case jumping:
        printf("Jumping\n"); // 跳跃
        break;
    case ducking:
        printf("Ducking\n"); // 蹲下
        break;
    case unstoppable:
        printf("UNSTOPPABLE!!!\n");
        printf("The UNSTOPPABLE time left : %d\n", state->UNSTOPPABLE_time); // 无敌
        break;
    }

    if (state->gamestate == PAUSED)
    {
        printf("\nGame Paused.\nPress 'p' again to continue or Press 'x' to end the game\n"); // 游戏暂停
    }
    else if (state->gamestate == GAME_OVER)
    {
        printf("\nGame Over.\nPress 'r' to restart or Press 'x' to end the game\n"); // 游戏结束
    }
    state->stateChanged = 0; // 重置状态改变标志
}
void game_run()
{
    Game_State state;
    Initialize_game(&state);    // 初始化游戏状态
    clock_t lastTime = clock(); // 记录上一次更新时间

    while (1) // 游戏的主循环
    {
        if (_kbhit()) // 检查是否有键盘输入
        {
            char input = _getch();      // 获取输入字符
            update_Game(&state, input); // 处理玩家输入
            state.stateChanged = 1;     // 标记游戏状态已改变
        }

        if (state.gamestate == RUNNING) // 如果游戏正在进行
        {
            clock_t currentTime = clock();                                        // 获取当前时间
            double elapsed = ((double)(currentTime - lastTime)) / CLOCKS_PER_SEC; // 计算时间差
            if (elapsed >= (1.0 / state.speed))                                   // 检查是否达到更新频率
            {
                lastTime = currentTime;      // 更新最后一次的时间记录
                born_obstacles_prop(&state); // 生成障碍物和道具
                check_collision(&state);     // 检测碰撞
                obstacle(&state);            // 移动障碍物
                update_UNSTOPPABLE(&state);  // 更新无敌状态
                update_score_speed(&state);  // 更新分数和速度
                state.stateChanged = 1;      // 标记游戏状态已改变
            }

            update_playerstate(&state); // 更新玩家状态
        }
        if (state.gamestate == STOP)
        {
            printf("The game is closing\n");
            Sleep(1145);
            return;
        }

        map(&state); // 渲染游戏画面
    }
}
