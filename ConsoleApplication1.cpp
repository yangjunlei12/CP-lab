// ljwvs.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

int position(char *aim);
void enter_value();
void enter_name(char* aim, int type);
void fun_backpatch(int nowadd, int label);
int minDFA();
void getch();
void destory_code();
void destroy();


#define TABLE_LENTH 33
#define TABLE_WIDTH 20
#define PROCESS_LENTH 51
#define STACK_LENTH 100


#define REL_LENTH 6
#define RESERVED_LENTH 10	
#define OP_LENTH 9		

#define MAXSTRLEN 100   //最长字符长度
#define MAXNUMLEN 30	//最长数字长度



#define PROCEDURE 0
#define LEFT_1 1
#define COSNT_INSTRUCT 2
#define LEFT_2 3
#define CONST_DEF 4
#define UNSIGNED_INT 5
#define LEFT_3 6
#define NUMBER 7
#define CHAR_AZ 8
#define IDENT 9
#define LEFT_4 10
#define VARIABLE_INSTRUCT 11
#define LEFT_5 12
#define SENTENCE 13
#define ASSIGNMENT 14
#define EXPRESSION 15
#define TEMP4 16
#define LEFT_6 17
#define TEMP2 18
#define ITEM 19
#define LEFT_7 20
#define TEMP3 21
#define FACTOR 22
#define ADD_OPERATION 23
#define MUL_OPERATION 24
#define IF_CODITION 25
#define LEFT_8 26
#define CONDITION 27
#define REL_OPERATION 28
#define LOOP 29
#define COMPOUND 30
#define LEFT_9 31
#define EMPTY 32

/*单词的种别*/
#define  SYM_NUMBER 0	//常数
#define  SYM_IDENT 1	//标识符
#define  SYM_LBRACKET 2	//左括弧
#define  SYM_RBRACKEET 3	//右括弧
#define  SYM_PLUS_MINS 4  // + | -
#define  SYM_MUL_DIV 5    // * | /
#define  SYM_RELOP 6	//关系操作符
#define  SYM_IF 7      //if
#define  SYM_THEN 8		//then
#define  SYM_ELSE 9		//else
#define  SYM_WHILE 10	//while
#define  SYM_DO 11		//do
#define  SYM_BEGIN 12	//begin
#define  SYM_END 13		//edn
#define  SYM_SEMICOLON 14	//;
#define  SYM_COMMA 15		//,
#define  SYM_A 16		//=
#define  SYM_CONST 17	//const
#define  SYM_VAR 18		//var
#define  SYM_DOLLAR 19	//$

/*语义处理需要的结构*/
/*2018.12.16*/
#define TYPE_CONST 0	//常量类型
#define TYPE_VAR 1		//变量类型
#define SIGN_TABLE_LENTH 20		
/*中间代码需要的*/

#define CODE_LEN 100
#define MAC_CODE_LEN 30



/*打印非终结符*/
char *printNonterminal[] = { "PROCEDURE","LEFT_1","COSNT_INSTRUCT","LEFT_2","CONST_DEF",
"UNSIGNED_INT","LEFT_3","NUMBER","CHAR_AZ","IDENT",
"LEFT_4","VARIABLE_INSTRUCT","LEFT_5","SENTENCE","ASSIGNMENT",
"EXPRESSION","TEMP4","LEFT_6","TEMP2","ITEM",
"LEFT_7","TEMP3","FACTOR","ADD_OPERATION","MUL_OPERATION",
"IF_CODITION","LEFT_8","CONDITION","REL_OPERATION","LOOP",
"COMPOUND","LEFT_9","EMPTY" };

/*打印终结符*/
char *printTerminal[] = { "SYM_NUMBER","SYM_IDENT","SYM_LBRACKET","SYM_RBRACKEET","SYM_PLUS_MINS",
"SYM_MUL_DIV","SYM_RELOP","SYM_IF","SYM_THEN","SYM_ELSE",
"SYM_WHILE","SYM_DO","SYM_BEGIN","SYM_END","SYM_SEMICOLON",
"SYM_COMMA","SYM_A","SYM_CONST","SYM_VAR","SYM_DOLLAR" };




struct Stack
{
	int stack[STACK_LENTH];
	int now;
};


struct Node
{
	int non_terminal;
	int flag;				
	struct Node *next;
};

typedef struct signTable			//符号表
{
	char* sign[SIGN_TABLE_LENTH];
	int type[SIGN_TABLE_LENTH];		//0表示const，1表示var
	int value[SIGN_TABLE_LENTH];
	int off[SIGN_TABLE_LENTH];
	int count;					//计数一共有几个变量了
	int offset;					//下一个变量在符号表中的偏移量
}signTable;


/*中间代码需要的*/
struct sema_stack
{
	char *stack[STACK_LENTH];
	int now;
};

struct Code
{
	char* midcode[CODE_LEN];		//记录中间代码
	int now;

};

/*所有关系字符，运算字符，和保留字*/
char* rel_pun[] = { " ","==","<=","<",">",">=","<>" };
char op_pun[] = { ' ','+','-','*','/',';',',','(',')','=' };
char* reserved[] = { " ","while","do","const","var","begin","end","if","else","then","$" };



int AnalyseTable[TABLE_LENTH][TABLE_WIDTH];//预测分析表
struct Node *Production[PROCESS_LENTH];
struct Stack signStack;
struct Stack flagStack;
struct Stack levelStack;
						
int count = 0;
char ch = ' ';
char buffer[MAXSTRLEN];
/*画图时计数每一层已经有的节点数目*/
int nodeCount[MAXSTRLEN] = { 0 };
/*符号表*/
signTable* table;


