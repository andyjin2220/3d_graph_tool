#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "mesh.h"
#include "InputDevice.h"
#include "matrix.h"

extern float global_distance;

void run_terminal()
{
    Mesh my_cube = init_cube_mesh();
    nodelay(stdscr, TRUE);

    is_dragging = false;
    is_wheel_holding = false;
    Toggle_G = false;
    Toggle_P = false;

    timeout(16); // 사용자 입력을 16밀리초(0.016초) 동안 기다립니다.

    while (true)
    {
        //===============================================================
        // 사용자 입력 데이터 - 사용자 입력 싹다 모으기
        //==============================================================
        InputState input = {0};
        input.key = getch();

        if (input.key == KEY_MOUSE)
        {
            MEVENT event;
            if (getmouse(&event) == OK)
            {
                input.is_mouse_event = true;
                input.mouse_x = event.x;
                input.mouse_y = event.y;
                input.bstate = event.bstate; // 마우스 왼쪽 오른쪽 휠 이런정보가 여기 저장됨
            }
        }

        // 종료 조건 체크
        if (input.key == 'q' || input.key == 'Q')
        {
            printf("\033[?1003l");
            fflush(stdout);
            free_mesh(&my_cube);
            return;
        }
        //==================================================================
        // 행동 판단 및 연산 - 조건들을 싹 모으는 곳
        //==================================================================

        handle_keyboard_actions(&my_cube, input.key); // 키보드 행동 조건 모음
        handle_mouse_drag_actions(input);             // 마우스 드래그 행동 조건 모음
        handle_mouse_wheel_actions(input);            // 마우스 휠 행동 조건 모음
        handle_combo_actions(&my_cube, input);        // 마우스+키보드 콤보 행동 조건 모음

        // p================================================================
        //  화면 그리기 - 청소 후 순서대로 드로잉
        //====================================================================
        clear();

        mvprintw(0, 0, "=== 3D Engine Architecture Model ===");
        mvprintw(1, 0, "WASD: Move Cube | Mouse Left Click + Drag: Selection Box");
        mvprintw(2, 0, "Cube Pos Info: X=%.2f, Y=%.2f, Z=%.2f", my_cube.vertices[0].x, my_cube.vertices[0].y, my_cube.vertices[0].z);

        // 내부 핵심 토글 가방 상태 출력 (삼항 연산자로 가시성 확보)
        mvprintw(4, 0, "Toggle States : [Grab(G): %s]  [Panning(P): %s]  [Wheel-Hold: %s]    ",
                 Toggle_G ? "ON " : "OFF",
                 Toggle_P ? "ON " : "OFF",
                 is_wheel_holding ? "DOWN" : "UP  ");

        // 현재 활성화된 조작 모드 상태 출력
        if (Toggle_G)
        {
            mvprintw(5, 0, "Active Mode   : 🟢 OBJECT GRAB (Moving Mesh with Mouse)        ");
        }
        else if (Toggle_P && is_wheel_holding)
        {
            mvprintw(5, 0, "Active Mode   : 🔵 VIEWPORT PANNING (Moving Camera with Wheel) ");
        }
        else if (Toggle_P)
        {
            mvprintw(5, 0, "Active Mode   : 🟡 PANNING STANDBY (Press Wheel Click to Move) ");
        }
        else if (is_dragging)
        {
            mvprintw(5, 0, "Active Mode   : 🟠 SELECTION BOX DRAGGING                      ");
        }
        else
        {
            mvprintw(5, 0, "Active Mode   : ⚫ IDLE VIEWMODE (Camera Distance: %.1f)        ", global_distance);
        }

        mvprintw(6, 0, "--------------------------------------------------------");

        // 드래그 영역 출력
        if (is_dragging && Toggle_P == false)
        {
            // 드래그 영역 표시하기 -> 문제점 이미 그려진 드래그 영역은 지워지지 않고 드래그 영역을 줄여도 반영이 안됨 + 드래그 방향에 따라서 출력 반복문을 바꿔야함 (해결함)
            for (int i = 0; i < abs(drag_end.y - drag_strat.y); i++)
            {
                for (int j = 0; j < abs(drag_end.x - drag_strat.x); j++)
                {
                    int sy = (drag_end.y >= drag_strat.y) ? i : -i;
                    int sx = (drag_end.x >= drag_strat.x) ? j : -j;

                    mvaddch(drag_strat.y + sy, drag_strat.x + sx, '#');
                }
            }
        }
        // 3D 메쉬 투영 및 출력ㅔ
        draw_projected_mesh(&my_cube);
        // 3d 공간내 표시점
        draw_world_gizmo();

        refresh(); // 갱신
    }
}

int main()
{

    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    mouseinterval(0);

    printf("\033[?1003h");
    fflush(stdout);

    Mesh my_cube = init_cube_mesh();

    clear();
    run_terminal();

    refresh();

    free_mesh(&my_cube);
    endwin();
    return 0;
}
