#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>
#include "mesh.h"

void draw_Edge_bresenham(int x_1, int y_1, int x_2, int y_2, const char *ch)
{
    // ncurses 화면 밖으로 이탈 방지용 크기 체크
    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);

    // 선이 거꾸로 그려지는 상황(좌측/상단 방향)을 처리하기 위한 증감 방향 변수
    int sx = (x_1 < x_2) ? 1 : -1;
    int sy = (y_1 < y_2) ? 1 : -1;

    int dx = abs(x_2 - x_1);
    int dy = abs(y_2 - y_1);

    // 기울기가 1보다 큰 경우
    if (dy > dx)
    {
        int F = 2 * dx - dy;      // 중단점(m_k)
        int dF_1 = 2 * dx;        // F<0일때
        int dF_2 = 2 * (dx - dy); // F<0일때

        int x = x_1;
        // y_1에서 y_2 방향으로 sy만큼 전진 (거꾸로 그리는 선도 대응 가능)
        for (int y = y_1; y != y_2 + sy; y += sy)
        {
            if (x >= 0 && x < term_width && y >= 0 && y < term_height)
            {
                mvaddstr(y, x, ch);
            }

            if (F < 0)
            {
                F += dF_1;
            }
            else
            {
                x += sx; // X축 방향으로 전진
                F += dF_2;
            }
        }
    }
    // 기울기가 1보다 작거나 같은 경우
    else
    {
        int F = 2 * dy - dx;      // 중단점(m_k)
        int dF_1 = 2 * dy;        // F<0일때
        int dF_2 = 2 * (dy - dx); // F<0일때

        int y = y_1;
        // x_1에서 x_2 방향으로 sx만큼 전진
        for (int x = x_1; x != x_2 + sx; x += sx)
        {
            if (x >= 0 && x < term_width && y >= 0 && y < term_height)
            {
                mvaddstr(y, x, ch);
            }

            if (F < 0)
            {
                F += dF_1;
            }
            else
            {
                y += sy; // Y축 방향으로 전진
                F += dF_2;
            }
        }
    }
}

void draw_vertex(int x1, int y1, int x2, int y2) // 버택스 그리는 함수
{
    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);

    if (x1 >= 0 && x1 < term_width && y1 >= 0 && y1 < term_height)
    {
        mvaddch(y1, x1, 'o');
    }
    if (x2 >= 0 && x2 < term_width && y2 >= 0 && y2 < term_height)
    {
        mvaddch(y2, x2, 'o');
    }
}

void draw_projected_mesh(Mesh *mesh) // 구조체로 받은 이유: 실시간으로 수정하고 바로 바로 저장하려고 // 버텍스 수동 선택후 엣지와 페이스 만드는  함수는 따로 작성해야할듯
{
    float distance = 3.5f; // 카메라 거리 -> 마우스 휠 스크롤 해서 크기 조절
    float scale_y = 8.0f;
    float scale_x = scale_y * 2.2f; // 터미널 특성상 y가 x의 2.2배여서 배율 적용

    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);

    int center_x = term_width / 2;
    int center_y = term_height / 2;

    for (int i = 0; i < mesh->edge_count; i++)
    {
        int v1 = mesh->edges[i].v1_index;
        int v2 = mesh->edges[i].v2_index;

        // mesh.h에 정의된 구조를 활용해 원본 좌표 주소록 매칭
        Vertex p1 = mesh->vertices[v1];
        Vertex p2 = mesh->vertices[v2];

        // 원근 투영 -> 함수로 바꾸기
        float proj_x1 = (p1.x / (p1.z + distance)) * scale_x + center_x;
        float proj_y1 = (p1.y / (p1.z + distance)) * scale_y + center_y;
        float proj_x2 = (p2.x / (p2.z + distance)) * scale_x + center_x;
        float proj_y2 = (p2.y / (p2.z + distance)) * scale_y + center_y;

        // 터미널에서 출력해야하기 때문에 소숫점으로 버림 -> 추후에 여러칸을 1픽셀로 치환해서 정밀한 값 나타낼 수 있게 구현해보기
        int screen_x1 = (int)proj_x1;
        int screen_y1 = (int)proj_y1;
        int screen_x2 = (int)proj_x2;
        int screen_y2 = (int)proj_y2;

        // 순서는 엣지 -> 버텍스

        // 엣지 출력 함수 호출
        draw_Edge_bresenham(screen_x1, screen_y1, screen_x2, screen_y2, "·"); //"·" 가운뎃점은 문자가 아니라서 다른 형식이 필요
        // 버텍스 출력 함수 호출
        draw_vertex(screen_x1, screen_y1, screen_x2, screen_y2);
    }
}

