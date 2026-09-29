#include "matrix.h"

// 단위 행렬 생성 (아무 변화도 주지 않는 대각선 1 형태의 기본 베이스)
Matrix4 matrix_identity()
{
    Matrix4 mat = {0};
    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;
    return mat;
}
// 이동 변환 행렬 만들기
// 4x4 행렬의 맨 우측 세로줄(T 영역)에 이동 거리를 넣기
Matrix4 matrix_make_translation(float tx, float ty, float tz)
{
    Matrix4 mat = matrix_identity(); // 기본 단위 행렬을 먼저 불러옴

    mat.m[0][3] = tx; // X축으로 이동할 거리
    mat.m[1][3] = ty; // Y축으로 이동할 거리
    mat.m[2][3] = tz; // Z축으로 이동할 거리

    return mat;
}

// 4x4 이동 행렬과 3D 버텍스를 곱해주는 진짜 연산 함수
// 끝에 가상의 숫자 '1'이 숨어있다고 생각하고 계산하는 동차 좌표계 수식
Vertex matrix_multiply_vertex(Matrix4 mat, Vertex v) {
    Vertex out;
    
    // 행렬의 [행]과 버텍스의 (X, Y, Z, 1) [열]을 순서대로 곱해서 더합니다.
    out.x = (v.x * mat.m[0][0]) + (v.y * mat.m[0][1]) + (v.z * mat.m[0][2]) + (1.0f * mat.m[0][3]);
    out.y = (v.x * mat.m[1][0]) + (v.y * mat.m[1][1]) + (v.z * mat.m[1][2]) + (1.0f * mat.m[1][3]);
    out.z = (v.x * mat.m[2][0]) + (v.y * mat.m[2][1]) + (v.z * mat.m[2][2]) + (1.0f * mat.m[2][3]);
    
    return out;
}