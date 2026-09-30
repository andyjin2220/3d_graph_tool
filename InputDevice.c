#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "mesh.h"
#include "InputDevice.h"
#include "matrix.h"

extern float global_distance;
extern float camera_x;
extern float camera_y;

// 지난 프레임의 마우스 좌표를 기억하기 위한 변수
int prev_mouse_x = -1;
int prev_mouse_y = -1;

// 키보드 행동 조건 모음
void handle_keyboard_actions(Mesh *mesh, int key)
{
    // 매쉬 이동 테스트
    // if (key == 'w' || key == 'W')
    //     move_mesh(mesh, 0.0f, -0.5f, 0.0f);
    // if (key == 's' || key == 'S')
    //     move_mesh(mesh, 0.0f, 0.5f, 0.0f);
    // if (key == 'a' || key == 'A')
    //     move_mesh(mesh, -0.5f, 0.0f, 0.0f);
    // if (key == 'd' || key == 'D')
    //     move_mesh(mesh, 0.5f, 0.0f, 0.0f);
    if (key == 'g' || key == 'G')
    {
        Toggle_G = !Toggle_G;
        
    }
    if (key == 'p' || key == 'P') // 원래는 누르고 있을대가 토글이 켜지고 때면 바로 꺼지게 만들려고 했는데 마우스 입력때문에 안되서 토글 형식으로 교체함
    {
        Toggle_P = !Toggle_P;
    }
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
    // 조건 D: 마우스 휠 클릭 중일때
    if (input.bstate & 0x40)
    {
        is_wheel_holding = true;
    }
    else if ((input.bstate & 0x20) || input.bstate == 0)
    {
        is_wheel_holding = false;
    }
}

// 마우스 휠 행동 조건 모음
void handle_mouse_wheel_actions(InputState input)
{
    if (!input.is_mouse_event)
        return;

    // 휠을 위로 굴렸을 때 -> 카메라가 물체와 가까워짐 (원근 왜곡 증가)
    if (input.bstate & BUTTON4_PRESSED)
    {
        if (global_distance > 3.0f)
        {
            global_distance -= 0.5f;
        }
    }

    // 휠 아래로 스크롤 (축소 / distance 증가)
    if (input.bstate & BUTTON5_PRESSED) // BUTTON5_PRESSED 인식을 못해서 개지랄을 다함
    {
        // if (global_distance < 20.0f)
        // {
        //     //global_distance += 0.3f;
        // }
        global_distance += 0.5f;
    }
}

void handle_combo_actions(Mesh *mesh, InputState input)
{
    grab_mode(mesh, input);
    panning_mode(mesh, input);
}

bool is_first_grab = true;   // 지금 막 그랩을 시작했는지 판별하는 플래그
Vertex grab_start_3d = {0};  // 그랩을 시작한 순간의 마우스 3D 절대 좌표
Vertex mesh_start_pos = {0}; // 그랩을 시작한 순간의 메쉬 현재 3D 중심 좌표

