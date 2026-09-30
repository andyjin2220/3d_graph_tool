#ifndef MESH_H
#define MESH_H

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    int v1_index;
    int v2_index;
} Edge;

typedef struct {
    int v_indices[3]; // 삼각형 면을 이루는 방 번호 3개
} Face;


typedef struct {
    Vertex* vertices;
    Edge* edges;
    Face* faces;
    int vertex_count;
    int edge_count;
    int face_count;
} Mesh;


Mesh init_cube_mesh();
void free_mesh(Mesh* mesh);
void move_mesh(Mesh *mesh, float tx, float ty, float tz);
void draw_Edge_bresenham(int x_1, int y_1, int x_2, int y_2, const char *ch);
void draw_vertex(int x1, int y1, int x2, int y2); // 버택스 그리는 함수
void draw_projected_mesh(Mesh *mesh); // 구조체로 받은 이유: 실시간으로 수정하고 바로 바로 저장하려고 // 버텍스 수동 선택후 엣지와 페이스 만드는  함수는 따로 작성해야할듯
void draw_world_gizmo(); // 3d 공간의 정 중앙에 표시점


#endif
