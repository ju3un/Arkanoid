#include "turboc.h"
#include <math.h>
#pragma warning(disable : 4996)
#define BRICK_NUM	30
#define MAX 50

//┌─┐│┘└  ← : 75, → : 77, ↑ : 72, ↓ : 80, ENTER키 : 13  〓〓〓〓〓  ○

typedef struct brickxy {		//벽돌 좌표값을 위한 구조체 선언
	int leftx;
	int rightx;
	int y;
}brick;

typedef struct ballxy {			//공의 좌표값을 위한 구조체 선언
	int x;
	int y;
	int angle;
	int xsw;
	int ysw;
}ball;

typedef struct barxy {			//발판 좌표값 구조체 선언
	int leftx;
	int rightx;
	int y;
}bar;

void textcolor(int color_number)			//벽돌 색상 선언
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_number);
}

void APPEAR(int);
int GAME_START();																	//게임시작
int STAGE_SELECT();																	//스테이지 선택
void SCORE_PRINT(int);																//점수 출력
void LIFE_PRINT(int);																//생명 출력
void INIT_VALUE(ball *, bar *, int *, int *, int *, int *, int *, int *);			//공 튀기기
void INIT_VALUE2(ball *, bar *, int *, int *, int *, int *);						//낙하
void CREATE_BRICK(brick *, int);													//벽돌 생성
void CREATE_BAR(bar *);																//발판 생성
void CREATE_BALL(ball *);															//공
void BAR_MOVE(bar *, int);															//발판 이동
void BALL_MOVE(ball *, bar *, int);													//공 이동
int BREAK_CHECK(brick *, ball *);													//파괴 판정
void BREAK_UP(brick *);																//파괴 처리
int RESTART_CHECK(int);																//재시작

void main() {
	int start, key, i, check, stage, broken_bk, clear, score, count, life;
	int break_check[MAX] = { 0 };
	brick bk[MAX];
	ball bl;
	bar br;

	start = GAME_START();															//시작 화면
	stage = STAGE_SELECT();															//스테이지 선택
	life = 3;																		//최초 생명 3
	randomize();

	while ((start) && (stage < 5)) {												//선택한 스테이지를 루프시작
		INIT_VALUE(&bl, &br, &key, &check, &broken_bk, &clear, &score, &count);
		CREATE_BRICK(bk, stage);
		CREATE_BAR(&br);
		CREATE_BALL(&bl);

		while (life >= 0) {			//생명 체크(생명이 있으면 루프시작)
			gotoxy(20, 20);
			printf("                    ");
			gotoxy(2, 39);
			printf("                                                                ");
			INIT_VALUE2(&bl, &br, &key, &check, &clear, &count);
			CREATE_BAR(&br);		//발판생성
			CREATE_BALL(&bl);		//공생성
			while (1) {
				if (kbhit()) {
					key = getch();
					BAR_MOVE(&br, key);
					if (!check && key == 75) {
						check++;
						bl.xsw = -1;
					}
					else if((!check && key == 77))
						check++;
				}

				else if (key) {
					BALL_MOVE(&bl, &br, stage);

					for (i = 0; i < (BRICK_NUM + (stage * 5)); i++) {
						break_check[i] = BREAK_CHECK(&bk[i], &bl);

						if (break_check[i]) {				//벽돌 파괴 처리
							BREAK_UP(&bk[i]);
							broken_bk++;
							score += 500;

							if (break_check[i] == 1)
								bl.ysw = -1;
							else if (break_check[i] == 2)
								bl.xsw = -1;
							else if (break_check[i] == 3)
								bl.ysw = 1;
							else if (break_check[i] == 4)
								bl.xsw = 1;
							else if (break_check[i] == 5) {
								bl.xsw = -1;
								bl.ysw = -1;
							}
							else if (break_check[i] == 6) {
								bl.xsw = 1;
								bl.ysw = -1;
							}
							else if (break_check[i] == 7) {
								bl.xsw = -1;
								bl.ysw = 1;
							}
							else if (break_check[i] == 8) {
								bl.xsw = 1;
								bl.ysw = 1;
							}
						}
					}
					if (count == 5) count = 0;			//딜레이에 따른 시간 감소 간격 조정
					if (!count) score--;
					SCORE_PRINT(score);
					LIFE_PRINT(life);
					count++;
				}

				if (bl.y == 39)
					break;
				else if (broken_bk == (BRICK_NUM + (stage * 5))) {
					clear = 1;
					break;
				}
			}

			if (clear)
				break;

			else {
				life--;
				if (life < 0) {		//실패 메세지
					gotoxy(25, 21);
					textcolor(252);
					printf("Failure...");
					textcolor(240);
					delay(4000);
				}
				else {			//낙하시 현재생명 표기
					gotoxy(20, 20);
					printf("현재 LIFE : ");
					textcolor(252);
					for (i = 0; i < life; i++)
						printf("♥");
					textcolor(240);
					delay(2000);
				}
			}
		}

		start = RESTART_CHECK(clear);		//스테이지 넘기기

		if (start == 2)
			stage++;
		else if (start == 1) {
			life = 3;
			stage = STAGE_SELECT();
		}
	}

	if (stage == 5) {
		clrscr();
		APPEAR(0);
		gotoxy(20, 15);
		printf("STAGE ALL CLEAR");
		gotoxy(20, 16);
		printf("게임을 종료합니다.");
	}
}