void run_terminal() // 여기서 이제 다 할거임 ㅇㅇ
{
    nodelay(stdscr, TRUE);
    clear();
    MEVENT event;

    typedef struct main
    {
        int y;
        int x;
    } pos;

    pos drag_strat;
    pos drag_end;

    bool is_dragging = false;

    while (true)
    {
        int input = getch(); // 엔터없이도 바로 입력 받을 수 있음

        if (input == 'q' || input == 'Q')
        {
            printf("\033[?1003l"); // 103 모드 끄기
            fflush(stdout);
            break;
        }
        mvprintw(0, 0, "커서 좌표: X=%d, Y=%d\n", event.x, event.y);
        mvprintw(1, 0, "Drag State  : %s", is_dragging ? "DRAGGING (ON)" : "STOPPED (OFF)"); // 드래그 상태보기
        if (input == KEY_MOUSE)
        {
            if (getmouse(&event) == OK)
            {
                // 비트 연산자를 이용하는 이유 -> 옵시디언에 올리기
                // 드래그 상태 갱신
                if (event.bstate & BUTTON1_PRESSED) // 드래그 시작
                {
                    if (!(is_dragging)) // 첫 드래그 확인일때만 확인해서 시작점 고정
                    {
                        drag_strat = (pos){event.y, event.x};
                    }
                    is_dragging = true;
                }
                else if (event.bstate & BUTTON1_RELEASED || event.bstate == 0) // 드래그 종료
                {
                    drag_end = (pos){event.y, event.x}; // 얘는 drag_start와 다르게 드래그 할때마다 갱신
                    is_dragging = false;
                    clear();
                }

                if (event.bstate & REPORT_MOUSE_POSITION) // 드래그 중에 할일
                {
                    drag_end = (pos){event.y, event.x};
                    if (is_dragging)
                    {
                        // 드래그 영역 표시하기 -> 문제점 이미 그려진 드래그 영역은 지워지지 않고 드래그 영역을 줄여도 반영이 안됨 + 드래그 방향에 따라서 출력 반복문을 바꿔야함(해결함)
                        for (int i = 0; i < abs(drag_end.y - drag_strat.y); i++)
                        {
                            for (int j = 0; j < abs(drag_end.x - drag_strat.x); j++)
                            {
                                // 엣지 출력 함수에 브레젠험 알로리즘 코드 활용
                                int sy = (drag_end.y >= drag_strat.y) ? i : -i;
                                int sx = (drag_end.x >= drag_strat.x) ? j : -j;

                                mvaddch(drag_strat.y + sy, drag_strat.x + sx, '#');
                                // attron(COLOR_PAIR(1));  
                                // mvaddstr(drag_strat.y + sy, drag_strat.x + sx, " ");
                                // attroff(COLOR_PAIR(1)); 
                            }
                        }
                    }
                }
            }
        }
        refresh();
    }
}

int main()
{
    // 세팅 -> 이거도 나중에 걍 함수로 만들자 setting();
    setlocale(LC_ALL, ""); // "·" 출력하기 위한 세팅
    initscr();             // ncurses 모드 시작
    cbreak();              // 터미널 입력 버퍼링 헤제(키 누르면 바로 인식)
    noecho();              // 사용자가 입력한 글자가 출력 안되도록 가림
    curs_set(0);           // 커서 가리기
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    // start_color();
    // use_default_colors();
    // init_pair(1, -1, COLOR_WHITE);

    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL); // 마우스 모드 켜기
    mouseinterval(0);                                          // 마우스 클릭 응답 지연을 제거하여 부드럽게 만듦

    printf("\033[?1003h"); // 모든 마우스를 이용한 이벤트를 실시간으로 좌표를 보고
    fflush(stdout);

    MEVENT event;

    Mesh my_cube = init_cube_mesh(); // 정육면체 기본 도형 매쉬 데이터 불러오기

    clear();

    run_terminal();
    draw_projected_mesh(&my_cube); // 추후에 드래그후 mesh만들때 어케함? -> 드래그 해서 선택된 버텍스를 임시 매쉬로 저장하고 그걸 인수에 넣으면 될듯
    refresh();

    getch(); // 바로 꺼짐 방지 > 나중에 빼면됨

    free_mesh(&my_cube);
    endwin();
    return 0;
}
