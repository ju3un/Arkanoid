#include "turboc.h"
#include <math.h>
#pragma warning(disable : 4996)
#define BRICK_NUM	30
#define MAX 50

//������������  �� : 75, �� : 77, �� : 72, �� : 80, ENTERŰ : 13  ������  ��

typedef struct brickxy {		//���� ��ǥ���� ���� ����ü ����
	int leftx;
	int rightx;
	int y;
}brick;

typedef struct ballxy {			//���� ��ǥ���� ���� ����ü ����
	int x;
	int y;
	int angle;
	int xsw;
	int ysw;
}ball;

typedef struct barxy {			//���� ��ǥ�� ����ü ����
	int leftx;
	int rightx;
	int y;
}bar;

void textcolor(int color_number)			//���� ���� ����
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_number);
}

void APPEAR(int);
int GAME_START();																	//���ӽ���
int STAGE_SELECT();																	//�������� ����
void SCORE_PRINT(int);																//���� ���
void LIFE_PRINT(int);																//���� ���
void INIT_VALUE(ball *, bar *, int *, int *, int *, int *, int *, int *);			//�� Ƣ���
void INIT_VALUE2(ball *, bar *, int *, int *, int *, int *);						//����
void CREATE_BRICK(brick *, int);													//���� ����
void CREATE_BAR(bar *);																//���� ����
void CREATE_BALL(ball *);															//��
void BAR_MOVE(bar *, int);															//���� �̵�
void BALL_MOVE(ball *, bar *, int);													//�� �̵�
int BREAK_CHECK(brick *, ball *);													//�ı� ����
void BREAK_UP(brick *);																//�ı� ó��
int RESTART_CHECK(int);																//�����

void main() {
	int start, key, i, check, stage, broken_bk, clear, score, count, life;
	int break_check[MAX] = { 0 };
	brick bk[MAX];
	ball bl;
	bar br;

	start = GAME_START();															//���� ȭ��
	stage = STAGE_SELECT();															//�������� ����
	life = 3;																		//���� ���� 3
	randomize();

	while ((start) && (stage < 5)) {												//������ ���������� ��������
		INIT_VALUE(&bl, &br, &key, &check, &broken_bk, &clear, &score, &count);
		CREATE_BRICK(bk, stage);
		CREATE_BAR(&br);
		CREATE_BALL(&bl);

		while (life >= 0) {			//���� üũ(������ ������ ��������)
			gotoxy(20, 20);
			printf("                    ");
			gotoxy(2, 39);
			printf("                                                                ");
			INIT_VALUE2(&bl, &br, &key, &check, &clear, &count);
			CREATE_BAR(&br);		//���ǻ���
			CREATE_BALL(&bl);		//������
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

						if (break_check[i]) {				//���� �ı� ó��
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
					if (count == 5) count = 0;			//�����̿� ���� �ð� ���� ���� ����
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
				if (life < 0) {		//���� �޼���
					gotoxy(25, 21);
					textcolor(252);
					printf("Failure...");
					textcolor(240);
					delay(4000);
				}
				else {			//���Ͻ� ������� ǥ��
					gotoxy(20, 20);
					printf("���� LIFE : ");
					textcolor(252);
					for (i = 0; i < life; i++)
						printf("��");
					textcolor(240);
					delay(2000);
				}
			}
		}

		start = RESTART_CHECK(clear);		//�������� �ѱ��

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
		printf("������ �����մϴ�.");
	}
}

void APPEAR(int stage) {			//�������� �׵θ�
	int i;

	gotoxy(0, 0);
	printf("��������������������������������������������������������������������\n");

	for (i = 1; i < 40; i++) {
		printf("��");
		gotoxy(66, i);
		printf("��\n");
	}

	printf("��������������������������������������������������������������������\n");
	printf("  �������� : %d", stage + 1);
}