void APPEAR(int stage) {			//스테이지 테두리
	int i;

	gotoxy(0, 0);
	printf("┌────────────────────────────────┐\n");

	for (i = 1; i < 40; i++) {
		printf("│");
		gotoxy(66, i);
		printf("│\n");
	}

	printf("└────────────────────────────────┘\n");
	printf("  스테이지 : %d", stage + 1);
}

int GAME_START() {
	int select = 1, key = 72;

	APPEAR(0);
	gotoxy(24, 3);
	printf("게임을 시작합니다.");
	gotoxy(26, 10);
	printf("조작법");
	gotoxy(9, 12);
	printf("왼쪽으로 이동 : 방향키(←)");
	gotoxy(7, 13);
	printf("오른쪽으로 이동 : 방향키(→)");
	gotoxy(18, 14);
	printf("선택 : 방향키(↑), 방향키(↓)");
	gotoxy(18, 15);
	printf("결정 : ENTER키");
	gotoxy(24, 18);
	printf("▶ 시작");
	gotoxy(24, 19);
	printf("   종료");

	while (1) {
		if (kbhit()) {
			key = getch();

			if (key == 72) {
				gotoxy(24, 18);
				printf("       ");
				gotoxy(24, 19);
				printf("       ");
				gotoxy(24, 18);
				printf("▶ 시작");
				gotoxy(24, 19);
				printf("   종료");
				select = 1;
			}

			else if (key == 80) {
				gotoxy(24, 18);
				printf("       ");
				gotoxy(24, 19);
				printf("       ");
				gotoxy(24, 18);
				printf("   시작");
				gotoxy(24, 19);
				printf("▶ 종료");
				select = 0;
			}

			else if (key == 13)
				return select;
		}
	}
}

