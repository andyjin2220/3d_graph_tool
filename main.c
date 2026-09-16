#include <ncurses.h>
#include "mesh.h"



void draw_projected_mesh(Mesh *mesh)
{
    float distance = 3.5f;  //카메라 거리 -> 마우스 휠 스크롤 해서 크기 조절
    float scale_y = 8.0f;   
    float scale_x = scale_y * 2.2f; //터미널 특성상 y가 x의 2.2배여서 배율 적용

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

        //원근 투영
        float proj_x1 = (p1.x / (p1.z + distance)) * scale_x + center_x;
        float proj_y1 = (p1.y / (p1.z + distance)) * scale_y + center_y;
        float proj_x2 = (p2.x / (p2.z + distance)) * scale_x + center_x;
        float proj_y2 = (p2.y / (p2.z + distance)) * scale_y + center_y;

        int screen_x1 = (int)proj_x1;
        int screen_y1 = (int)proj_y1;
        int screen_x2 = (int)proj_x2;
        int screen_y2 = (int)proj_y2;

        if (screen_x1 >= 0 && screen_x1 < term_width && screen_y1 >= 0 && screen_y1 < term_height)
        {
            mvaddch(screen_y1, screen_x1, 'o');
        }
        if (screen_x2 >= 0 && screen_x2 < term_width && screen_y2 >= 0 && screen_y2 < term_height)
        {
            mvaddch(screen_y2, screen_x2, 'o');
        }
    }
}

int main()
{
    // 세팅
    initscr();   // ncurses 모드 시작
    cbreak();    // 터미널 입력 버퍼링 헤제(키 누르면 바로 인식)
    noecho();    // 사용자가 입력한 글자가 출력 안되도록 가림
    curs_set(0); //커서 가리기

    
    Mesh my_cube = init_cube_mesh();    //정육면체 기본 도형 매쉬 데이터 불러오기

    clear();
    mvprintw(0, 0, "=== Modular 3D Cube Perspective Test ===");
    mvprintw(1, 0, "Successfully built with separate files! Press any key to exit.");

    draw_projected_mesh(&my_cube);  //추후에 드래그후 mesh만들때 어케함? -> 드래그 해서 선택된 버텍스를 임시 매쉬로 저장하고 그걸 인수에 넣으면 될듯
    refresh();

    getch();    //바로 꺼짐 방지 > 나중에 빼면됨

    free_mesh(&my_cube);
    endwin();
    return 0;
}
