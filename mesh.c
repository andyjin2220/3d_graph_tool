#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "mesh.h"
#include "InputDevice.h"
#include "matrix.h"

Mesh init_cube_mesh()
{
    Mesh cube;

    // 1. 꼭지점 8개 메모리 파기
    cube.vertex_count = 8;
    cube.vertices = (Vertex *)malloc(sizeof(Vertex) * 8);

    // 실수(float) 공간에 정육면체 꼭지점 배치 (중심이 0,0,0인 크기 2짜리 큐브)
    cube.vertices[0] = (Vertex){-2.0f, 2.0f, 2.0f};   // 앞 좌상
    cube.vertices[1] = (Vertex){2.0f, 2.0f, 2.0f};    // 앞 우상
    cube.vertices[2] = (Vertex){2.0f, -2.0f, 2.0f};   // 앞 우하
    cube.vertices[3] = (Vertex){-2.0f, -2.0f, 2.0f};  // 앞 좌하
    cube.vertices[4] = (Vertex){-2.0f, 2.0f, -2.0f};  // 뒤 좌상
    cube.vertices[5] = (Vertex){2.0f, 2.0f, -2.0f};   // 뒤 우상
    cube.vertices[6] = (Vertex){2.0f, -2.0f, -2.0f};  // 뒤 우하
    cube.vertices[7] = (Vertex){-2.0f, -2.0f, -2.0f}; // 뒤 좌하

    // 2. 모서리(선) 12개 주소록 채우기
    cube.edge_count = 12;
    cube.edges = (Edge *)malloc(sizeof(Edge) * 12);
    cube.edges[0] = (Edge){0, 1};
    cube.edges[1] = (Edge){1, 2};
    cube.edges[2] = (Edge){2, 3};
    cube.edges[3] = (Edge){3, 0}; // 앞면 네모
    cube.edges[4] = (Edge){4, 5};
    cube.edges[5] = (Edge){5, 6};
    cube.edges[6] = (Edge){6, 7};
    cube.edges[7] = (Edge){7, 4}; // 뒷면 네모
    cube.edges[8] = (Edge){0, 4};
    cube.edges[9] = (Edge){1, 5};
    cube.edges[10] = (Edge){2, 6};
    cube.edges[11] = (Edge){3, 7}; // 앞뒤 연결 기둥

    // 3. 면(Face) 정보는 일단 와이어프레임(선) 중심이므로 0개로 비워둠
    cube.face_count = 0;
    cube.faces = NULL;

    return cube;
}

void move_mesh(Mesh *mesh, float tx, float ty, float tz)
{
    // 수학 모듈을 호출해 4x4 이동 행렬(주소록)을 만듭니다.
    Matrix4 trans_mat = matrix_make_translation(tx, ty, tz);

    // 점 가방(vertices) 아파트를 처음부터 끝까지 루프 돕니다.
    for (int i = 0; i < mesh->vertex_count; i++)
    {
        // i번째 원본 버텍스 좌표를 꺼내와서 행렬 곱셈 함수에 던집니다.
        Vertex updated_v = matrix_multiply_vertex(trans_mat, mesh->vertices[i]);

        // 4. 이동이 완료된 새 좌표를 제자리에 그대로 덮어씁니다.
        mesh->vertices[i] = updated_v;
    }
}

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

// 프로그램 종료 시 메모리 누수를 막는 청소 함수
void free_mesh(Mesh *mesh)
{
    if (mesh->vertices)
        free(mesh->vertices);
    if (mesh->edges)
        free(mesh->edges);
    if (mesh->faces)
        free(mesh->faces);
}
