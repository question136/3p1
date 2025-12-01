/*
 * 모듈: 사용자 관리 (로그인/회원가입)
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "login.h"

typedef struct {
    char id[MAX_LEN];
    char pw[MAX_LEN];
} User;

User users[100];
int userCount = 0;
char currentUserId[MAX_LEN]; // 현재 로그인한 ID 저장

void clear_buffer() { while (getchar() != '\n'); }

// 1. 유저 정보 불러오기
void loadUsers() {
    FILE* fp = fopen(USER_FILE, "r");
    if (fp == NULL) {
        fp = fopen(USER_FILE, "w");
        // 기본 관리자 계정 생성 (admin / 1234)
        if (fp) { fprintf(fp, "admin 1234\n"); fclose(fp); }
        return;
    }
    userCount = 0;
    while (fscanf(fp, "%s %s", users[userCount].id, users[userCount].pw) != EOF) {
        userCount++;
    }
    fclose(fp);
}

// 2. 파일에 저장
void saveUser(char* id, char* pw) {
    FILE* fp = fopen(USER_FILE, "a");
    if (fp) { fprintf(fp, "%s %s\n", id, pw); fclose(fp); }
}

// 3. 회원가입
void do_register() {
    char id[MAX_LEN], pw[MAX_LEN];
    printf("\n[회원가입]");
    printf("\n사용할 ID: ");
    scanf("%s", id);

    // 중복 체크
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].id, id) == 0) {
            printf(">> 이미 있는 ID입니다.\n"); return;
        }
    }
    printf("\n[회원가입]");
    printf("\n사용할 PW: ");
    scanf("%s", pw);

    strcpy(users[userCount].id, id);
    strcpy(users[userCount].pw, pw);
    userCount++;
    saveUser(id, pw);
    printf(">> 가입 완료! 로그인해주세요.\n");
}

// 4. 로그인
int do_login() {
    char id[MAX_LEN], pw[MAX_LEN];
    printf("[로그인]\n");
    printf("\nID: "); scanf("%s", id);
    printf("PW: "); scanf("%s", pw);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].id, id) == 0 && strcmp(users[i].pw, pw) == 0) {
            strcpy(currentUserId, id);
            return 1; // 성공
        }
    }
    printf(">> 아이디 또는 비밀번호가 틀렸습니다.\n");
    return 0;
}