/*存放当前的字符串,value存放值---minDFA(用到)*/
char a[SIGN_TABLE_LENTH];
int value;



/*中间代码需要的*/

int templemet_count = 0;					//记录临时变量的编号（在产生临时变量的时候用到）
struct sema_stack semanteme_signStack;		//存放符号栈
struct sema_stack semanteme_tempvarStack;//存放变量栈
struct Code code;

/*控制语句以及布尔表达式修改*/

int  newlable = 0;
int backpatch[CODE_LEN];



void initStack(struct Stack* ptrStack) 
{
	ptrStack->now = 0;
}



int pop(struct Stack* ptrStack) 
{
	if (ptrStack->now <= 0) 
	{
		printf("Stack pop() error:\tthe Stack is empty...\n");
		return -1;
	}
	(ptrStack->now)--;
	return ptrStack->stack[ptrStack->now];
}


int push(struct Stack *ptrStack, int ele) {
	if (ptrStack->now >= STACK_LENTH) {
		printf("Stack push() error:\t the Stack is full...\n");
		return 0;
	}

	ptrStack->stack[ptrStack->now] = ele;
	ptrStack->now++;
	return 1;

}



int isEmpty(struct Stack *ptrStack) {
	if (ptrStack->now == 0) {
		return 1;
	}
	return 0;
}



int topEle(struct Stack *ptrStack) {
	if (isEmpty(ptrStack)) {
		return -1;
	}

	int index;
	index = ptrStack->now - 1;
	return ptrStack->stack[index];
}



/*中间代码*/
void init_sema_Stack(struct sema_stack* ptrStack) 
{
	ptrStack->now = 0;
}

char* sema_pop(struct sema_stack* ptrStack) 
{
	if (ptrStack->now <= 0) 
	{
		printf("sem_Stack pop() error:\tthe Stack is empty...\n");
		return NULL;
	}
	(ptrStack->now)--;
	return ptrStack->stack[ptrStack->now];
}


int sema_push(struct sema_stack *ptrStack, char* ele) {
	if (ptrStack->now >= STACK_LENTH) {
		printf("sem_Stack push() error:\t the Stack is full...\n");
		return 0;
	}

	ptrStack->stack[ptrStack->now] = ele;
	ptrStack->now++;
	return 1;

}

int sema_isEmpty(struct sema_stack *ptrStack) {
	if (ptrStack->now == 0) {
		return 1;
	}
	return 0;
}

char* sema_topEle(struct sema_stack *ptrStack) {
	if (sema_isEmpty(ptrStack)) {
		return NULL;
	}

	int index;
	index = ptrStack->now - 1;
	return ptrStack->stack[index];
}



