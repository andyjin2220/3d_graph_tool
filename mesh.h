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

#endif
