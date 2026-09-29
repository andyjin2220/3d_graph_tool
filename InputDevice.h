#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

typedef struct main
{
    int y;
    int x;
} pos;

// 키보드와 마우스의 실시간 상태를 한 몸에 담는 가방 <- 키보드와 마우스 입력이 계속 중첩되서 입력받아지는 문제를 해결하기 위함
typedef struct
{
    int key;              // 현재 눌린 키보드 값 (없으면 -1)
    int mouse_x;          // 실시간 마우스 X 좌표
    int mouse_y;          // 실시간 마우스 Y 좌표
    unsigned long bstate; // 마우스 버튼 상태 (클릭, 뗌, 드래그 등)
    bool is_mouse_event;  // 이번 프레임에 마우스 신호가 들어왔는지 여부
} InputState;



pos drag_strat;
pos drag_end;

bool is_dragging;

void handle_keyboard_actions(Mesh *mesh, int key);
void handle_mouse_drag_actions(InputState input);
void handle_mouse_wheel_actions(InputState input);
#endif