/*初始化分析表*/
void initAnalyseTable() {
	int i, j;
	for (i = 0; i<TABLE_LENTH; i++) {
		for (j = 0; j<TABLE_WIDTH; j++) {
			AnalyseTable[i][j] = -1;
		}
	}

	AnalyseTable[PROCEDURE][SYM_IDENT] = 0;
	AnalyseTable[PROCEDURE][SYM_IF] = 0;
	AnalyseTable[PROCEDURE][SYM_WHILE] = 0;
	AnalyseTable[PROCEDURE][SYM_BEGIN] = 0;
	AnalyseTable[PROCEDURE][SYM_CONST] = 1;
	AnalyseTable[PROCEDURE][SYM_VAR] = 4;
	AnalyseTable[PROCEDURE][SYM_DOLLAR] = 0;

	AnalyseTable[LEFT_1][SYM_IDENT] = 2;
	AnalyseTable[LEFT_1][SYM_IF] = 2;
	AnalyseTable[LEFT_1][SYM_WHILE] = 2;
	AnalyseTable[LEFT_1][SYM_BEGIN] = 2;
	AnalyseTable[LEFT_1][SYM_VAR] = 3;
	AnalyseTable[LEFT_1][SYM_DOLLAR] = 2;

	AnalyseTable[COSNT_INSTRUCT][SYM_CONST] = 5;

	AnalyseTable[LEFT_2][SYM_SEMICOLON] = 6;
	AnalyseTable[LEFT_2][SYM_COMMA] = 7;

	AnalyseTable[CONST_DEF][SYM_IDENT] = 8;
	AnalyseTable[UNSIGNED_INT][SYM_NUMBER] = 9;

	AnalyseTable[LEFT_3][SYM_NUMBER] = 11;
	AnalyseTable[LEFT_3][SYM_RBRACKEET] = 10;
	AnalyseTable[LEFT_3][SYM_PLUS_MINS] = 10;
	AnalyseTable[LEFT_3][SYM_MUL_DIV] = 10;
	AnalyseTable[LEFT_3][SYM_RELOP] = 10;
	AnalyseTable[LEFT_3][SYM_THEN] = 10;
	AnalyseTable[LEFT_3][SYM_DO] = 10;
	AnalyseTable[LEFT_3][SYM_SEMICOLON] = 10;
	AnalyseTable[LEFT_3][SYM_COMMA] = 10;

	AnalyseTable[NUMBER][SYM_NUMBER] = 12;
	AnalyseTable[CHAR_AZ][SYM_IDENT] = 13;
	AnalyseTable[IDENT][SYM_IDENT] = 14;

	AnalyseTable[LEFT_4][SYM_NUMBER] = 17;
	AnalyseTable[LEFT_4][SYM_IDENT] = 16;
	AnalyseTable[LEFT_4][SYM_RBRACKEET] = 15;
	AnalyseTable[LEFT_4][SYM_PLUS_MINS] = 15;
	AnalyseTable[LEFT_4][SYM_MUL_DIV] = 15;
	AnalyseTable[LEFT_4][SYM_RELOP] = 15;
	AnalyseTable[LEFT_4][SYM_THEN] = 15;
	AnalyseTable[LEFT_4][SYM_DO] = 15;
	AnalyseTable[LEFT_4][SYM_SEMICOLON] = 15;
	AnalyseTable[LEFT_4][SYM_COMMA] = 15;
	AnalyseTable[LEFT_4][SYM_A] = 15;

	AnalyseTable[VARIABLE_INSTRUCT][SYM_VAR] = 18;

	AnalyseTable[LEFT_5][SYM_SEMICOLON] = 19;
	AnalyseTable[LEFT_5][SYM_COMMA] = 20;

	AnalyseTable[SENTENCE][SYM_IDENT] = 21;
	AnalyseTable[SENTENCE][SYM_IF] = 22;
	AnalyseTable[SENTENCE][SYM_ELSE] = 25;
	AnalyseTable[SENTENCE][SYM_WHILE] = 23;
	AnalyseTable[SENTENCE][SYM_BEGIN] = 24;
	AnalyseTable[SENTENCE][SYM_END] = 25;
	AnalyseTable[SENTENCE][SYM_SEMICOLON] = 25;
	AnalyseTable[SENTENCE][SYM_DOLLAR] = 25;

	AnalyseTable[ASSIGNMENT][SYM_IDENT] = 26;

	AnalyseTable[EXPRESSION][SYM_NUMBER] = 27;
	AnalyseTable[EXPRESSION][SYM_IDENT] = 27;
	AnalyseTable[EXPRESSION][SYM_LBRACKET] = 27;
	AnalyseTable[EXPRESSION][SYM_PLUS_MINS] = 27;

	AnalyseTable[TEMP4][SYM_NUMBER] = 28;
	AnalyseTable[TEMP4][SYM_IDENT] = 28;
	AnalyseTable[TEMP4][SYM_LBRACKET] = 28;
	AnalyseTable[TEMP4][SYM_PLUS_MINS] = 29;

	AnalyseTable[LEFT_6][SYM_RBRACKEET] = 30;
	AnalyseTable[LEFT_6][SYM_PLUS_MINS] = 31;
	AnalyseTable[LEFT_6][SYM_RELOP] = 30;
	AnalyseTable[LEFT_6][SYM_THEN] = 30;
	AnalyseTable[LEFT_6][SYM_DO] = 30;
	AnalyseTable[LEFT_6][SYM_SEMICOLON] = 30;

	AnalyseTable[TEMP2][SYM_PLUS_MINS] = 32;

	AnalyseTable[ITEM][SYM_NUMBER] = 33;
	AnalyseTable[ITEM][SYM_IDENT] = 33;
	AnalyseTable[ITEM][SYM_LBRACKET] = 33;

	AnalyseTable[LEFT_7][SYM_RBRACKEET] = 34;
	AnalyseTable[LEFT_7][SYM_PLUS_MINS] = 34;
	AnalyseTable[LEFT_7][SYM_MUL_DIV] = 35;
	AnalyseTable[LEFT_7][SYM_RELOP] = 34;
	AnalyseTable[LEFT_7][SYM_THEN] = 34;
	AnalyseTable[LEFT_7][SYM_DO] = 34;
	AnalyseTable[LEFT_7][SYM_SEMICOLON] = 34;

	AnalyseTable[TEMP3][SYM_MUL_DIV] = 36;

	AnalyseTable[FACTOR][SYM_NUMBER] = 38;
	AnalyseTable[FACTOR][SYM_IDENT] = 37;
	AnalyseTable[FACTOR][SYM_LBRACKET] = 39;

	AnalyseTable[ADD_OPERATION][SYM_PLUS_MINS] = 40;
	AnalyseTable[MUL_OPERATION][SYM_MUL_DIV] = 41;
	AnalyseTable[IF_CODITION][SYM_IF] = 42;

	AnalyseTable[LEFT_8][SYM_ELSE] = 44;
	AnalyseTable[LEFT_8][SYM_END] = 43;
	AnalyseTable[LEFT_8][SYM_SEMICOLON] = 43;
	AnalyseTable[LEFT_8][SYM_DOLLAR] = 43;

	AnalyseTable[CONDITION][SYM_NUMBER] = 45;
	AnalyseTable[CONDITION][SYM_IDENT] = 45;
	AnalyseTable[CONDITION][SYM_LBRACKET] = 45;
	AnalyseTable[CONDITION][SYM_PLUS_MINS] = 45;

	AnalyseTable[REL_OPERATION][SYM_RELOP] = 46;
	AnalyseTable[LOOP][SYM_WHILE] = 47;
	AnalyseTable[COMPOUND][SYM_BEGIN] = 48;

	AnalyseTable[LEFT_9][SYM_END] = 49;
	AnalyseTable[LEFT_9][SYM_SEMICOLON] = 50;


}