int STAGE_SELECT() {
	int key = 72, stage = 0;

	clrscr();
	APPEAR(0);
	gotoxy(24, 5);
	printf("STAGE 선택");
	gotoxy(24, 8);
	printf("▶ 1 STAGE");
	gotoxy(24, 11);
	printf("   2 STAGE");
	gotoxy(24, 14);
	printf("   3 STAGE");
	gotoxy(24, 17);
	printf("   4 STAGE");
	gotoxy(24, 20);
	printf("   5 STAGE");

	while (1) {
		if (kbhit()) {
			key = getch();

			if (key == 72) {			//스테이지 선택(stage값에 따른 글을 변화)
				switch (stage) {
				case 0:
					gotoxy(24, 8);
					printf("          ");
					gotoxy(24, 11);
					printf("          ");
					gotoxy(24, 8);
					printf("▶ 1 STAGE");
					gotoxy(24, 11);
					printf("   2 STAGE");
					break;
				case 1:
					gotoxy(24, 8);
					printf("          ");
					gotoxy(24, 11);
					printf("          ");
					gotoxy(24, 8);
					printf("▶ 1 STAGE");
					gotoxy(24, 11);
					printf("   2 STAGE");
					stage--;
					break;
				case 2:
					gotoxy(24, 11);
					printf("          ");
					gotoxy(24, 14);
					printf("          ");
					gotoxy(24, 11);
					printf("▶ 2 STAGE");
					gotoxy(24, 14);
					printf("   3 STAGE");
					stage--;
					break;
				case 3:
					gotoxy(24, 14);
					printf("          ");
					gotoxy(24, 17);
					printf("          ");
					gotoxy(24, 14);
					printf("▶ 3 STAGE");
					gotoxy(24, 17);
					printf("   4 STAGE");
					stage--;
					break;
				case 4:
					gotoxy(24, 17);
					printf("          ");
					gotoxy(24, 20);
					printf("          ");
					gotoxy(24, 17);
					printf("▶ 4 STAGE");
					gotoxy(24, 20);
					printf("   5 STAGE");
					stage--;
					break;
				}
			}

			else if (key == 80) {		//방향키 아래
				switch (stage) {
				case 0:
					gotoxy(24, 8);
					printf("          ");
					gotoxy(24, 11);
					printf("          ");
					gotoxy(24, 8);
					printf("   1 STAGE");
					gotoxy(24, 11);
					printf("▶ 2 STAGE");
					stage++;
					break;
				case 1:
					gotoxy(24, 11);
					printf("          ");
					gotoxy(24, 14);
					printf("          ");
					gotoxy(24, 11);
					printf("   2 STAGE");
					gotoxy(24, 14);
					printf("▶ 3 STAGE");
					stage++;
					break;
				case 2:
					gotoxy(24, 14);
					printf("          ");
					gotoxy(24, 17);
					printf("          ");
					gotoxy(24, 14);
					printf("   3 STAGE");
					gotoxy(24, 17);
					printf("▶ 4 STAGE");
					stage++;
					break;
				case 3:
					gotoxy(24, 17);
					printf("          ");
					gotoxy(24, 20);
					printf("          ");
					gotoxy(24, 17);
					printf("   4 STAGE");
					gotoxy(24, 20);
					printf("▶ 5 STAGE");
					stage++;
					break;
				case 4:
					gotoxy(24, 17);
					printf("          ");
					gotoxy(24, 20);
					printf("          ");
					gotoxy(24, 17);
					printf("   4 STAGE");
					gotoxy(24, 20);
					printf("▶ 5 STAGE");
					break;
				}
			}

			else if (key == 13)
				return stage;
		}
	}
}

void SCORE_PRINT(int score) {		//점수 표기
	gotoxy(24, 41);
	printf("점수 :      ", score);
	gotoxy(24, 41);
	printf("점수 : %d", score);
}

void LIFE_PRINT(int life) {			//상시 생명 표기
	int i;
	gotoxy(50, 41);
	printf("LIFE :           ");
	gotoxy(50, 41);
	printf("LIFE : ");
	textcolor(252);
	for (i = 0; i < life; i++)
		printf("♥");
	textcolor(240);
}


void INIT_VALUE(ball *bl, bar *br, int *key, int *check, int *broken_bk, int *clear, int *score, int *count) {
	br->leftx = 28;
	br->rightx = (br->leftx + 10);
	br->y = 39;

	bl->x = 32;
	bl->y = 38;
	bl->angle = 1;
	bl->xsw = 1;
	bl->ysw = 1;

	*key = 0;
	*check = 0;
	*broken_bk = 0;
	*clear = 0;
	*score = 500;
	*count = 0;
}

void INIT_VALUE2(ball *bl, bar *br, int *key, int *check, int *clear, int *count) {
	br->leftx = 28;
	br->rightx = (br->leftx + 10);
	br->y = 39;

	bl->x = 32;
	bl->y = 38;
	bl->angle = 1;
	bl->xsw = 1;
	bl->ysw = 1;

	*key = 0;
	*check = 0;
	*clear = 0;
	*count = 0;
}

