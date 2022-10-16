#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>


using namespace std;
bool n = false;
int p = 60, q = 20;
int k = 0;
string map[22];
ofstream file("xy.txt");
struct SNAKE{
	char pixel = '*';
	int x = 1, y = 1;
	int direction = 1; //1是向前，-1是向后，2是向下，-2是向上
	int odirection = 1;
	SNAKE * next;
};

void initialize();
void append(SNAKE * head, int x); //添加尾巴
void snake(SNAKE * &head, int length); //初始化蛇
void printphoto(SNAKE * head); //打印棋盘一次
void print(SNAKE * head); //打印棋盘，先清屏
void printhead(SNAKE * head); //打印linklist
void add(SNAKE * head);

SNAKE * ntol(SNAKE * head, int n); //数字n指向SNAKE
SNAKE * last(SNAKE * head, SNAKE * now); //上一个snake

void run(SNAKE * p, bool b); //左右走
void turn(SNAKE * p, bool b); //上下走
void move(SNAKE * p); //一起走


void movehead(SNAKE * head, int dx, int dy); //头动
void follow(SNAKE * head, SNAKE * now); //追踪上一个像素块
void moveall(SNAKE * head, int dx, int dy); //全部一起动
void flow(SNAKE * head, int dx, int dy); //跑一次

void work(SNAKE * head);//整个流程

void Delay(int time); //延迟
int get(); //无阻塞读取
int kbhit(void); //检查是否按键盘



int main(int argc, char *argv[]) {
	SNAKE * head = NULL;
	snake(head, 9);
	
	//printhead(head);
	work(head);
}

void initialize(){
	map[0] = "--------------------------------------------------------------\n";
	map[21] = map[0];
	for ( int i = 1; i < 21; i++ )
		map[i] = "|                                                            |\n";
	map[q][p] = '#';
}

void printhead(SNAKE * head){
	int n = 1;
	SNAKE * curr = head;
	while ( curr != NULL ){
		curr = curr->next;
	}
	while ( n <= 3 ){
		file << n << ": " << endl;
		file << "X :" << last(head, curr)->x << endl;
		file << "Y: " << last(head, curr)->y << endl;
		file << "D: " << last(head, curr)->direction << endl;
		file << "OD: " << last(head, curr)->odirection << endl;
		curr = last(head,curr);
		n++;
	}
	file << "\n\n";
}