/*初始化产生式*/
void initProduction() {
	int i;
	struct Node *temp = NULL;
	for (int i = 0; i<PROCESS_LENTH; i++) {
		Production[i] = (struct Node *)malloc(sizeof(struct Node));
		Production[i]->next = NULL;
		Production[i]->flag = 0;
	}

	
	Production[0]->non_terminal = SENTENCE;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = 0;
	temp->flag = -1;
	temp->next = NULL;
	//Production[0]->next=NULL;


	Production[1]->non_terminal = LEFT_1;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = COSNT_INSTRUCT;
	temp->flag = 0;
	Production[1]->next = temp;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = 0;
	temp->flag = -2;
	temp->next = NULL;

	Production[2]->non_terminal = SENTENCE;
	//Production[2]->next=NULL;

	Production[3]->non_terminal = SENTENCE;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = VARIABLE_INSTRUCT;
	temp->next = NULL;
	temp->flag = 0;
	Production[3]->next = temp;

	Production[4]->non_terminal = SENTENCE;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = VARIABLE_INSTRUCT;
	temp->flag = 0;
	Production[4]->next = temp;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = 0;
	temp->flag = -5;
	temp->next = NULL;

	Production[5]->non_terminal = SYM_SEMICOLON;
	Production[5]->flag = 1;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = LEFT_2;
	temp->flag = 0;
	Production[5]->next = temp;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = CONST_DEF;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_CONST;
	temp->flag = 1;
	temp->next = NULL;

	Production[6]->non_terminal = EMPTY;
	//Production[6]->next=NULL;

	Production[7]->non_terminal = LEFT_2;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = CONST_DEF;
	temp->flag = 0;
	Production[7]->next = temp;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_COMMA;
	temp->flag = 1;
	temp->next = NULL;

	Production[8]->non_terminal = UNSIGNED_INT;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = 1;
	temp->flag = -9;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	Production[8]->next = temp;
	temp = temp->next;
	temp->non_terminal = SYM_A;
	temp->flag = 1;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = IDENT;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = 0;
	temp->flag = -9;
	temp->next = NULL;



	Production[9]->non_terminal = LEFT_3;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[9]->next = temp;
	temp->non_terminal = NUMBER;
	temp->flag = 0;
	temp->next = NULL;

	Production[10]->non_terminal = EMPTY;


	Production[11]->non_terminal = LEFT_3;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = NUMBER;
	temp->flag = 0;
	temp->next = NULL;
	Production[11]->next = temp;

	Production[12]->non_terminal = SYM_NUMBER;
	Production[12]->flag = 1;


	Production[13]->non_terminal = SYM_IDENT;
	Production[13]->flag = 1;

	Production[14]->non_terminal = LEFT_4;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[14]->next = temp;
	temp->non_terminal = CHAR_AZ;
	temp->flag = 0;
	temp->next = NULL;


	Production[15]->non_terminal = EMPTY;

	Production[16]->non_terminal = LEFT_4;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = CHAR_AZ;
	temp->flag = 0;
	temp->next = NULL;
	Production[16]->next = temp;

	Production[17]->non_terminal = LEFT_4;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = NUMBER;
	temp->flag = 0;
	temp->next = NULL;
	Production[17]->next = temp;

	Production[18]->non_terminal = SYM_SEMICOLON;
	Production[18]->flag = 1;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = LEFT_5;
	temp->flag = 0;
	Production[18]->next = temp;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = IDENT;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = 0;
	temp->flag = -19;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_VAR;
	temp->flag = 1;
	temp->next = NULL;

	Production[19]->non_terminal = EMPTY;

	Production[20]->non_terminal = LEFT_5;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = IDENT;
	temp->flag = 0;
	Production[20]->next = temp;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = 0;
	temp->flag = -21;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_COMMA;
	temp->flag = 1;
	temp->next = NULL;

	Production[21]->non_terminal = ASSIGNMENT;
	Production[22]->non_terminal = IF_CODITION;
	Production[23]->non_terminal = LOOP;
	Production[24]->non_terminal = COMPOUND;
	Production[25]->non_terminal = EMPTY;

	Production[26]->non_terminal = 0;
	Production[26]->flag = -27;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[26]->next = temp;
	temp->non_terminal = SYM_SEMICOLON;
	temp->flag = 1;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = EXPRESSION;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_A;
	temp->flag = 1;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = IDENT;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = 1;
	temp->flag = -27;
	temp->next = NULL;

	Production[27]->non_terminal = LEFT_6;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[27]->next = temp;
	temp->non_terminal = 0;
	temp->flag = -28;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = ITEM;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = TEMP4;
	temp->flag = 0;
	temp->next = NULL;

	Production[28]->non_terminal = EMPTY;


	Production[29]->non_terminal = SYM_PLUS_MINS;
	Production[29]->flag = 1;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[29]->next = temp;
	temp->non_terminal = 0;
	temp->flag = -30;
	temp->next = NULL;

	Production[30]->non_terminal = EMPTY;

	Production[31]->non_terminal = LEFT_6;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = TEMP2;
	temp->flag = 0;
	temp->next = NULL;
	Production[31]->next = temp;

	Production[32]->non_terminal = 0;
	Production[32]->flag = -33;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[32]->next = temp;
	temp->non_terminal = ITEM;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = ADD_OPERATION;
	temp->flag = 0;
	temp->next = NULL;


	Production[33]->non_terminal = LEFT_7;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[33]->next = temp;
	temp->non_terminal = FACTOR;
	temp->flag = 0;
	temp->next = NULL;


	Production[34]->non_terminal = EMPTY;

	Production[35]->non_terminal = LEFT_7;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = TEMP3;
	temp->flag = 0;
	temp->next = NULL;
	Production[35]->next = temp;

	Production[36]->non_terminal = 0;
	Production[36]->flag = -37;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[36]->next = temp;
	temp->non_terminal = FACTOR;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = MUL_OPERATION;
	temp->flag = 0;
	temp->next = NULL;


	Production[37]->non_terminal = IDENT;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[37]->next = temp;
	temp->non_terminal = 0;
	temp->flag = -38;
	temp->next = NULL;



	Production[38]->non_terminal = UNSIGNED_INT;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[38]->next = temp;
	temp->non_terminal = 0;
	temp->flag = -39;
	temp->next = NULL;

	Production[39]->non_terminal = SYM_RBRACKEET;
	Production[39]->flag = 1;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = EXPRESSION;
	temp->flag = 0;
	Production[39]->next = temp;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_LBRACKET;
	temp->flag = 1;
	temp->next = NULL;

	Production[40]->non_terminal = SYM_PLUS_MINS;
	Production[40]->flag = 1;
	temp = (struct Node *)malloc(sizeof(struct Node));;
	Production[40]->next = temp;
	temp->non_terminal = 0;
	temp->flag = -41;
	temp->next = NULL;


	Production[41]->non_terminal = SYM_MUL_DIV;
	Production[41]->flag = 1;
	temp = (struct Node *)malloc(sizeof(struct Node));;
	Production[41]->next = temp;
	temp->non_terminal = 0;
	temp->flag = -42;
	temp->next = NULL;

	Production[42]->non_terminal = LEFT_8;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = SENTENCE;
	temp->flag = 0;
	Production[42]->next = temp;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_THEN;
	temp->flag = 1;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = CONDITION;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_IF;
	temp->flag = 1;
	temp->next = NULL;


	Production[43]->non_terminal = EMPTY;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[43]->next = temp;
	temp->non_terminal = 0;
	temp->flag = -44;
	temp->next = NULL;




	Production[44]->non_terminal = 0;
	Production[44]->flag = -45;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[44]->next = temp;
	temp->non_terminal = SENTENCE;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = 1;
	temp->flag = -45;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_ELSE;
	temp->flag = 1;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = 2;
	temp->flag = -45;
	temp->next = NULL;


	Production[45]->non_terminal = 0;
	Production[45]->flag = -46;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[45]->next = temp;
	temp->non_terminal = EXPRESSION;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = REL_OPERATION;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = 1;
	temp->flag = -46;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = EXPRESSION;
	temp->flag = 0;
	temp->next = NULL;

	Production[46]->non_terminal = SYM_RELOP;
	Production[46]->flag = 1;

	Production[47]->non_terminal = 0;
	Production[47]->flag = -48;
	temp = (struct Node *)malloc(sizeof(struct Node));
	Production[47]->next = temp;
	temp->non_terminal = SENTENCE;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_DO;
	temp->flag = 1;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = CONDITION;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_WHILE;
	temp->flag = 1;
	temp->next = NULL;

	Production[48]->non_terminal = SYM_END;
	Production[48]->flag = 1;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = LEFT_9;
	temp->flag = 0;
	Production[48]->next = temp;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SENTENCE;
	temp->flag = 0;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_BEGIN;
	temp->flag = 1;
	temp->next = NULL;

	Production[49]->non_terminal = EMPTY;

	Production[50]->non_terminal = LEFT_9;
	temp = (struct Node *)malloc(sizeof(struct Node));
	temp->non_terminal = SENTENCE;
	temp->flag = 0;
	Production[50]->next = temp;
	temp->next = (struct Node *)malloc(sizeof(struct Node));
	temp = temp->next;
	temp->non_terminal = SYM_SEMICOLON;
	temp->flag = 1;
	temp->next = NULL;

}