void CREATE_BRICK(brick *b, int stage) {
	int i, j = 2;

	clrscr();
	APPEAR(stage);									//로딩
	gotoxy(24, 15);
	printf("블록을 생성합니다.\n");
	delay(2000);
	clrscr();
	APPEAR(stage);

	for (i = 0; i < (BRICK_NUM + (stage * 5)); i++) {				//벽돌 좌표 지정		
		b[i].leftx = (random(28) + 1) * 2;
		b[i].rightx = b[i].leftx + 7;
		b[i].y = random(18) + 1;

		for (j = 0; j < i; j++)
			if ((b[i].y == b[j].y) && abs(b[i].leftx - b[j].leftx) < 8)
				i--;
	}

	for (i = 0; i < (BRICK_NUM + (stage * 5)); i++) {				//스테이지에 따른 맵구성
		gotoxy(b[i].leftx, b[i].y);
		textcolor(random(15) + 240);								//벽돌 색상 지정
		for (j = b[i].leftx; j <= b[i].rightx;) {					//지정된 좌표로 벽돌 생성
			if (j == b[i].leftx) {
				printf("[");
				j++;
			}

			else if (j == b[i].rightx) {
				printf("]");
				break;
			}

			else {
				printf("■");
				j += 2;
			}
		}
	}
	textcolor(240);
}

void CREATE_BAR(bar *br) {					//발판 생성
	int i;
	textcolor(243);
	gotoxy(br->leftx, br->y);
	for (i = 0; i < 5; i++)
		printf("〓");
	textcolor(240);
}

void CREATE_BALL(ball *bl) {				//공생성
	gotoxy(bl->x, bl->y);
	printf("○");
}

void BAR_MOVE(bar *br, int key) {			//발판 움직임
	if (key == 75) {						//좌
		gotoxy(br->leftx, br->y);
		printf("          ");

		br->leftx -= 4;
		br->rightx -= 4;

		if (br->leftx < 2) {
			br->leftx = 2;
			br->rightx = 12;
		}

		CREATE_BAR(br);
	}

	else if (key == 77) {					//우
		gotoxy(br->leftx, br->y);
		printf("          ");

		br->leftx += 4;
		br->rightx += 4;

		if (br->rightx > 66) {
			br->leftx = 56;
			br->rightx = 66;
		}

		CREATE_BAR(br);
	}
}

void BALL_MOVE(ball *bl, bar *br, int stage) {
	int i;

	delay(100 - (stage * 13) + (bl->angle * 20));			//공 속도
	gotoxy(bl->x, bl->y);
	printf("  ");

	if (bl->x == 64)						//오른쪽 벽
		bl->xsw = -1;
	else if (bl->x == 2)					//왼쪽 벽
		bl->xsw = 1;

	if (bl->y == 1)							//천장
		bl->ysw = 1;

	if ((bl->x % 2) && ((bl->y + 1) == (br->y))) {
		if (((bl->x + 1) == br->leftx) || ((bl->x + 1) == br->rightx)) {
			bl->angle = 3;
			bl->ysw = -1;
		}
		else if (((bl->x - 1) == br->leftx) || ((bl->x + 3) == br->rightx)) {
			bl->angle = 2;
			bl->ysw = -1;
		}
		else if (((bl->x - 3) == br->leftx) || ((bl->x + 5) == br->rightx)) {
			bl->angle = 1;
			bl->ysw = -1;
		}
	}

	else if ((bl->y + 1) == (br->y)) {
		if ((bl->x == br->leftx) || ((bl->x + 2) == br->rightx)) {
			bl->angle = 3;
			bl->ysw = -1;
		}
		else if (((bl->x - 2) == br->leftx) || ((bl->x + 4) == br->rightx)) {
			bl->angle = 2;
			bl->ysw = -1;
		}
		else if ((bl->x - 4) == br->leftx) {
			bl->angle = 1;
			bl->ysw = -1;
		}
	}


	if (bl->xsw == 1) {
		for (i = 0; i < bl->angle; i++) {
			bl->x++;
			if (bl->x > 64)
				bl->x--;
		}
	}
	else if (bl->xsw == -1) {
		for (i = 0; i < bl->angle; i++) {
			bl->x--;
			if (bl->x < 2)
				bl->x++;
		}
	}

	if (bl->ysw == 1)
		bl->y++;
	else if (bl->ysw == -1)
		bl->y--;

	CREATE_BALL(bl);
}

