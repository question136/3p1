#ifndef LOGIN_H
#define LOGIN_H

// 최대 아이디 길이
#define MAX_LEN 50
#define USER_FILE "users.txt"

// 현재 로그인한 사용자 ID (전역 변수)
extern char currentUserId[MAX_LEN];

// 함수 선언
void loadUsers();           // 유저 정보 불러오기
void do_register();         // 회원가입
int do_login();             // 로그인 (성공:1, 실패:0)
void clear_buffer();        // 입력 오류 방지

#endif