void destroy() {
	struct Node *pre, *now;
	int i;
	for (i = 0; i<PROCESS_LENTH; i++) {
		now = Production[i];
		while (now != NULL) {
			pre = now;
			now = now->next;
			free(pre);
		}
	}
	return;
}

/*最后要释放code的数组*/
void destory_code() {
	int amount = code.now;
	for (int i = 0; i<amount; i++) {
		printf("code.midcode[%d],%s\n", i, code.midcode[i]);
		free(code.midcode[i]);
	}
}


void getch() {
	ch = buffer[count++];
	return;
}



/*get the next string's nature*/
int minDFA() {

	int i, j;
	//	enum terminal_symbols_type sym;

	if (ch == '\0') {
		return SYM_DOLLAR;
	}

	while (ch == ' ') {
		getch();
	}
	//printf("ch:%c\n",ch);

	if (isalpha(ch) && islower(ch)) {
		i = 0;
		do {
			if (i<MAXSTRLEN) {
				a[i++] = ch;
			}
			else {
				printf("The string is too long...\n");
			}
			getch();
		} while (isalpha(ch) || isdigit(ch));
		a[i] = '\0';//¼ÓÉÏ½áÊø·û
		reserved[0] = a;
		i = RESERVED_LENTH;
		while (strcmp(a, reserved[i--]));
		switch (++i) {
		case 1:return SYM_WHILE;
		case 2:return SYM_DO;
		case 3:return SYM_CONST;
		case 4:return SYM_VAR;
		case 5:return SYM_BEGIN;
		case 6:return SYM_END;
		case 7:return SYM_IF;
		case 8:return SYM_ELSE;
		case 9:return SYM_THEN;
		case 10:return SYM_DOLLAR;
		default:return SYM_IDENT;
		}

	}
	//ÅÐ¶ÏÊÇ·ñÎªÊý×Ö
	else if (isdigit(ch)) {
		i = 0; j = 0;
		do {
			i = i * 10 + ch - '0';
			getch();
			j++;
			if (j >= MAXNUMLEN) {
				printf("The number is too great....\n");
				break;
			}
		} while (isdigit(ch));
		//printf("The string is number:    %d\n",i );
		value = i;
		return SYM_NUMBER;

	}
	else if (ch == '=') {
		getch();
		if (ch == '=') {
			//printf("The string is op:    ==\n");
			a[0] = '='; a[1] = '='; a[2] = '\0';
			getch();
			return SYM_RELOP;

		}
		else {
			a[0] = '='; a[1] = '\0';
			//printf("The string is op:    =\n");
			return SYM_A;
		}
	}
	else if (ch == '<') {
		a[0] = '<'; a[1] = '\0';
		getch();
		switch (ch) {
		case '=':a[1] = '='; a[2] = '\0'; getch(); break;
		case '>':a[1] = '>'; a[2] = '\0'; getch(); break;
		}
		return SYM_RELOP;
	}
	else if (ch == '>') {
		a[0] = '>'; a[1] = '\0';
		getch();
		if (ch == '=') {
			a[1] = '='; a[2] = '\0';
			getch();
		}
		return SYM_RELOP;
	}
	else {
		//printf(".......................................ch:%c\n",ch);
		a[0] = ch; a[1] = '\0';
		//printf("....................1...................a:%s\n",a);
		op_pun[0] = ch;
		i = OP_LENTH;
		while (ch != op_pun[i--]);

		switch (++i) {
		case 1:
		case 2:getch(); return SYM_PLUS_MINS;
		case 3:
		case 4:getch(); return SYM_MUL_DIV;
		case 5:getch(); return SYM_SEMICOLON;
		case 6:getch(); return SYM_COMMA;
		case 7:getch(); return SYM_LBRACKET;
		case 8:getch(); return SYM_RBRACKEET;
		case 9:getch(); return SYM_A;
		default:return -2;
		}
	}
}