int BREAK_CHECK(brick *bk, ball *bl) {
	if (bl->x % 2) {
		if (((bl->y + 1) == bk->y) && (bl->x >= (bk->leftx - 3)) && (bl->x <= (bk->rightx + 2)))
			return 1;
		else if (((bl->y - 1) == bk->y) && (bl->x >= (bk->leftx - 3)) && (bl->x <= (bk->rightx + 2)))
			return 3;
		else if ((bl->y == bk->y) && ((bl->x + 3) == bk->leftx))
			return 2;
		else if ((bl->y == bk->y) && (bl->x == (bk->rightx + 2)))
			return 4;
		else
			return 0;
	}

	else {
		if (((bl->y + 1) == bk->y) && (bl->x >= bk->leftx) && (bl->x <= (bk->rightx - 1)))
			return 1;
		else if ((bl->y == bk->y) && ((bl->x + 2) == bk->leftx))
			return 2;
		else if (((bl->y - 1) == bk->y) && (bl->x >= bk->leftx) && (bl->x <= (bk->rightx - 1)))
			return 3;
		else if ((bl->y == bk->y) && (bl->x == (bk->rightx + 1)))
			return 4;
		else if (((bl->y + 1) == bk->y) && ((bl->x + 2) == bk->leftx))
			return 5;
		else if (((bl->y + 1) == bk->y) && (bl->x == (bk->rightx + 1)))
			return 6;
		else if (((bl->y - 1) == bk->y) && ((bl->x + 2) == bk->leftx))
			return 7;
		else if (((bl->y - 1) == bk->y) && (bl->x == (bk->rightx + 1)))
			return 8;
		else
			return 0;
	}
}

void BREAK_UP(brick *bk) {					//벽돌 파괴 처리
	gotoxy(bk->leftx, bk->y);
	printf("        ");

	bk->leftx = 0;
	bk->rightx = 0;
	bk->y = 0;
}

int RESTART_CHECK(int clear) {				//재시작
	int select, key = 72;

	if (clear) {
		select = 2;
		gotoxy(20, 15);
		printf("게임을 클리어하셨습니다.");
		gotoxy(20, 16);
		printf("다음 스테이지로 이동하시겠습니까?");
		gotoxy(20, 17);
		printf("▶ 예    ");
		gotoxy(20, 18);
		printf("   아니오");

		while (1) {
			if (kbhit()) {
				key = getch();

				if (key == 72) {
					gotoxy(20, 17);
					printf("         ");
					gotoxy(20, 18);
					printf("         ");
					gotoxy(20, 17);
					printf("▶ 예    ");
					gotoxy(20, 18);
					printf("   아니오");
					select = 2;
				}

				else if (key == 80) {
					gotoxy(20, 17);
					printf("         ");
					gotoxy(20, 18);
					printf("         ");
					gotoxy(20, 17);
					printf("   예    ");
					gotoxy(20, 18);
					printf("▶ 아니오");
					select = 0;
				}

				else if (key == 13)
					return select;
			}
		}
	}

	else {								//게임 실패 이벤트
		select = 1;
		gotoxy(20, 20);
		printf("게임이 종료되었습니다.");
		gotoxy(20, 21);
		printf("다시 하시겠습니까?");
		gotoxy(20, 22);
		printf("▶ 예    ");
		gotoxy(20, 23);
		printf("   아니오");

		while (1) {
			if (kbhit()) {
				key = getch();

				if (key == 72) {
					gotoxy(20, 22);
					printf("         ");
					gotoxy(20, 23);
					printf("         ");
					gotoxy(20, 22);
					printf("▶ 예    ");
					gotoxy(20, 23);
					printf("   아니오");
					select = 1;
				}

				else if (key == 80) {
					gotoxy(20, 22);
					printf("         ");
					gotoxy(20, 23);
					printf("         ");
					gotoxy(20, 22);
					printf("   예    ");
					gotoxy(20, 23);
					printf("▶ 아니오");
					select = 0;
				}

				else if (key == 13)
					return select;
			}
		}
	}
}