//  G+마우스 이동 -> 매쉬 이동 함수
void grab_mode(Mesh *mesh, InputState input) // 처음 마우스 좌표를 기록해놓고 기능하는동안 이동 거리를 계산하고 토글이 끝아면 다시 초기화
{
    // [토글 상태 체크] G 키 토글이 켜져 있을 때만 발동
    if (Toggle_G && input.is_mouse_event)
    {
        int term_width, term_height;
        getmaxyx(stdscr, term_height, term_width);

        // 현재 프레임의 마우스 위치를 3D 공간 실수 좌표로 역투영
        Vertex current_mouse_3d = unproject_mouse(input.mouse_x, input.mouse_y, global_distance, term_width, term_height, 0.0f);

        // 그랩 모드 켜지고 '첫 프레임' 진입일 때만 실행
        if (is_first_grab)
        {
            // 그랩을 시작한 순간의 마우스 3D 좌표를 기록
            grab_start_3d = current_mouse_3d;

            // 그랩을 시작한 순간의 메쉬의 원래 3D 중심점(평균값)을 구해 기록
            mesh_start_pos.x = 0.0f;
            mesh_start_pos.y = 0.0f;
            for (int i = 0; i < mesh->vertex_count; i++)
            {
                mesh_start_pos.x += mesh->vertices[i].x;
                mesh_start_pos.y += mesh->vertices[i].y;
            }
            mesh_start_pos.x /= mesh->vertex_count;
            mesh_start_pos.y /= mesh->vertex_count;

            is_first_grab = false; // 첫 진입 설정을 꺼서 다음 프레임부턴 패스하게 만듭니다.
        }

        // 그랩 시작점으로부터 마우스가 이동한 3D 절대 변화량(Delta) 유도
        float delta_3d_x = current_mouse_3d.x - grab_start_3d.x;
        float delta_3d_y = current_mouse_3d.y - grab_start_3d.y;

        // 현재 메쉬의 실시간 3D 중심점 추적
        float current_mesh_center_x = 0.0f;
        float current_mesh_center_y = 0.0f;
        for (int i = 0; i < mesh->vertex_count; i++)
        {
            current_mesh_center_x += mesh->vertices[i].x;
            current_mesh_center_y += mesh->vertices[i].y;
        }
        current_mesh_center_x /= mesh->vertex_count;
        current_mesh_center_y /= mesh->vertex_count;

        // 최종 연산
        float target_x = mesh_start_pos.x + delta_3d_x;
        float target_y = mesh_start_pos.y + delta_3d_y;

        float diff_x = target_x - current_mesh_center_x;
        float diff_y = target_y - current_mesh_center_y;

        // 6. 정확한 오차만큼만 메쉬를 스무스하게 누적 이동시킵니다.
        if (diff_x != 0.0f || diff_y != 0.0f)
        {
            move_mesh(mesh, diff_x, diff_y, 0.0f);
        }
    }
    else
    {
        // G토글이 꺼지는 순간 다음 그랩을 위해 첫 진입 플레그를 다시 초기화
        is_first_grab = true;
    }
}

bool is_first_panning = true;      // 지금 막 패닝을 시작했는지 판별하는 플래그
Vertex panning_start_3d = {0};     // 패닝을 시작한 순간의 마우스 3D 시작 좌표
float camera_start_x = 0.0f;       // 패닝을 시작한 순간의 카메라 원래 X 위치
float camera_start_y = 0.0f;       // 패닝을 시작한 순간의 카메라 원래 Y 위치

void panning_mode(Mesh *mesh, InputState input) // 화면 이동
{
    if (!Toggle_P || !input.is_mouse_event) // 입력 없으면 넘어기기
    {
        is_first_panning = true;
        return;
    }

    if (is_wheel_holding)
    {
        int term_width, term_height;
        getmaxyx(stdscr, term_height, term_width);

        //현재 프레임의 마우스 위치를 3D 공간 실수 좌표로 역투영
        Vertex current_mouse_3d = unproject_mouse(input.mouse_x, input.mouse_y, global_distance, term_width, term_height, 0.0f);

        // 휠 클릭을 누른 처음 진입일 때만 실행 -> 패닝 시작 마우스 위치 기록
        if (is_first_panning)
        {
            panning_start_3d = current_mouse_3d;

            camera_start_x = camera_x;
            camera_start_y = camera_y;

            is_first_panning = false;
        }

        // 휠 클릭 시작점으로부터 마우스가 진짜 이동한 거리 구하기
        float delta_3d_x = current_mouse_3d.x - panning_start_3d.x;
        float delta_3d_y = current_mouse_3d.y - panning_start_3d.y;

        //카메라 위치 갱신
        camera_x = camera_start_x + delta_3d_x;
        camera_y = camera_start_y + delta_3d_y;
    }
    else
    {
        //초기화
        is_first_panning = true;
    }
}