/*语义处理*/
int semanteme(int production_id, int process_id) {
	switch (production_id) {
		//说明部分语义
	case -1:
	case -2:
	case -5:
		//创建一张符号表
		table = (signTable *)malloc(sizeof(signTable));
		table->count = 0;
		table->offset = 0;
		//中间代码的数值为0
		code.now = 0;
		return 0;//表示成功
	case -9:
	{
		switch (process_id) {
		case 0:
			if (position(a) == -1) 
			{						//变量不在符号表中
				enter_name(a, TYPE_CONST);
				return 0;
			}
			else {
				return 1;//表示变量重复定义
			}
		case 1:
			enter_value();
			return 0;
		}
	}
	case -19:
	case -21:
		if (position(a) == -1) {
			enter_name(a, TYPE_VAR);
			return 0;
		}
		else {
			return 1;
		}

		//赋值语句的翻译
	case -27:
	{
		switch (process_id) {
		case 0: {
			//                        printf("the sema -27..0..\n");
			char *temp2 = sema_pop(&semanteme_tempvarStack);
			char *temp1 = sema_pop(&semanteme_tempvarStack);
			char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN);
			strcpy(temp, temp1);
			strcat(temp, "=");
			strcat(temp, temp2);
			free(temp1);
			free(temp2);
			code.midcode[code.now++] = temp;//输出的三地址代码
			return 0;
		}
		case 1:
		{
			//                            printf("the sema -27..1..\n");
			int flag = position(a);
			if (flag == -1) {
				return 3;//变量不存在
			}
			if (flag == 0) {
				return 2;//变量类型错误,是常量不能赋值

			}
			char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN);
			strcpy(temp, a);
			sema_push(&semanteme_tempvarStack, temp);
			return 0;

		}

		}

	}

	case -28:
	{
		//		        printf("the sema -28..\n");
		//判断现在的临时符号栈中
		char *temp4_value = sema_topEle(&semanteme_signStack);
		if (temp4_value == NULL) {
			return 0;
		}
		//如果项前面有符号
		if ((strcmp(temp4_value, "+.") == 0) || (strcmp(temp4_value, "-.") == 0)) {

			char *item_pl = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
			char *temp2 = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
			_itoa(templemet_count, temp2, 10);
			strcpy(item_pl, "t");
			strcat(item_pl, temp2);
			free(temp2);
			templemet_count++;//临时变量计数count+1




			char *temp1 = sema_pop(&semanteme_tempvarStack);
			temp4_value = sema_pop(&semanteme_signStack);
			char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN);
			strcpy(temp, item_pl);
			strcat(temp, "=");
			strcat(temp, temp4_value);
			strcat(temp, temp1);
			free(temp1);
			free(temp4_value);
			code.midcode[code.now++] = temp;//输出的三地址代码


			sema_push(&semanteme_tempvarStack, item_pl);//变量压入临时栈
			return 0;
		}
		// return 4;
		return 0;

	}

	case -30:
	{
		//printf("pro:")
		//		        printf("the sema -30..\n");
		char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
		printf("%s", a);
		strcpy(temp, a);
		strcat(temp, ".");
		sema_push(&semanteme_signStack, temp);

		return 0;
	}
	case -33:
	{
		//		        printf("the sema -33..\n");
		char *temp2_pl = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);//临时变量
		char *temp2 = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
		_itoa(templemet_count, temp2, 10);
		strcpy(temp2_pl, "t");
		strcat(temp2_pl, temp2);
		free(temp2);
		templemet_count++;//临时变量计数count+1


		char *item2_pl = sema_pop(&semanteme_tempvarStack);
		char *item1_pl = sema_pop(&semanteme_tempvarStack);
		char *add_val = sema_pop(&semanteme_signStack);
		char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN);
		strcpy(temp, temp2_pl);
		strcat(temp, "=");
		strcat(temp, item1_pl);
		strcat(temp, add_val);
		strcat(temp, item2_pl);
		free(item2_pl);
		free(item1_pl);
		free(add_val);
		code.midcode[code.now++] = temp;//输出的三地址代码

		sema_push(&semanteme_tempvarStack, temp2_pl);//变量压入临时栈
		return 0;

	}

	case -37:
	{
		//		        printf("the sema -37..\n");
		char *temp3_pl = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
		char *temp3 = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
		_itoa(templemet_count, temp3, 10);
		strcpy(temp3_pl, "t");
		strcat(temp3_pl, temp3);
		free(temp3);
		templemet_count++;//临时变量计数count+1

		char *factor2_pl = sema_pop(&semanteme_tempvarStack);
		char *factor1_pl = sema_pop(&semanteme_tempvarStack);
		char *mul_val = sema_pop(&semanteme_signStack);
		char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN);
		strcpy(temp, temp3_pl);
		strcat(temp, "=");
		strcat(temp, factor1_pl);
		strcat(temp, mul_val);
		strcat(temp, factor2_pl);
		free(factor2_pl);
		free(factor1_pl);
		free(mul_val);
		code.midcode[code.now++] = temp;//输出的三地址代码

		sema_push(&semanteme_tempvarStack, temp3_pl);//变量压入临时栈
		return 0;
	}
	case -38:
	{
		//                printf("the sema -38..\n");
		int flag = position(a);
		if (flag == -1) {
			return 3;//变量不存在
		}
		char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN);
		strcpy(temp, a);
		sema_push(&semanteme_tempvarStack, temp);
		return 0;
	}
	case -39:
	{
		//		        printf("the sema -39..\n");
		char *un_int_pl = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);//临时变量
		char *un_int = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
		_itoa(templemet_count, un_int, 10);
		strcpy(un_int_pl, "t");
		strcat(un_int_pl, un_int);
		free(un_int);
		templemet_count++;


		char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN);//code
		char *temp1 = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);//临时变量

		_itoa(value, temp1, 10);
		strcpy(temp, un_int_pl);
		strcat(temp, "=");
		strcat(temp, temp1);
		free(temp1);
		code.midcode[code.now++] = temp;				//输出的三地址代码


		sema_push(&semanteme_tempvarStack, un_int_pl);	//变量压入临时栈
		return 0;

	}
	case -41:
	case -42:
	{
		//		        printf("the sema -42..\n");
		char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
		strcpy(temp, a);
		sema_push(&semanteme_signStack, temp);
		return 0;

	}

	//控制流语句的翻译
	case -44:
	case -48:
	{
		fun_backpatch(code.now, newlable);
		newlable--;
		return 0;
	}
	case -45:
	{
		switch (process_id) {
		case 0:
		{
			fun_backpatch(code.now, newlable);
			newlable--;
			return 0;
		}
		case 1:
		{
			int t_label = newlable - 1;
			fun_backpatch(code.now, t_label);
			return 0;
		}
		case 2:
		{
			newlable++;
			backpatch[newlable] = code.now;

			char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN);
			strcpy(temp, "goto");
			code.midcode[code.now++] = temp;//输出的三地址代码
			return 0;
		}

		}
		return 0;
	}
	case -46:
	{
		switch (process_id) {
		case 0:
		{
			//真出口语句
			int t_add = code.now + 2;
			char *add = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
			_itoa(t_add, add, 10);


			char *exp2_pl = sema_pop(&semanteme_tempvarStack);
			char *exp1_pl = sema_pop(&semanteme_tempvarStack);
			char *rel_op = sema_pop(&semanteme_signStack);
			char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN);
			strcpy(temp, "if");
			strcat(temp, exp1_pl);
			strcat(temp, rel_op);
			strcat(temp, exp2_pl);
			strcat(temp, " goto ");
			strcat(temp, add);
			free(exp2_pl);
			free(exp1_pl);
			free(rel_op);
			free(add);
			code.midcode[code.now++] = temp;//输出的三地址代码


											//假出口语句
			newlable++;
			backpatch[newlable] = code.now;
			temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN);
			strcpy(temp, "goto ");
			code.midcode[code.now++] = temp;//输出的三地址代码
			return 0;
		}
		case 1:
		{
			//printf("pro:")
			//				        printf("the sema -46...1...\n");
			char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
			printf("%s", a);
			strcpy(temp, a);
			sema_push(&semanteme_signStack, temp);
			return 0;
		}

		}
		return 0;
	}

	}
}


