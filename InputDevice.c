#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>
#include "mesh.h"
#include "InputDevice.h"

// 키보드 행동 조건 모음
void handle_keyboard_actions(Mesh *mesh, int key)
{
    if (key == 'w' || key == 'W')
        move_mesh(mesh, 0.0f, -0.5f, 0.0f);
    if (key == 's' || key == 'S')
        move_mesh(mesh, 0.0f, 0.5f, 0.0f);
    if (key == 'a' || key == 'A')
        move_mesh(mesh, -0.5f, 0.0f, 0.0f);
    if (key == 'd' || key == 'D')
        move_mesh(mesh, 0.5f, 0.0f, 0.0f);
}

// 마우스 드래그 행동 조건 모음
void handle_mouse_drag_actions(InputState input)
{
    if (!input.is_mouse_event)
        return; // 마우스 움직임이 없으면 통과

    // 조건 A: 마우스 왼쪽 버튼을 처음 눌렀을 때 -> 드래그 시작
    if (input.bstate & BUTTON1_PRESSED)
    {
        if (!is_dragging)
        {
            drag_strat = (pos){input.mouse_y, input.mouse_x};
        }
        is_dragging = true;
    }
    // 조건 B: 마우스 왼쪽 버튼을 뗐을 때 -> 드래그 종료 및 매쉬 생성 유도 기점
    else if (input.bstate & BUTTON1_RELEASED || input.bstate == 0)
    {
        drag_end = (pos){input.mouse_y, input.mouse_x};
        is_dragging = false;
    }

    // 조건 C: 누른 상태로 이동 중일 때 -> 끝점 실시간 갱신
    if (input.bstate & REPORT_MOUSE_POSITION)
    {
        drag_end = (pos){input.mouse_y, input.mouse_x};
    }
}