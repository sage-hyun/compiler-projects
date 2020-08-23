/******************************************************************************
컴파일러 과제 2

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STACK_SIZE 100
#define MAX_ARRAY_SIZE 1000

FILE *fp;			//파일을 읽을 때 가리키는 포인터
//char **tokenArray;
int size = 0; 
char tokenArray[MAX_ARRAY_SIZE][7];

int n = -1;
char stack[MAX_STACK_SIZE][3];
int top=-1;

int splitterIndex = 0; 
int accept = 0;

char GOTO[52][3][7] = {
{"1", "CODE", "2"},      {"1", "VDECL", "3"},     {"1", "FDECL", "5"},
{"3", "CODE", "4"},      {"3", "VDECL", "3"},     {"3", "FDECL", "5"},
{"5", "CODE", "6"},      {"5", "VDECL", "3"},     {"5", "FDECL", "5"},
{"7", "ASSIGN", "9"},
{"13", "EXPR", "15"},    {"13", "RHS", "14"},     {"13", "TERM", "17"},    {"13", "FACTOR", "18"},
{"22", "EXPR", "27"},    {"22", "TERM", "17"},    {"22", "FACTOR", "18"},
{"23", "EXPR", "24"},    {"23", "TERM", "17"},    {"23", "FACTOR", "18"},
{"25", "TERM", "26"},    {"25", "FACTOR", "18"},
{"29", "ARG", "37"},
{"31", "MOREA", "32"},
{"35", "MOREA", "36"},
{"39", "VDECL", "42"},   {"39", "ASSIGN", "43"},  {"39", "STMT", "40"},    {"39", "BLOCK", "78"},
{"40", "VDECL", "42"},   {"40", "ASSIGN", "43"},  {"40", "BLOCK", "41"},
{"46", "FACTOR", "57"},  {"46", "COND", "47"},
{"49", "VDECL", "42"},   {"49", "ASSIGN", "43"},  {"49", "BLOCK", "50"},
{"51", "ELSE", "52"},
{"54", "VDECL", "42"},   {"54", "ASSIGN", "43"},  {"54", "BLOCK", "55"},
{"58", "FACTOR", "59"},
{"61", "FACTOR", "57"},  {"61", "COND", "62"},
{"64", "BLOCK", "65"},
{"68", "ASSIGN", "69"},
{"70", "FACTOR", "57"},  {"70", "COND", "71"},
{"72", "ASSIGN", "73"},
{"75", "BLOCK", "76"},
{"78", "RETURN", "82"},
{"79", "FACTOR", "80"}
};

char shift[96][3][8] = {
{"1", "vtype", "7"},
{"3", "vtype", "7"},
{"5", "vtype", "7"},
{"7", "id", "8"},
{"8", "assign", "13"}, {"8", "semi", "10"},    {"8", "lparen", "29"},
{"9", "semi", "11"},
{"13", "literal", "16"},{"13", "num", "20"},    {"13", "float", "21"},  {"13", "lparen", "22"}, {"13", "id", "19"},
{"17", "addsub", "23"},
{"18", "multdiv", "25"},
{"22", "num", "20"},    {"22", "float", "21"},  {"22", "lparen", "22"}, {"22", "id", "19"},
{"23", "num", "20"},    {"23", "float", "21"},  {"23", "lparen", "22"}, {"23", "id", "19"},
{"25", "num", "20"},    {"25", "float", "21"},  {"25", "lparen", "22"}, {"25", "id", "19"},
{"27", "rparen", "28"},
{"29", "vtype", "30"},
{"30", "id", "31"},
{"31", "comma", "33"},
{"33", "vtype", "34"},
{"34", "id", "35"},
{"35", "comma", "33"},
{"37", "rparen", "38"},
{"38", "lbrace", "39"},
{"39", "if", "45"},     {"39", "while", "60"},  {"39", "for", "67"},    {"39", "id", "12"},     {"39", "vtype", "7"},
{"40", "if", "45"},     {"40", "while", "60"},  {"40", "for", "67"},    {"40", "id", "12"},     {"40", "vtype", "7"},
{"43", "semi", "44"},
{"45", "lparen", "46"},
{"46", "num", "20"},    {"46", "float", "21"},  {"46", "lparen", "22"}, {"46", "id", "19"},
{"47", "rparen", "48"},
{"48", "lbrace", "49"},
{"49", "if", "45"},     {"49", "while", "60"},  {"49", "for", "67"},    {"49", "vtype", "7"},
{"50", "rbrace", "51"},
{"51", "else", "53"},
{"53", "lbrace", "54"},
{"54", "if", "45"},     {"54", "while", "60"},  {"54", "for", "67"},    {"54", "vtype", "7"},
{"55", "rbrace", "56"},
{"57", "comp", "58"},
{"58", "num", "20"},    {"58", "float", "21"},  {"58", "lparen", "22"}, {"58", "id", "19"},
{"60", "lparen", "61"},
{"61", "num", "20"},    {"61", "float", "21"},  {"61", "lparen", "22"}, {"61", "id", "19"},
{"62", "rparen", "63"},
{"63", "lbrace", "64"},
{"65", "rbrace", "66"},
{"67", "lparen", "68"},
{"69", "semi", "70"},
{"70", "num", "20"},    {"70", "float", "21"},  {"70", "lparen", "22"}, {"70", "id", "19"},
{"71", "semi", "72"},
{"73", "rparen", "74"},
{"74", "lbrace", "75"},
{"76", "rbrace", "77"},
{"78", "return", "79"},
{"79", "num", "20"},    {"79", "float", "21"},  {"79", "lparen", "22"}, {"79", "id", "19"},
{"80", "semi", "81"},
{"82", "rbrace", "83"}
};

//순서대로 context free grammar 33개의 좌변
char reduceLS[33][7] = {
"S", 
"CODE","CODE","CODE","VDECL","VDECL","ASSIGN","FDECL","ARG","ARG","MOREA",
"MOREA","BLOCK","BLOCK","STMT","STMT","STMT","STMT","STMT","ELSE","ELSE",
"RHS","RHS","EXPR","EXPR","TERM","TERM","FACTOR","FACTOR","FACTOR","FACTOR",
"COND","RETURN"
};

//순서대로 context free grammar 33개의 우변의 길이
//Reduce 시, 스택에서 빼내야 할 아이템의 개수, tokenArray의 삭제할 원소의 수를 알려준다.
int reduceRS[33] = {1,
2, 2, 0, 3, 3, 3, 9, 3, 0, 4, 
0, 2, 0, 1, 2, 8, 7, 11, 4, 0, 
1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 
3, 3};

char reduce[104][3][8] = {
{"1", "$", "3"},
{"2", "$", "0"},
{"3", "$", "3"},
{"4", "$", "1"},
{"5", "$", "3"},
{"6", "$", "2"},
{"10", "vtype", "4"},   {"10", "$", "4"},
{"11", "vtype", "5"},   {"11", "$", "5"},
{"14", "semi", "6"},    {"14", "rparen", "6"},
{"15", "semi", "21"},   {"15", "rparen", "21"},
{"16", "semi", "22"},   {"16", "rparen", "22"},
{"17", "semi", "24"},   {"17", "rparen", "24"},
{"18", "semi", "26"},   {"18", "addsub", "26"}, {"18", "rparen", "26"},
{"19", "semi", "28"},   {"19", "addsub", "28"}, {"19", "multdiv", "28"},{"19", "comp", "28"},
{"20", "semi", "29"},   {"20", "addsub", "29"}, {"20", "multdiv", "29"},{"20", "comp", "29"},
{"21", "semi", "30"},   {"21", "addsub", "30"}, {"21", "multdiv", "30"},{"21", "comp", "30"},
{"24", "semi", "23"},   {"24", "rparen", "23"},
{"26", "semi", "25"},   {"26", "addsub", "25"}, {"26", "rparen", "25"},
{"28", "semi", "27"},   {"28", "addsub", "27"}, {"28", "multdiv", "27"},{"28", "comp", "27"},
{"29", "rparen", "9"},
{"31", "rparen", "11"},
{"32", "rparen", "8"},
{"36", "rparen", "10"},
{"39", "rbrace", "13"}, {"39", "return", "13"},
{"40", "rbrace", "13"}, {"40", "return", "13"},
{"41", "rbrace", "12"}, {"41", "return", "12"},
{"42", "if", "14"},     {"42", "while", "14"},  {"42", "for", "14"},    {"42", "id", "14"},     {"42", "vtype", "14"},  {"42", "rbrace", "14"}, {"42", "return", "14"},
{"44", "if", "15"},     {"44", "while", "15"},  {"44", "for", "15"},    {"44", "id", "15"},     {"44", "vtype", "15"},  {"44", "rbrace", "15"}, {"44", "return", "15"},
{"49", "rbrace", "13"}, {"49", "return", "13"},
{"51", "rbrace", "20"}, {"51", "return", "20"},
{"52", "if", "16"},     {"52", "while", "16"},  {"52", "for", "16"},    {"52", "id", "16"},     {"52", "vtype", "16"},  {"52", "rbrace", "16"}, {"52", "return", "16"},
{"54", "rbrace", "13"}, {"54", "return", "13"},
{"56", "rbrace", "19"}, {"56", "return", "19"},
{"59", "semi", "31"},   {"59", "rparen", "31"},
{"64", "rbrace", "13"}, {"64", "return", "13"},
{"66", "if", "17"},     {"66", "while", "17"},  {"66", "for", "17"},    {"66", "id", "17"},     {"66", "vtype", "17"},  {"66", "rbrace", "17"}, {"66", "return", "17"},
{"75", "rbrace", "13"}, {"75", "return", "13"},
{"77", "if", "18"},     {"77", "while", "18"},  {"77", "for", "18"},    {"77", "id", "18"},     {"77", "vtype", "18"},  {"77", "rbrace", "18"}, {"77", "return", "18"},
{"81", "rbrace", "32"},
{"83", "vtype", "7"},   {"83", "$", "7"}
};


void openfile(char filename[])
{
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("Error: File not found\n");
		exit(1);
	}
}








void readToken()
{
   
   
    
    int i = 0;
    while(!feof(fp))
	{
		fgets(tokenArray[i], 1000, fp);
		tokenArray[i][strcspn(tokenArray[i], " \n\t")] = '\0';
		
		i++;
		size++;

    }
    strcpy(tokenArray[--i], "$");
    
    
}

void print_accept(char filename[])
{
	fclose(fp);
	remove(filename);                                   //기존의 output 파일 제거
	fp = fopen(filename, "w");							//output 파일 다시 생성
	fprintf(fp, "Accepted");	                        //Accepted 출력
}

void print_error(char filename[])
{
	fclose(fp);
	remove(filename);                                   //기존의 output 파일 제거
	fp = fopen(filename, "w");							//output 파일 다시 생성
	fprintf(fp, "Error\n");     	//에러 출력
	fclose(fp);
}

void push(char* state)
{
    if(top<MAX_STACK_SIZE-1)
        strcpy(stack[++top], state); 
}
 
char* pop()
{
    if(top>=0)
        return stack[top--];
}




int shiftDecision()
{
    for(int i = 0; i < 96; i++)
    {
        if(!strcmp(stack[top], shift[i][0]))
        {
            printf("shift1");
            
            if(!strcmp(tokenArray[splitterIndex],shift[i][1]))
            {
                push(shift[i][2]);
                splitterIndex = splitterIndex + 1;
                printf("shift2\n");
                return 1;
            }
        }
    }
    return 0;
}

void reduceAction(int n) {
    int delSize = reduceRS[n] - 1;
    
    if(delSize != 0)
    {
        for(int i = size-1; i >= splitterIndex; i--)
            strcpy(tokenArray[i-delSize], tokenArray[i]);
    }
    
    size -= delSize;
    splitterIndex -= delSize;
    
    strcpy(tokenArray[splitterIndex-1], reduceLS[n]);
    printf("X: %s\n",tokenArray[splitterIndex-1]);
}

int reduceDecision()
{
    for(int i = 0; i < 104; i++)
    {
        if(!strcmp(stack[top],reduce[i][0]))
            if(!strcmp(tokenArray[splitterIndex],reduce[i][1]))
            {
                n = atoi(reduce[i][2]); //contex free grammar 정보
                printf("reduce %s %d\n",reduceLS[n], reduceRS[n]);
                if(n == 0)
                {
                    accept = 1;
                    return 1;
                }
                
                reduceAction(n);
                
                
                
                for(int j = 0; j < reduceRS[n]; j++)    //step1
                    pop();
                printf("stt: %s\n",stack[top]);
               
                
                for(int i = 0; i < 52; i++)     //step2
                {
                    if(!strcmp(stack[top],GOTO[i][0]))
                        if(!strcmp(reduceLS[n],GOTO[i][1]))
                        {
                            push(GOTO[i][2]);
                            
                            printf("reduce2");
                            return 1;
                        }    
                }
            }
    }
    return 0;
}



int main(int argc, char *argv[])
{
    openfile(argv[1]);			//소스 코드의 이름을 인수로 전달받아 파일을 연다
    readToken();
    
    
    for(int i = 0; i < size; i++)
        printf("%d %s \n",i, tokenArray[i]);
    if(!strcmp(tokenArray[4], "semi"))
        printf("yes");
    
    
    
    
    push("1");
    while(accept == 0)
    {
        printf("spl:%d stt:%s\n", splitterIndex, stack[top]);
        for(int i = 0; i < size; i++)
                printf("%d %s \n",i, tokenArray[i]);
        if(shiftDecision() == 0)
            if(reduceDecision() == 0)
            {
                
                //print_error(argv[1]);
                return 0;
            }
    }
    print_accept(argv[1]);
    
    return 0;
        
}