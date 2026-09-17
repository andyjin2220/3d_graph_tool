#include "mesh.h"
#include <stdlib.h>

Mesh init_cube_mesh()
{
    Mesh cube;

    // 1. 꼭지점 8개 메모리 파기
    cube.vertex_count = 8;
    cube.vertices = (Vertex *)malloc(sizeof(Vertex) * 8);

    // 실수(float) 공간에 정육면체 꼭지점 배치 (중심이 0,0,0인 크기 2짜리 큐브)
    cube.vertices[0] = (Vertex){-2.0f,  2.0f,  2.0f}; // 앞 좌상
    cube.vertices[1] = (Vertex){ 2.0f,  2.0f,  2.0f}; // 앞 우상
    cube.vertices[2] = (Vertex){ 2.0f, -2.0f,  2.0f}; // 앞 우하
    cube.vertices[3] = (Vertex){-2.0f, -2.0f,  2.0f}; // 앞 좌하
    cube.vertices[4] = (Vertex){-2.0f,  2.0f, -2.0f}; // 뒤 좌상
    cube.vertices[5] = (Vertex){ 2.0f,  2.0f, -2.0f}; // 뒤 우상
    cube.vertices[6] = (Vertex){ 2.0f, -2.0f, -2.0f}; // 뒤 우하
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