int GAME_START() {
	int select = 1, key = 72;

	APPEAR(0);
	gotoxy(24, 3);
	printf("������ �����մϴ�.");
	gotoxy(26, 10);
	printf("���۹�");
	gotoxy(9, 12);
	printf("�������� �̵� : ����Ű(��)");
	gotoxy(7, 13);
	printf("���������� �̵� : ����Ű(��)");
	gotoxy(18, 14);
	printf("���� : ����Ű(��), ����Ű(��)");
	gotoxy(18, 15);
	printf("���� : ENTERŰ");
	gotoxy(24, 18);
	printf("�� ����");
	gotoxy(24, 19);
	printf("   ����");

	while (1) {
		if (kbhit()) {
			key = getch();

			if (key == 72) {
				gotoxy(24, 18);
				printf("       ");
				gotoxy(24, 19);
				printf("       ");
				gotoxy(24, 18);
				printf("�� ����");
				gotoxy(24, 19);
				printf("   ����");
				select = 1;
			}

			else if (key == 80) {
				gotoxy(24, 18);
				printf("       ");
				gotoxy(24, 19);
				printf("       ");
				gotoxy(24, 18);
				printf("   ����");
				gotoxy(24, 19);
				printf("�� ����");
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
	printf("STAGE ����");
	gotoxy(24, 8);
	printf("�� 1 STAGE");
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

			if (key == 72) {			//�������� ����(stage���� ���� ���� ��ȭ)
				switch (stage) {
				case 0:
					gotoxy(24, 8);
					printf("          ");
					gotoxy(24, 11);
					printf("          ");
					gotoxy(24, 8);
					printf("�� 1 STAGE");
					gotoxy(24, 11);
					printf("   2 STAGE");
					break;
				case 1:
					gotoxy(24, 8);
					printf("          ");
					gotoxy(24, 11);
					printf("          ");
					gotoxy(24, 8);
					printf("�� 1 STAGE");
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
					printf("�� 2 STAGE");
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
					printf("�� 3 STAGE");
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
					printf("�� 4 STAGE");
					gotoxy(24, 20);
					printf("   5 STAGE");
					stage--;
					break;
				}
			}

			else if (key == 80) {		//����Ű �Ʒ�
				switch (stage) {
				case 0:
					gotoxy(24, 8);
					printf("          ");
					gotoxy(24, 11);
					printf("          ");
					gotoxy(24, 8);
					printf("   1 STAGE");
					gotoxy(24, 11);
					printf("�� 2 STAGE");
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
					printf("�� 3 STAGE");
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
					printf("�� 4 STAGE");
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
					printf("�� 5 STAGE");
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
					printf("�� 5 STAGE");
					break;
				}
			}

			else if (key == 13)
				return stage;
		}
	}
}

void SCORE_PRINT(int score) {		//���� ǥ��
	gotoxy(24, 41);
	printf("���� :      ", score);
	gotoxy(24, 41);
	printf("���� : %d", score);
}

void LIFE_PRINT(int life) {			//��� ���� ǥ��
	int i;
	gotoxy(50, 41);
	printf("LIFE :           ");
	gotoxy(50, 41);
	printf("LIFE : ");
	textcolor(252);
	for (i = 0; i < life; i++)
		printf("��");
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
	APPEAR(stage);									//�ε�
	gotoxy(24, 15);
	printf("����� �����մϴ�.\n");
	delay(2000);
	clrscr();
	APPEAR(stage);

	for (i = 0; i < (BRICK_NUM + (stage * 5)); i++) {				//���� ��ǥ ����		
		b[i].leftx = (random(28) + 1) * 2;
		b[i].rightx = b[i].leftx + 7;
		b[i].y = random(18) + 1;

		for (j = 0; j < i; j++)
			if ((b[i].y == b[j].y) && abs(b[i].leftx - b[j].leftx) < 8)
				i--;
	}

	for (i = 0; i < (BRICK_NUM + (stage * 5)); i++) {				//���������� ���� �ʱ���
		gotoxy(b[i].leftx, b[i].y);
		textcolor(random(15) + 240);								//���� ���� ����
		for (j = b[i].leftx; j <= b[i].rightx;) {					//������ ��ǥ�� ���� ����
			if (j == b[i].leftx) {
				printf("[");
				j++;
			}

			else if (j == b[i].rightx) {
				printf("]");
				break;
			}

			else {
				printf("��");
				j += 2;
			}
		}
	}
	textcolor(240);
}

void CREATE_BAR(bar *br) {					//���� ����
	int i;
	textcolor(243);
	gotoxy(br->leftx, br->y);
	for (i = 0; i < 5; i++)
		printf("��");
	textcolor(240);
}

void CREATE_BALL(ball *bl) {				//������
	gotoxy(bl->x, bl->y);
	printf("��");
}

void BAR_MOVE(bar *br, int key) {			//���� ������
	if (key == 75) {						//��
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

	else if (key == 77) {					//��
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

	delay(100 - (stage * 13) + (bl->angle * 20));			//�� �ӵ�
	gotoxy(bl->x, bl->y);
	printf("  ");

	if (bl->x == 64)						//������ ��
		bl->xsw = -1;
	else if (bl->x == 2)					//���� ��
		bl->xsw = 1;

	if (bl->y == 1)							//õ��
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

void BREAK_UP(brick *bk) {					//���� �ı� ó��
	gotoxy(bk->leftx, bk->y);
	printf("        ");

	bk->leftx = 0;
	bk->rightx = 0;
	bk->y = 0;
}

int RESTART_CHECK(int clear) {				//�����
	int select, key = 72;

	if (clear) {
		select = 2;
		gotoxy(20, 15);
		printf("������ Ŭ�����ϼ̽��ϴ�.");
		gotoxy(20, 16);
		printf("���� ���������� �̵��Ͻðڽ��ϱ�?");
		gotoxy(20, 17);
		printf("�� ��    ");
		gotoxy(20, 18);
		printf("   �ƴϿ�");

		while (1) {
			if (kbhit()) {
				key = getch();

				if (key == 72) {
					gotoxy(20, 17);
					printf("         ");
					gotoxy(20, 18);
					printf("         ");
					gotoxy(20, 17);
					printf("�� ��    ");
					gotoxy(20, 18);
					printf("   �ƴϿ�");
					select = 2;
				}

				else if (key == 80) {
					gotoxy(20, 17);
					printf("         ");
					gotoxy(20, 18);
					printf("         ");
					gotoxy(20, 17);
					printf("   ��    ");
					gotoxy(20, 18);
					printf("�� �ƴϿ�");
					select = 0;
				}

				else if (key == 13)
					return select;
			}
		}
	}

	else {								//���� ���� �̺�Ʈ
		select = 1;
		gotoxy(20, 20);
		printf("������ ����Ǿ����ϴ�.");
		gotoxy(20, 21);
		printf("�ٽ� �Ͻðڽ��ϱ�?");
		gotoxy(20, 22);
		printf("�� ��    ");
		gotoxy(20, 23);
		printf("   �ƴϿ�");

		while (1) {
			if (kbhit()) {
				key = getch();

				if (key == 72) {
					gotoxy(20, 22);
					printf("         ");
					gotoxy(20, 23);
					printf("         ");
					gotoxy(20, 22);
					printf("�� ��    ");
					gotoxy(20, 23);
					printf("   �ƴϿ�");
					select = 1;
				}

				else if (key == 80) {
					gotoxy(20, 22);
					printf("         ");
					gotoxy(20, 23);
					printf("         ");
					gotoxy(20, 22);
					printf("   ��    ");
					gotoxy(20, 23);
					printf("�� �ƴϿ�");
					select = 0;
				}

				else if (key == 13)
					return select;
			}
		}
	}
}