/*回填真假出口*/
void fun_backpatch(int nowadd, int label) {
	int t_add = backpatch[label];//得到label对应的中间语句
	char *temp = (char *)malloc(sizeof(char)*MAC_CODE_LEN*0.5);
	_itoa(nowadd, temp, 10);
	strcat(code.midcode[t_add], temp);

}


/*find ident: aim the position(offset) in the table*/

int position(char *aim) {
	// printf("aim:%s\n",aim);

	int now_count = table->count;
	for (int k = 0; k<now_count; k++) {
		int flag = strcmp(table->sign[k], aim);
		if (flag == 0) {
			return table->type[k];//表示变量重复定义,返回现在变量的类型）
		}
	}
	return -1;//表示变量不存在

}



/*enter the const's value into table*/
void enter_value() {
	// printf("v:%d\n",v);
	table->value[table->count - 1] = value;
}

/*enter the const or  the var's name into table*/
void enter_name(char* aim, int type) {

	int now_count = table->count;


	table->off[now_count] = table->offset;
	table->type[now_count] = type;
	table->count++;
	table->offset = table->offset + 4;
	//printf("aim:%s\n",aim);

	//printf("table->sign[now_count]:%s\n",table->sign[now_count]);
	table->sign[now_count] = (char *)malloc(SIGN_TABLE_LENTH * sizeof(char));
	strcpy(table->sign[now_count], aim);

}



/*print semanteme's error msg*/
void error(int num) {
	switch (num) {
	case 1:
		printf("\nThe ident is define more than one time....\n");
		return;
	case 2:
		printf("\nThe const ident can't be assign...\n");
		return;
	case 3:
		printf("\nThe ident is not exist...\n");
	case 4:
		printf("\nThe assignment translation has some error...\n");
	}

}


