#ifndef MATRIX_H
#define MATRIX_H

#include "mesh.h"

// 4x4 행렬 구조체 정의 (3D 이동, 회전, 크기 변환을 모두 담을 수 있는 규격)
typedef struct
{
    float m[4][4];
} Matrix4;

// 1. 아무 변화도 주지 않는 '단위 행렬(Identity Matrix)'을 만드는 함수 (숫자 '1'과 같은 역할)
Matrix4 matrix_identity();

// 두 행렬을 곱하여 하나의 종합 변환 행렬로 합치는 핵심 함수 -> 여러 행렬을 하나로 합침
Matrix4 matrix_multiply(Matrix4 m1, Matrix4 m2);

// 4x4 변환 행렬을 3D 버텍스(점)에 곱해서 새로운 3D 점으로 변환하는 함수 (가상의 1을 붙여 계산) -> 실제로 이동 시킴
Vertex matrix_multiply_vertex(Matrix4 mat, Vertex v);

// 입력받은 (tx, ty, tz)만큼 물체를 평행 이동시키는 '이동 행렬' 생성 함수
Matrix4 matrix_make_translation(float tx, float ty, float tz);

// X축을 기준으로 지정한 각도(angle)만큼 돌려주는 'X축 회전 행렬' 생성 함수
Matrix4 matrix_make_rotation_x(float angle);

// Y축을 기준으로 지정한 각도(angle)만큼 돌려주는 'Y축 회전 행렬' 생성 함수
Matrix4 matrix_make_rotation_y(float angle);

// Z축을 기준으로 지정한 각도(angle)만큼 돌려주는 'Z축 회전 행렬' 생성 함수
Matrix4 matrix_make_rotation_z(float angle);


#endif