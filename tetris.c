#include "tetris.h"

static struct sigaction act, oact;

int main()
{
	int exit = 0;

	if (VISIBLE_BLOCKS < 3)
	{
		nextBlock = malloc(sizeof(int) * 3);
	}
	else
	{
		nextBlock = malloc(sizeof(int) * VISIBLE_BLOCKS);
	}

	binscrTree = malloc(sizeof(binTree));
	binscrTree->len = 0;
	recStart = malloc(sizeof(RecNode));
	recStart->total = 0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));

	while (!exit)
	{
		clear();
		switch (menu())
		{
		case MENU_PLAY:
			play();
			break;
		case MENU_EXIT:
			exit = 1;
			break;
		case RANKING:
			rank();
			break;
		default:
			break;
		}
	}

	freeTree(binscrTree->start);
	free(binscrTree);
	free(nextBlock);
	endwin();
	system("clear");
	return 0;
}

void InitTetris()
{
	int i, j;

	for (j = 0; j < HEIGHT; j++)
		for (i = 0; i < WIDTH; i++)
			field[j][i] = 0;
	if (VISIBLE_BLOCKS < 3)
	{
		nextBlock[0] = rand() % 7;
		nextBlock[1] = rand() % 7;
		nextBlock[2] = rand() % 7;
	}
	else
	{
		for (i = 0; i < VISIBLE_BLOCKS; i++)
		{
			nextBlock[i] = rand() % 7;
		}
	}
	blockRotate = 0;
	blockY = -1;
	blockX = WIDTH / 2 - 2;
	score = 0;
	gameOver = 0;
	timed_out = 0;

	memcpy(recStart->f, field, sizeof(char) * HEIGHT * WIDTH);
	recStart->score = score;
	recommend(recStart, 0, &dummy);

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate, dummy);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline()
{
	int i, j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0, 0, HEIGHT, WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2, WIDTH + 10);
	printw("NEXT BLOCK");
	DrawBox(3, WIDTH + 10, 4, 8);
	DrawBox(10, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(17, WIDTH + 10);
	printw("SCORE");
	DrawBox(18, WIDTH + 10, 1, 8);
}

int GetCommand()
{
	int command;
	command = wgetch(stdscr);
	switch (command)
	{
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ': /* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command)
{
	int ret = 1;
	int drawFlag = 0;
	switch (command)
	{
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if ((drawFlag = CheckToMove(field, nextBlock[0], (blockRotate + 1) % 4, blockY, blockX)))
			blockRotate = (blockRotate + 1) % 4;
		break;
	case KEY_DOWN:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX + 1)))
			blockX++;
		break;
	case KEY_LEFT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX - 1)))
			blockX--;
		break;
	default:
		break;
	}
	if (drawFlag)
		DrawChange(field, command, nextBlock[0], blockRotate, blockY, blockX);
	return ret;
}