int programarAnalyse() {
	int nowchar;
	int nowposition = 0;
	int i, j, f, l;
	int temp;
	int pre = 0;//前一个弹出栈的元素的层数
	struct Node *nowNode;
	int stackSign, stackFlag;
	//enum nonterminal_symbol_type empty=EMPTY;
	nowchar = minDFA();

	
	push(&signStack, 0);
	push(&flagStack, 0);
	push(&levelStack, 0);

	do {
		//printf("nowchar: %d\n", nowchar);
		i = pop(&signStack);
		f = pop(&flagStack);
		l = pop(&levelStack);

		/*打印输出语法树*/
		if (pre<l) {
			nodeCount[l] = 0;
		}
		pre = l;
		if (nodeCount[l] == 0) {//表示现在这一层没有节点，那么width不变

			if (l == 0) {
				printf("%-18s", printNonterminal[i]);
			}
			else {
				if (f == 1) {
					printf("%-18s", printTerminal[i]);
				}
				else if (f == 0) {
					printf("%-18s", printNonterminal[i]);
				}
				else if (f<0) {
					//语义节点对应的产生式的序号
					printf("pro:%-4d,node:%-4d", -f - 1, i);
				}
			}

			nodeCount[l]++;
			//printf("nodeCount:%d\n",l);

		}
		else {

			/*打印输出*/
			printf("\n");

			for (int k = 0; k<l; k++) {
				for (int q = 0; q<18; q++) {
					printf(" ");
				}
			}
			if (f == 1) {
				printf("%-18s", printTerminal[i]);
			}
			else if (f == 0) {
				printf("%-18s", printNonterminal[i]);
			}
			else if (f<0) {
				//语义节点对应的产生式的序号
				printf("pro:%-4d,node:%-4d", -f - 1, i);
			}
		}


		if (f == 1) {
			if (i == nowchar) {
				//nowchar=getNextString();
				if (nowchar == SYM_DOLLAR) {
					break;
				}
				nowchar = minDFA();
				nowposition++;
			}
			else {
				printf("\nThe programar is error at position %d\n,%d", nowposition, nowchar);
				return 0;
			}
		}
		else if (f == 0) {//±íÊ¾Õ»¶¥ÔªËØÊÇ·ÇÖÕ½á·ûºÅ
			if (nowchar<0) {
				break;
			}
			temp = AnalyseTable[i][nowchar];
			//			printf("production:%d\n",temp);
			if (temp == -1) {
				printf("\nError:The programmer is wrong...some element need be added or removed...\n");
				printf("The rest nodes in the stack is:\n");
				//输出栈中剩余节点的信息
				while (!isEmpty(&signStack)) {

					i = pop(&signStack);
					f = pop(&flagStack);
					l = pop(&levelStack);
					if (f == 0) {
						printf("\nlevel:%d   sign:%s  flag:%d\n", l, printNonterminal[i], f);
					}
					else if (f == 1) {
						printf("level:%d   sign:%s  flag:%d\n", l, printTerminal[i], f);
					}
					else if (f == 2) {
						//语义节点
						printf("level:%d   sign:%d  flag:%d\n", l, i, f);

					}
					printf("nowchar:%s\n", printTerminal[nowchar]);
				}
				return 0;
			}
			nowNode = Production[temp];
			//			printf("\n\nthe production num: %d\n\n",temp);
			while (nowNode != NULL) {
				stackSign = nowNode->non_terminal;
				//                printf("\n\nthe node num: %d\n\n",stackSign);
				stackFlag = nowNode->flag;
				if ((stackFlag == 0) && (stackSign == EMPTY)) {//Èç¹ûµÃµ½µÄ²úÉúÊ½ÓÒ±ßÎª¿Õ
					nowNode = nowNode->next;
					printf("%-18s", printNonterminal[EMPTY]);
				}
				else {
					if (push(&signStack, stackSign)) {
						push(&flagStack, stackFlag);
						push(&levelStack, l + 1);
						nowNode = nowNode->next;
					}
					else {
						printf("\nStack push() error:\t the Stack is full...\n");
						return 0;
					}
				}
			}
		}
		else if (f<0) {
			//语义分析
			int flag;
			flag = semanteme(f, i);

			if (flag != 0) {
				//报错打印
				error(flag);
				return 0;
			}

		}
	} while (!isEmpty(&signStack));
	if (nowchar == -2) {
		printf("\nminDFA() error:\t the vacabulary is illegal...\n");
		return 0;
	}

	if (isEmpty(&signStack)) {
		if (nowchar == SYM_DOLLAR) {
			return 1;
		}
		else {
			printf("\nprogramarAnalyse() error:\t the stack  is empty but the char_input is not over...\n");
			return 0;
		}
	}
	else {
		printf("\nprogramarAnalyse() error:\t the char_input  is empty but the stack is not over...\n");
		return 0;
	}

}


int main() {
	int analyse;
	

	initAnalyseTable();
	initProduction();
	initStack(&signStack);
	initStack(&flagStack);
	initStack(&levelStack);
	init_sema_Stack(&semanteme_signStack);
	init_sema_Stack(&semanteme_tempvarStack);

	
	// printf("The input sentence is:\n");
	freopen("1.txt", "r", stdin);
	freopen("result.txt", "w", stdout); 
	gets(buffer);

	analyse = programarAnalyse();
	if (!analyse) {
		printf("\nThe programmer process is failure..... \n");

	}
	else {
		printf("\nThe programmer process is successful....\n");
	}


	printf("the table count:%d\n", table->count);
	for (int k = 0; k<table->count; k++) 
	{
		printf("ident:%s  type:%d  value:%d  offset:%d\n", table->sign[k], table->type[k], table->value[k], table->off[k]);
	}

	

	destory_code();
	return 0;


}



