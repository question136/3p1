/*
 * 발표용 파일 권한 시스템 (Auto-Sync Version)
 * 기능: 실제 파일이 삭제되면 목록에서도 자동으로 사라짐 (동기화)
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <io.h> // [중요] 파일 존재 여부 확인용 (_access)
#include "login.h"

#define INFO_FILE "files_info.txt" 

typedef struct {
    char fileName[50];
    char owner[50];
    int isPublic;
    char content[200];
} FileNode;

FileNode fileList[100];
int fileCount = 0;

// --- 함수 선언 ---
void load_files();
void rewrite_info_file(); // [NEW] 장부 최신화 함수
void save_file_content(char* name, char* content);

void show_guide();
void show_files();
void create_file();
void try_read_file();

int main() {
    loadUsers();
    load_files(); // [핵심] 여기서 실제 파일 검사를 수행함

    show_guide();

    int choice;
    int isLogin = 0;

    while (1) {
        if (isLogin == 0) {
            printf("\n=== 메인 메뉴 ===\n");
            printf("1. 로그인  2. 회원가입  3. 종료\n선택 > ");
            if (scanf("%d", &choice) != 1) { clear_buffer(); continue; }

            if (choice == 1) {
                if (do_login()) {
                    isLogin = 1;
                    printf("\n>> 환영합니다! [%s]님.\n", currentUserId);
                }
            }
            else if (choice == 2) do_register();
            else if (choice == 3) break;
        }
        else {
            show_files();

            printf("\n[메뉴] 1. 파일 만들기  2. 파일 읽기  3. 로그아웃\n선택 > ");
            if (scanf("%d", &choice) != 1) { clear_buffer(); continue; }

            if (choice == 1) create_file();
            else if (choice == 2) try_read_file();
            else if (choice == 3) {
                isLogin = 0;
                printf(">> 로그아웃 되었습니다.\n");
            }
        }
    }
    return 0;
}

// --- [동기화 기능 구현] ---

// 1. 파일 불러오기 + 실제 존재 여부 체크 (Sync)
void load_files() {
    FILE* fp = fopen(INFO_FILE, "r");
    if (fp == NULL) return;

    fileCount = 0;
    char tempName[50], tempOwner[50];
    int tempPub;

    // 장부를 한 줄씩 읽음
    while (fscanf(fp, "%s %s %d", tempName, tempOwner, &tempPub) != EOF) {

        // [핵심] 실제 파일이 존재하는지 확인 (_access 함수)
        // 0이면 존재, -1이면 없음
        if (_access(tempName, 0) == 0) {
            // A. 실제 파일이 있으면 -> 리스트에 등록
            strcpy(fileList[fileCount].fileName, tempName);
            strcpy(fileList[fileCount].owner, tempOwner);
            fileList[fileCount].isPublic = tempPub;

            // 내용 읽어오기
            FILE* realFile = fopen(tempName, "r");
            if (realFile) {
                fscanf(realFile, "%s", fileList[fileCount].content);
                fclose(realFile);
            }
            else {
                strcpy(fileList[fileCount].content, "(내용 없음)");
            }
            fileCount++;
        }
    }
    fclose(fp);

    // [중요] 깨끗해진 리스트로 장부(files_info.txt)를 다시 씀
    rewrite_info_file();

}

// 2. 장부(files_info.txt)를 현재 리스트대로 덮어쓰기
void rewrite_info_file() {
    FILE* fp = fopen(INFO_FILE, "w"); // "w" 모드는 내용을 싹 지우고 새로 씀
    if (fp == NULL) return;

    for (int i = 0; i < fileCount; i++) {
        fprintf(fp, "%s %s %d\n", fileList[i].fileName, fileList[i].owner, fileList[i].isPublic);
    }
    fclose(fp);
}

// 3. 실제 내용 파일(.txt) 생성
void save_file_content(char* name, char* content) {
    FILE* fp = fopen(name, "w");
    if (fp) {
        fprintf(fp, "%s", content);
        fclose(fp);
    }
}

// --- [기능 구현] ---

void create_file() {
    if (fileCount >= 100) return;

    printf("\n[새 파일 만들기]\n");
    printf("파일명 입력(예: a.txt): ");
    scanf("%s", fileList[fileCount].fileName);

    printf("내용 입력(띄어쓰기 금지): ");
    scanf("%s", fileList[fileCount].content);

    printf("공개 여부 (1:전체공개, 0:비공개): ");
    scanf("%d", &fileList[fileCount].isPublic);

    strcpy(fileList[fileCount].owner, currentUserId);

    // 실제 파일 생성
    save_file_content(fileList[fileCount].fileName, fileList[fileCount].content);

    fileCount++;

    // 장부 업데이트
    rewrite_info_file();

    printf(">> 파일 저장 및 권한 설정 완료!\n");
}

void try_read_file() {
    if (fileCount == 0) { printf(">> 파일 없음\n"); return; }

    int num;
    printf("읽을 파일 번호: ");
    if (scanf("%d", &num) != 1) { clear_buffer(); return; }
    if (num < 1 || num > fileCount) return;

    int idx = num - 1;
    int isAllowed = 0;

    // 권한 체크
    if (strcmp(currentUserId, "admin") == 0) isAllowed = 1;
    else if (fileList[idx].isPublic == 1) isAllowed = 1;
    else if (strcmp(currentUserId, fileList[idx].owner) == 0) isAllowed = 1;

    if (isAllowed) {
        printf(">> [승인] 내용: %s\n", fileList[idx].content);
    }
    else {
        printf(">> [거부] 읽기 권한이 없습니다 (소유자: %s)\n", fileList[idx].owner);
    }
}

void show_guide() {
        printf("\n");
    printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃             파일 관리 시스템 사용설명서              ┃\n");
    printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
    printf("┃ 1. 시작하기: '회원가입'으로 나만의 계정을 만드세요.  ┃\n");
    printf("┃ 2. 파일생성: 로그인 후 파일을 직접 만들어보세요.     ┃\n");
    printf("┃    * [비공개]로 만들면 다른 사람은 볼 수 없습니다.   ┃\n");
    printf("┃ 3. 권한확인: 로그아웃 후 다른 아이디로 접속해서      ┃\n");
    printf("┃    내 파일을 읽을 수 있는지 테스트해보세요.          ┃\n");
    printf("┃ 4. 영구저장: 프로그램을 껐다 켜도 데이터는 남습니다. ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");
}

void show_files() {
    printf("\n[파일 목록]\n");
    for (int i = 0; i < fileCount; i++) {
        char status[20];
        if (fileList[i].isPublic == 1) strcpy(status, "공개");
        else strcpy(status, "비공개");
        printf("[%d] %s (주인:%s) [%s]\n", i + 1, fileList[i].fileName, fileList[i].owner, status);
    }
    printf("---------------------------\n");
}