int kbhit(void){
	struct termios oldt, newt;
	int ch;
	int oldf;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	if(ch != EOF){
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}


int get(){
	struct termios stored_settings;
	struct termios new_settings;
	tcgetattr (0, &stored_settings);// 得到原来的终端属性
	new_settings = stored_settings;
	new_settings.c_lflag &= (~ICANON);//设置非正规模式，程序每次要从终端读取一个字符的
	new_settings.c_cc[VTIME] = 0;//设置非正规模式下的读延时
	new_settings.c_cc[VMIN] = 1;// 设置非正规模式下的最小字符数
	tcsetattr (0, TCSANOW, &new_settings);// 设置新的终端属性
	//然后就可以读取了
	int a;
	a = getchar();
	tcsetattr (0, TCSANOW, &stored_settings);// 恢复默认设置
	//读完记得恢复默认设置，不然。。。。。。
	return a;
	system("clear");
}


void Delay(int time){
	clock_t now = clock();
	
	while( clock() - now < time );
}


void snake(SNAKE * &head, int length){
	SNAKE * p = new SNAKE;
	p->x = length;
	p->next = NULL;
	head = p;
	SNAKE * curr = head;
	for ( int i = length - 1; i > 0; i-- ){
		append(head, i);
	}
}


void append(SNAKE * head, int x){
	while ( head->next != NULL )
		head = head->next;
	SNAKE * p = new SNAKE;
	p->x = x;
	head->next = p;
	head->next->next = NULL;
}

void add(SNAKE * head){
	SNAKE * curr = head, * prev;
	while ( curr->next != NULL )
		curr = curr->next;
	prev = last(head,curr);
	int x0 = prev->x, y0 = prev->y, x1 = curr->x, y1 = curr->y;
	SNAKE * tail = new SNAKE;
	
	tail->x = 2 * x1 - x0;
	tail->y = 2 * y1 - y0;
	tail->direction = curr->direction;
	
	if ( x1 == 1 && x0 == 2 ){
		tail->x = 60;
		tail->direction = 1;
	}
	else if ( x1 == 60 && x0 == 59 ){
		tail->x = 1;
		tail->direction = -1;
	}
	else if ( x1 == 1 && x0 == 60 ){
		tail->x = 2;
		tail->direction = -1;
	}
	else if ( x1 == 60 && x0 == 1 ){
		tail->x = 59;
		tail->direction = 1;
	}
	
	if ( y1 == 1 && y0 == 2 ){
		tail->y = 20;
		tail->direction = 2;
	}
	else if ( y1 == 20 && y0 == 19 ){
		tail->y = 1;
		tail->direction = -2;
	}
	else if ( y1 == 1 && y0 == 20 ){
		tail->y = 2;
		tail->direction = -2;
	}
	else if ( y1 == 20 && y0 == 1 ){
		tail->y = 19;
		tail->direction = 2;
	}

	tail->next = NULL;
	
	//tail->odirection = tail->direction;
	curr->next = tail;
}


void printphoto(SNAKE * head){
	initialize();
	SNAKE * curr = head;
	while ( curr != NULL ){
		map[curr->y][curr->x] = curr->pixel;
		curr = curr->next;
	}
	for ( int i = 0; i < 22; i++ ){
		cout << map[i];
		file << map[i];
	}
}


void print(SNAKE * head){
	Delay(60000);
	system("clear");
	printphoto(head);
}


SNAKE * ntol(SNAKE * head, int n){
	int i = 1;
	while ( i <= n ){
		if ( i == n )
			return head;
		head = head->next;
		i++;
	}
	return head;
}


SNAKE * last(SNAKE * head, SNAKE * now){
	while ( head->next != now )
		head = head->next;
	return head;
}


void run(SNAKE * p, bool b){
	if ( b )
		p->x++;
	else
		p->x--;
	if ( p->x == 0 )
		p->x = 60;
	else if ( p->x == 61 )
		p->x = 1;
}


void turn(SNAKE * p, bool b){
	if ( b )
		p->y++;
	else
		p->y--;
	if ( p->y == 0 ){
		p->y = 20;
	}
	else if ( p->y == 21 )
		p->y = 1;
}


void move(SNAKE * p){
	int direction = p->direction;
	bool x = false;
	if ( direction > 0 ) x = true;
	if ( abs(direction) == 1 )
		run(p, x);
	else
		turn(p,x);
}


void movehead(SNAKE * head, int dx, int dy){
	bool b1, b2;
	if ( dx == 0 && dy == 0 ) return;
	head->odirection = head->direction;
	if ( dx != 0 ){
		if ( dx > 0 )
			head->direction = 1;
		else
			head->direction = -1;
	}
	else if ( dy != 0 ){
		if ( dy > 0 )
			head->direction = 2;
		else
			head->direction = -2;
	}
	move(head);
}


void follow(SNAKE * head, SNAKE * now){
	SNAKE * prev = last(head,now);
	now->odirection = now->direction;
	now->direction = prev->odirection;
	if ( now->next == NULL ) printhead(head);
	move(now);
}


void moveall(SNAKE * head, int dx, int dy){
	movehead(head, dx, dy);
	if ( head->x == p && head->y == q ) {
		add(head);
		srand(time(NULL));
		p = rand() % 60 + 1, q = rand() % 20 + 1;
	}
	SNAKE * curr = head->next;
	while ( curr != NULL ){
		follow(head, curr);
		curr = curr->next;
	}
}


void flow(SNAKE * head, int dx, int dy){
	for (int i = 0; i < abs(dx); i++ ){
		moveall(head, abs(dx)/dx, 0);
		print(head);
	}
	for (int i = 0; i < abs(dy); i++ ){
		moveall(head, 0, abs(dy)/dy);
		print(head);
	}
}


void work(SNAKE * head){
	
	print(head);
	while (1){
		int a = get();
		
		if ( a == 119 ){
			//up
			while ( !kbhit() ){
				moveall(head, 0, -1);
				print(head);
				k++;
				if ( k % 200 == 0 ){
					srand(time(NULL));
					p = rand() % 60 + 1, q = rand() % 20 + 1;
				}
			}
		}
		else if ( a == 97 ){
			//left
			while ( !kbhit() ){
				moveall(head, -1, 0);
				print(head);
				//printhead(head);
				k++;
				if ( k % 200 == 0 ){
					srand(time(NULL));
					p = rand() % 60 + 1, q = rand() % 20 + 1;
				}
			}
		}
		else if ( a == 115 ){
			// down
			while ( !kbhit() ){
				moveall(head, 0, 1);
				print(head);
				//printhead(head);
				k++;
				if ( k % 200 == 0 ){
					srand(time(NULL));
					p = rand() % 60 + 1, q = rand() % 20 + 1;
				}
			}
		}
		else if ( a == 100 ){
			//right
			while ( !kbhit()  ){
				moveall(head, 1, 0);
				print(head);
				//printhead(head);
				k++;
				if ( k % 200 == 0 ){
					srand(time(NULL));
					p = rand() % 60 + 1, q = rand() % 20 + 1;
				}
			}
		}
	}
}