void DrawField()
{
	int i, j;
	for (j = 0; j < HEIGHT; j++)
	{
		move(j + 1, 1);
		for (i = 0; i < WIDTH; i++)
		{
			if (field[j][i] == 1)
			{
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else
				printw(".");
		}
	}
}

void PrintScore(int score)
{
	move(19, WIDTH + 11);
	printw("%8d", score);
}

void DrawNextBlock(int *nextBlock)
{
	int i, j;
	for (i = 0; i < BLOCK_HEIGHT; i++)
	{
		move(4 + i, WIDTH + 13);
		for (j = 0; j < BLOCK_WIDTH; j++)
		{
			if (block[nextBlock[1]][0][i][j] == 1)
			{
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else
				printw(" ");
		}
	}
	for (i = 0; i < BLOCK_HEIGHT; i++)
	{
		move(11 + i, WIDTH + 13);
		for (j = 0; j < BLOCK_WIDTH; j++)
		{
			if (block[nextBlock[2]][0][i][j] == 1)
			{
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else
				printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID, int blockRotate, char tile)
{
	int i, j;
	for (i = 0; i < BLOCK_HEIGHT; i++)
	{
		for (j = 0; j < BLOCK_WIDTH; j++)
		{
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0)
			{
				move(i + y + 1, j + x + 1);
				attron(A_REVERSE);
				printw("%c", tile);
				attroff(A_REVERSE);
			}
		}
	}
	move(HEIGHT, WIDTH + 10);
}

void DrawBox(int y, int x, int height, int width)
{
	int i, j;
	move(y, x);
	addch(ACS_ULCORNER);
	for (i = 0; i < width; i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for (j = 0; j < height; j++)
	{
		move(y + j + 1, x);
		addch(ACS_VLINE);
		move(y + j + 1, x + width + 1);
		addch(ACS_VLINE);
	}
	move(y + j + 1, x);
	addch(ACS_LLCORNER);
	for (i = 0; i < width; i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play()
{
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();
	do
	{
		if (timed_out == 0)
		{
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		if (ProcessCommand(command) == QUIT)
		{
			alarm(0);
			DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
			move(HEIGHT / 2, WIDTH / 2 - 4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	} while (!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
	move(HEIGHT / 2, WIDTH / 2 - 4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu()
{
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX)
{
	int final = 0;
	for (int i = 0; i < BLOCK_HEIGHT; i++)
	{
		for (int j = 0; j < BLOCK_WIDTH; j++)
		{
			if (block[currentBlock][blockRotate][i][j] == 1)
			{
				if (i + blockY < HEIGHT && j + blockX < WIDTH && j + blockX > -1)
				{
					if (f[i + blockY][j + blockX] == 0)
						final++;
				}
				else
					return 0;
			}
			if (final == 4)
				return 1;
		}
	}
	return 0;
}

int CheckifInside(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX)
{
	int final = 0;
	int movchk = 0;
	int lowest = 0;
	int i, j;
	for (i = 0; i < BLOCK_HEIGHT; i++)
	{
		if (block[currentBlock][blockRotate][i][0] == 1 || block[currentBlock][blockRotate][i][1] == 1 || block[currentBlock][blockRotate][i][2] == 1 || block[currentBlock][blockRotate][i][3] == 1)
		{
			for (j = 0; j < BLOCK_WIDTH; j++)
			{
				if (block[currentBlock][blockRotate][i + 1][j] == 0)
					lowest++;
				else
					break;
			}
		}
		for (j = 0; j < BLOCK_WIDTH; j++)
		{
			if (block[currentBlock][blockRotate][i][j] == 1)
			{
				if (i + blockY >= HEIGHT || j + blockX >= WIDTH - 1 || j + blockX < 0)
					return 1;
				if (f[i + blockY][j + blockX] == 1)
					return 1;
				if (f[i + blockY][j + blockX] == 0)
					final++;
				if (f[i + blockY + 1][j + blockX] == 1 || (lowest == 4 && i + blockY == HEIGHT - 1))
					movchk++;
			}
			if (final == 4 && movchk > 0)
				return 0;
		}
		movchk = 0;
		lowest = 0;
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX)
{
	int obY, obX, orot;
	orot = blockRotate;
	obY = blockY;
	obX = blockX;
	switch (command)
	{
	case KEY_UP:
		orot = blockRotate - 1;
		break;
	case KEY_DOWN:
		obY = blockY - 1;
		break;
	case KEY_RIGHT:
		obX = blockX - 1;
		break;
	case KEY_LEFT:
		obX = blockX + 1;
		break;
	default:
		break;
	}
	for (int i = 0; i < BLOCK_HEIGHT; i++)
	{
		for (int j = 0; j < BLOCK_WIDTH; j++)
		{
			if (block[currentBlock][orot][i][j] == 1 && field[i + obY][j + obX] == 1)
				field[i + obY][j + obX] = 0;
		}
	}
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate, dummy);
	move(HEIGHT, WIDTH + 10);
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다.
}
void BlockDown(int sig)
{
	int i;
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX) == 1)
	{
		blockY++;
		DrawField();
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate, dummy);
		alarm(1);
		return;
	}
	else if (blockY == -1)
	{
		gameOver = 1;
		alarm(1);
		return;
	}
	else
	{
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);
		PrintScore(score);
		if (VISIBLE_BLOCKS < 3)
		{
			nextBlock[0] = nextBlock[1];
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand() % 7;
		}
		else
		{
			for (i = 0; i < VISIBLE_BLOCKS - 1; i++)
			{
				nextBlock[i] = nextBlock[i + 1];
			}
		}
		nextBlock[i] = rand() % 7;
		DrawNextBlock(nextBlock);
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;
		memcpy(recStart->f, field, sizeof(char) * HEIGHT * WIDTH);
		recStart->score = score;
		recommend(recStart, 0, &dummy);
		DrawField();
		alarm(1);
	} //강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX)
{
	// user code
	int score = 0;
	for (int i = 0; i < BLOCK_HEIGHT; i++)
	{
		for (int j = 0; j < BLOCK_WIDTH; j++)
		{
			if (block[currentBlock][blockRotate][i][j] == 1)
			{
				f[blockY + i][blockX + j] = 1;
				if (blockY + i == 21)
					score++;
				if (blockY + i + 1 <= HEIGHT)
				{
					if (f[blockY + i + 1][blockX + j] == 1)
						score++;
				}
			}
		}
	}
	return score * 10;
	//Block이 추가된 영역의 필드값을 바꾼다.
}

int DeleteLine(char f[HEIGHT][WIDTH])
{
	// user code
	int temp = 0;
	int flag = 0;
	int r = 0;
	for (int i = 0; i <= HEIGHT - 1; i++)
	{
		for (int j = 0; j <= WIDTH - 1; j++)
		{
			if (f[i][j] == 1)
				temp++;
			else
			{
				temp = 0;
				break;
			}
			if (temp == WIDTH)
			{
				flag = 1;
				r++;
			}
		}
		if (flag == 1)
		{
			temp = 0;
			int k = i;
			for (k; k > 0; k--)
			{
				for (int l = 0; l <= WIDTH - 1; l++)
				{
					if (f[k - 1][l] == 1)
						f[k][l] = 1;
					else
						f[k][l] = 0;
				}
			}
			flag = 0;
		}
	}
	return r * r * 100;
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID, int blockRotate)
{
	while (CheckToMove(field, blockID, blockRotate, y + 1, x) == 1)
		y++;
	DrawBlock(y, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate, RecNode *dummy)
{
	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
	DrawRecommend(dummy);
}

void createRankList()
{
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	int i;

	//1. 파일 열기
	fp = fopen("rank.txt", "r");

	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	char nameTemp[NAMELEN + 1];
	int scoreTemp;
	fscanf(fp, "%d", &i);
	while (fscanf(fp, "%s %d", nameTemp, &scoreTemp) != EOF)
	{
		treeInput(nameTemp, scoreTemp);
	}
	// 4. 파일닫기
	fclose(fp);
}

void rank()
{
	createRankList();
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X = 0, Y = 0, ch, i, j, test = 0;
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1')
	{
		int counter = 0;
		echo();
		printw("X: ");
		scanw("%d", &X);
		printw("Y: ");
		scanw("%d", &Y);
		noecho();
		printw("      name	|     score	\n");
		printw("________________________________\n");
		if (X <= Y && X != 0 && Y != 0)
		{
			in(binscrTree->start, X, Y, &counter, &test);
			if (test == 0)
				printw("search failure: no rank in the list");
		}
		else if (X != 0 && Y == 0)
		{
			in(binscrTree->start, X, binscrTree->len, &counter, &test);
			if (test == 0)
				printw("search failure: no rank in the list");
		}
		else if (X == 0 && Y != 0)
		{
			in(binscrTree->start, 1, Y, &counter, &test);
			if (test == 0)
				printw("search failure: no rank in the list");
		}
		else if (X == 0 && Y == 0)
		{
			in(binscrTree->start, 1, binscrTree->len, &counter, &test);
			if (test == 0)
				printw("search failure: no rank in the list");
		}
		else
		{
			printw("search failure: no rank in the list");
		}
	}
	else if (ch == '2')
	{
		char str[NAMELEN + 1];
		echo();
		printw("Input the name: ");
		getstr(str);
		noecho();
		printw("      name	|     score	\n");
		printw("________________________________\n");
		inSearch(binscrTree->start, str, &test);
		if (test == 0)
			printw("search failure: no rank in the list");
	}
	else if (ch == '3')
	{
		int num, counter = 0, test = 0;
		echo();
		printw("Input the rank: ");
		scanw("%d", &num);
		noecho();
		inDelete(binscrTree->start, num, &counter, &test);
		if (test == 0)
			printw("search failure: no rank in the list");
	}
	getch();
	freeTree(binscrTree->start);
	binscrTree->start = NULL;
	binscrTree->len = 0;
}

void writeRankFile()
{
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int exit;
	//1. "rank.txt" 연다
	FILE *fp = fopen("rank.txt", "w");

	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", binscrTree->len);

	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료

	inAll(binscrTree->start, fp);
	fclose(fp);

	//freeTree(binscrTree->start);
	//binscrTree->start = NULL;
	//binscrTree->len = 0;
}

void newRank(int score)
{
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN + 1];
	int i, j;
	clear();
	//1. 사용자 이름을 입력받음
	printw("your name: ");
	echo();
	getstr(str);
	noecho();
	if (access("rank.txt", F_OK) != -1)
		createRankList();
	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	treeInput(str, score);
	writeRankFile();
}

void recommend(RecNode *root, int count, RecNode **maxN)
{
	if (count < VISIBLE_BLOCKS)
	{
		if (count == 0)
			*maxN = NULL;
		int maxH = HEIGHT - 5, max = 0;
		int i, j, k, rot;
		RecNode *maxNode;
		if (*maxN != NULL)
		{
			maxNode = *maxN;
			max = maxNode->score;
		}
		int tracker = 0;
		for (i = 0; i < HEIGHT; i++)
		{
			for (j = 0; j < WIDTH; j++)
			{
				if ((root->f)[i][j] == 1)
				{
					maxH = i - BLOCK_HEIGHT;
					tracker = 1;
					break;
				}
			}
			if (tracker == 1)
				break;
		}
		tracker = 0;
		switch (nextBlock[count])
		{
		case 0:
			rot = 2;
			break;
		case 4:
			rot = 1;
			break;
		case 5:
			rot = 2;
			break;
		case 6:
			rot = 2;
			break;
		default:
			rot = 4;
			break;
		}
		for (i = maxH; i < 21; i++)
		{
			for (k = 0; k < rot; k++)
			{
				for (j = -2; j < 9; j++)
				{
					if (tracker > CHILDREN_MAX)
						break;
					if (CheckifInside(root->f, nextBlock[count], k, i, j) == 0)
					{
						if (root->total == 0 || tracker >= root->total)
						{
							RecNode *newNode = malloc(sizeof(RecNode));
							memcpy(newNode->f, root->f, sizeof(char) * HEIGHT * WIDTH);
							newNode->curBlockID = nextBlock[count];
							newNode->recBlockX = j;
							newNode->recBlockY = i;
							newNode->recBlockRotate = k;
							newNode->score = root->score + AddBlockToField(newNode->f, nextBlock[count], k, i, j);
							newNode->score += DeleteLine(newNode->f);
							newNode->lv = count + 1;
							newNode->parent = root;
							newNode->total = 0;
							(root->c)[tracker] = newNode;
						}
						else
						{
							memcpy((root->c)[tracker]->f, root->f, sizeof(char) * HEIGHT * WIDTH);
							(root->c)[tracker]->curBlockID = nextBlock[count];
							(root->c)[tracker]->recBlockX = j;
							(root->c)[tracker]->recBlockY = i;
							(root->c)[tracker]->recBlockRotate = k;
							(root->c)[tracker]->score = root->score + AddBlockToField((root->c)[tracker]->f, nextBlock[count], k, i, j);
							(root->c)[tracker]->score += DeleteLine((root->c)[tracker]->f);
							(root->c)[tracker]->lv = count + 1;
							(root->c)[tracker]->total = 0;
							(root->c)[tracker]->parent = root;
						}
						if ((root->c)[tracker]->score > max && count == VISIBLE_BLOCKS - 1)
						{
							max = (root->c)[tracker]->score;
							maxNode = (root->c)[tracker];
						}
						tracker++;
					}
				}
			}
		}
		if (count == VISIBLE_BLOCKS - 1)
			*maxN = maxNode;
		root->total = tracker;
		for (i = 0; i < tracker; i++)
			recommend((root->c)[i], count + 1, maxN);
	}
	else
		return;
}

void DrawRecommend(RecNode *printNode)
{
	for (int i = 1; i < VISIBLE_BLOCKS; i++)
		printNode = printNode->parent;
	DrawBlock(printNode->recBlockY, printNode->recBlockX, printNode->curBlockID, printNode->recBlockRotate, 'R');
}

void recommendedPlay()
{
	// user code
}

void in(treeNode *root, int X, int Y, int *counter, int *test)
{
	if (root == NULL)
		return;
	in(root->right_child, X, Y, counter, test);
	(*counter)++;
	if (*counter >= X && *counter <= Y)
	{
		printw("%-15s |%10d\n", root->name, root->data);
		*test = 1;
	}
	if (root->repeat != NULL)
		in(root->repeat, X, Y, counter, test);
	in(root->left_child, X, Y, counter, test);
}

void inSearch(treeNode *root, char *name, int *test)
{
	if (root == NULL)
		return;
	inSearch(root->right_child, name, test);
	if (strcmp(name, root->name) == 0)
	{
		*test = 1;
		printw("%-15s |%10d\n", root->name, root->data);
	}
	if (root->repeat != NULL)
		inSearch(root->repeat, name, test);
	inSearch(root->left_child, name, test);
}

void inDelete(treeNode *root, int rank, int *counter, int *test)
{
	if (root == NULL)
		return;
	inDelete(root->right_child, rank, counter, test);
	(*counter)++;
	if (*counter == rank)
	{
		treeNode *temp;
		if (root->repeat != NULL || root->repeatI == 1)
		{
			if (root->parent->data != root->data)
			{
				temp = root->parent;
				int te = root->data;
				root->repeat->repeatI = 0;
				root->repeat->parent = temp;
				if (root->right_child != NULL)
					root->repeat->right_child = root->right_child;
				if (root->left_child != NULL)
					root->repeat->left_child = root->left_child;
				if (temp->data > te)
					temp->left_child = root->repeat;
				else
					temp->right_child = root->repeat;
				free(root->name);
				free(root);
			}
			else if (root->repeat != NULL)
			{
				temp = root->parent;
				root->repeat->parent = temp;
				temp->repeat = root->repeat;
				free(root->name);
				free(root);
			}
			else
			{
				temp = root->parent;
				temp->repeat = NULL;
				free(root->name);
				free(root);
			}
		}
		else if (root->right_child == NULL && root->left_child == NULL)
		{
			temp = root->parent;
			int te = root->data;
			if (temp->data > te)
				temp->left_child = NULL;
			else
				temp->right_child = NULL;
			free(root->name);
			free(root);
		}
		else if (root->right_child != NULL && root->left_child == NULL)
		{
			temp = root->right_child;
			root->data = root->right_child->data;
			strcpy(root->name, root->right_child->name);
			root->right_child = root->right_child->right_child;
			free(temp->name);
			free(temp);
		}
		else if (root->right_child == NULL && root->left_child != NULL)
		{
			temp = root->left_child;
			root->data = root->left_child->data;
			strcpy(root->name, root->left_child->name);
			root->left_child = root->left_child->left_child;
			free(temp->name);
			free(temp);
		}
		else if (root->right_child != NULL && root->left_child != NULL)
		{
			temp = inLeft(root->right_child);
			root->data = temp->data;
			strcpy(root->name, temp->name);
			if (temp->right_child != NULL)
			{
				temp->right_child->parent = temp->parent;
				temp->parent = temp->right_child;
				free(temp->name);
				free(temp);
			}
			else if (temp->right_child == NULL)
			{
				temp = temp->parent;
				free(temp->right_child->name);
				free(temp->right_child);
				temp->right_child = NULL;
			}
		}
		*test = 1;
		if (*test == 1)
		{
			printw("result: the rank deleted");
			binscrTree->len--;
			writeRankFile();
		}
		return;
	}
	if (root->repeat != NULL)
		inDelete(root->repeat, rank, counter, test);
	inDelete(root->left_child, rank, counter, test);
}

treeNode *inLeft(treeNode *root)
{
	if (root->left_child == NULL)
		return root;
	inLeft(root->left_child);
}

void inAll(treeNode *root, FILE *fp)
{
	if (root == NULL)
		return;
	inAll(root->right_child, fp);
	fprintf(fp, "%s %d\n", root->name, root->data);
	if (root->repeat != NULL)
		inAll(root->repeat, fp);
	inAll(root->left_child, fp);
}

void freeTree(treeNode *root)
{
	if (root == NULL)
		return;
	freeTree(root->right_child);
	free(root->name);
	if (root->repeat != NULL)
		freeTree(root->repeat);
	freeTree(root->left_child);
	free(root);
}

void treeInput(char *nameTemp, int scoreTemp)
{
	treeNode *temp = malloc(sizeof(treeNode));
	treeNode *traverse;
	int doneflag = 0;
	int exitflag = 0;
	temp->data = scoreTemp;
	temp->name = malloc(sizeof(char) * strlen(nameTemp));
	temp->repeat = NULL;
	temp->left_child = NULL;
	temp->right_child = NULL;
	temp->parent = NULL;
	strcpy(temp->name, nameTemp);
	if (binscrTree->len == 0)
		binscrTree->start = temp;
	else
	{
		traverse = binscrTree->start;
		while (doneflag == 0)
		{
			if (scoreTemp == traverse->data)
			{
				temp->repeatI = 1;
				while (traverse->repeat != NULL)
					traverse = traverse->repeat;
				temp->parent = traverse;
				traverse->repeat = temp;
				doneflag = 1;
			}
			else if (scoreTemp > traverse->data)
			{
				temp->repeatI = 0;
				if (traverse->right_child == NULL)
				{
					temp->parent = traverse;
					traverse->right_child = temp;
					doneflag = 1;
				}
				else
				{
					traverse = traverse->right_child;
					while (1)
					{
						if (scoreTemp <= traverse->data || traverse->right_child == NULL)
						{
							exitflag = 1;
							break;
						}
						else if (scoreTemp > traverse->data)
							traverse = traverse->right_child;
					}
					if (exitflag == 1)
					{
						exitflag = 0;
						continue;
					}
				}
			}
			else if (scoreTemp < traverse->data)
			{
				temp->repeatI = 0;
				if (traverse->left_child == NULL)
				{
					temp->parent = traverse;
					traverse->left_child = temp;
					doneflag = 1;
				}
				else
				{
					traverse = traverse->left_child;
					while (1)
					{
						if (scoreTemp >= traverse->data || traverse->left_child == NULL)
						{
							exitflag = 1;
							break;
						}
						else if (scoreTemp < traverse->data)
							traverse = traverse->left_child;
					}
					if (exitflag == 1)
					{
						exitflag = 0;
						continue;
					}
				}
			}
		}
	}
	binscrTree->len++;
}