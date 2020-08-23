#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp;			//파일을 읽을 때 가리키는 포인터
FILE *outfp;
char outputname[100];
char input;				//소스코드에서 읽어온 글자 한 개를 임시로 담을 변수
char attribute[1000];	//attribute를 입력하는 용도의 버퍼
int i = 0;				//attribute 버퍼의 첨자
int linenumber = 1;		//에러 발생 시 에러가 발생한 위치를 알리기 위한 소스코드의 현재 줄 위치


void openfile(char filename[])
{
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("Error: File not found\n");
		exit(1);
	}
}

void makeoutputfile(char filename[])
{
	strncpy(outputname, filename, strlen(filename) - 2);		//파일이름의 확장자 .c를 지운다
	strcat(outputname, ".out");									//확장자 .out을 붙여서 output의 이름을 짓는다
	outfp = fopen(outputname, "w");							//output 파일 생성
}

void print_error()
{
	
	if (ftell(outfp) != 0)										//만약 지금까지 output 파일에 적은 symbol table 내용이 있을 경우
	{
		fclose(outfp);
		remove(outputname);										//기존에 생성한 output file 제거
	}
	outfp = fopen(outputname, "w");							//output 파일 다시 생성
	fprintf(outfp, "Error at line %d: none of any tokens\n", linenumber);	//에러 출력
}

void closefile()
{
	fclose(fp);
	fclose(outfp);
}

void read(int &nget)
{
	input = fgetc(fp);		//파일 포인터에서 한 글자 읽어와 input에 대입
	nget++;					//읽은 글자수 세기
}

bool classifier(char tokenname[], char attribute[], char input) // final state에 도달한 경우 작동하는 함수
{
	if (attribute != NULL)
		fprintf(outfp, "%s\t\t%s\n", tokenname, attribute);	//token이름과 attribute를 output 파일에 출력한다
	else
		fprintf(outfp, "%s\n", tokenname);		//keyword나 symbol의 경우 attribute를 출력하지 않는다

	if (input != EOF)
		fseek(fp, -1, SEEK_CUR);			//소스코드를 끝까지 읽은 게 아니라면 파일 포인터가 가리키는 위치를 1만큼 앞당긴다
	return true;
}
	
bool rewinder(char input, int nget)	//final state에 도달 못한 경우 작동하는 함수. token분류가 안된 처음 위치로 돌아간다	
{
	if (input == EOF)
		fseek(fp, -nget+1, SEEK_CUR);		//마지막에 읽은 것이 파일의 끝 경우 한 자리 더 되감아지기 때문에 이와 같은 코드를 추가했다.
	else
		fseek(fp, -nget, SEEK_CUR);			//위와 같은 특별한 경우가 아니면 글자수를 읽은 만큼 되감는다		
	return false;
}

bool isletter(char input)
{
	char letter[] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };
	bool answer = false;
	for (int i = 0; i < sizeof(letter); i++)
		if (input == letter[i])
			answer = true;
	return answer;
}

bool isnatural(char input)
{
	char natural[] = { '1','2','3','4','5','6','7','8','9' };
	bool answer = false;
	for (int i = 0; i < sizeof(natural); i++)
		if (input == natural[i])
			answer = true;
	return answer;
}

bool isEND()	//한 글자를 읽어왔을 때 EOF (-1)값을 가지면 참을 반환한다 (파일의 끝에 도달했음을 의미)
{
	char input = fgetc(fp);

	if (input == EOF)
		return true;
	else
	{
		fseek(fp, -1, SEEK_CUR);
		return false;
	}
}

bool isWHITESPACE()
{
	char input = fgetc(fp);
	if (input == '\n')
		linenumber++;			//에러 발생 시 에러가 발생한 위치를 알리기 위해 줄 수를 센다.

	if (input == '\t' || input == ' ' || input == '\n')
		return true;
	else
	{
		if (input != EOF)
			fseek(fp, -1, SEEK_CUR);
		return false;
	}
}

bool isVTYPE()
{
	char tokenname[] = "vtype";		//token 이름
	int state = 0;					//처음 State 초기화
	int nget = 0;					//false를 반환할 경우 token분류가 안된 처음 위치로 돌아가기 위해, 얼만큼 문자를 읽었는지 세어준다
	
	while (1) {						//Documentation에 설명한 DFA를 따라서, 문자를 하나 읽고, state를 바꾸는 걸 반복한다
		read(nget);
		if (input == 'i' && state == 0) state = 1;
		else if (input == 'n' && state == 1) state = 2;
		else if (input == 't' && state == 2) state = 13;
		else if (input == 'c' && state == 0) state = 3;
		else if (input == 'h' && state == 3) state = 4;
		else if (input == 'a' && state == 4) state = 5;
		else if (input == 'r' && state == 5) state = 13;
		else if (input == 'b' && state == 0) state = 6;
		else if (input == 'o' && state == 6) state = 7;
		else if (input == 'o' && state == 7) state = 8;
		else if (input == 'l' && state == 8) state = 13;
		else if (input == 'f' && state == 0) state = 9;
		else if (input == 'l' && state == 9) state = 10;
		else if (input == 'o' && state == 10) state = 11;
		else if (input == 'a' && state == 11) state = 12;
		else if (input == 't' && state == 12) state = 13;
		else if (isletter(input) && state == 13) state = 14;
		else if (input == '0' && state == 13) state = 14;
		else if (isnatural(input) && state == 13) state = 14;
		else if (input == '_' && state == 13) state = 14;
		else break;
		attribute[i++] = input;		//attribute 버퍼에 input 문자를 추가	
	}
	attribute[i++] = '\0';			//attribute 입력 끝
	i = 0;							//attribute 버퍼 재사용을 위해서 첨자를 초기화

	if (state == 13)									//Final state에 도달한 경우
		return classifier(tokenname, attribute, input);	//classifier함수 작동. 현재state가 final state에 포함되면 true반환
	else
		return rewinder(input, nget);					//Final state에 도달하지 않은 경우 되감기
}

bool isINTEGER()
{
	char tokenname[] = "num";	//token 이름
	int state = 0;					//처음 State 초기화
	int nget = 0;					//false를 반환할 경우 token분류가 안된 처음 위치로 돌아가기 위해, 얼만큼 문자를 읽었는지 세어준다

	while (1) {						//Documentation에 설명한 DFA를 따라서, 문자를 하나 읽고, state를 바꾸는 걸 반복한다
		read(nget);
		if (input == '0' && state == 0) state = 1;
		else if (input == '-' && state == 0) state = 2;
		else if (isnatural(input) && state == 0) state = 3;
		else if (isnatural(input) && state == 2) state = 3;
		else if (input == '0' && state == 3) state = 3;
		else if (isnatural(input) && state == 3) state = 3;
		else break;
		attribute[i++] = input;		//attribute 버퍼에 input 문자를 추가
	}
	attribute[i++] = '\0';			//attribute 입력 끝
	i = 0;							//attribute 버퍼 재사용을 위해서 첨자를 초기화

	if (state == 1 || state == 3)						//Final state에 도달한 경우
		return classifier(tokenname, attribute, input);	//classifier함수 작동. 현재state가 final state에 포함되면 true반환
	else
		return rewinder(input, nget);					//Final state에 도달하지 않은 경우 되감기
}

bool isLITERAL()
{
	char tokenname[] = "literal";	//token 이름
	int state = 0;					//처음 State 초기화
	int nget = 0;					//false를 반환할 경우 token분류가 안된 처음 위치로 돌아가기 위해, 얼만큼 문자를 읽었는지 세어준다

	while (1) {						//Documentation에 설명한 DFA를 따라서, 문자를 하나 읽고, state를 바꾸는 걸 반복한다
		read(nget);
		if (input == '\"' && state == 0) state = 1;
		else if (input == '\"' && state == 1) state = 2;
		else if (isletter(input) && state == 1) state = 1;
		else if (isnatural(input) && state == 1) state = 1;
		else if (input == '0' && state == 1) state = 1;
		else if (input == ' ' && state == 1) state = 1;
		else break;
		attribute[i++] = input;		//attribute 버퍼에 input 문자를 추가	
	}
	attribute[i++] = '\0';			//attribute 입력 끝
	i = 0;							//attribute 버퍼 재사용을 위해서 첨자를 초기화

	if (state == 2)										//Final state에 도달한 경우
		return classifier(tokenname, attribute, input);	//classifier함수 작동. 현재state가 final state에 포함되면 true반환
	else
		return rewinder(input, nget);					//Final state에 도달하지 않은 경우 되감기
}

bool isBOOLEAN()
{
	char tokenname[] = "boolean";	//token 이름
	int state = 0;					//처음 State 초기화
	int nget = 0;					//false를 반환할 경우 token분류가 안된 처음 위치로 돌아가기 위해, 얼만큼 문자를 읽었는지 세어준다

	while (1) {						//Documentation에 설명한 DFA를 따라서, 문자를 하나 읽고, state를 바꾸는 걸 반복한다
		read(nget);
		if (input == 't' && state == 0) state = 1;
		else if (input == 'r' && state == 1) state = 2;
		else if (input == 'u' && state == 2) state = 3;
		else if (input == 'e' && state == 3) state = 8;
		else if (input == 'f' && state == 0) state = 4;
		else if (input == 'a' && state == 4) state = 5;
		else if (input == 'l' && state == 5) state = 6;
		else if (input == 's' && state == 6) state = 7;
		else if (input == 'e' && state == 7) state = 8;
		else if (isletter(input) && state == 8) state = 9;
		else if (input == '0' && state == 8) state = 9;
		else if (isnatural(input) && state == 8) state = 9;
		else if (input == '_' && state == 8) state = 9;
		else break;
		attribute[i++] = input;		//attribute 버퍼에 input 문자를 추가	
	}
	attribute[i++] = '\0';			//attribute 입력 끝
	i = 0;							//attribute 버퍼 재사용을 위해서 첨자를 초기화

	if (state == 8)										//Final state에 도달한 경우
		return classifier(tokenname, attribute, input);	//classifier함수 작동. 현재state가 final state에 포함되면 true반환
	else
		return rewinder(input, nget);					//Final state에 도달하지 않은 경우 되감기
}

bool isREAL()
{
	char tokenname[] = "float";		//token 이름
	int state = 0;					//처음 State 초기화
	int nget = 0;					//false를 반환할 경우 token분류가 안된 처음 위치로 돌아가기 위해, 얼만큼 문자를 읽었는지 세어준다

	while (1) {						//Documentation에 설명한 DFA를 따라서, 문자를 하나 읽고, state를 바꾸는 걸 반복한다
		read(nget);
		if (input == '-'&& state == 0) state = 1;
		else if (input == '0' && state == 0) state = 2;
		else if (isnatural(input) && state == 0) state = 3;
		else if (input == '0' && state == 1) state = 2;
		else if (isnatural(input) && state == 1) state = 3;
		else if (input == '.' && state == 2) state = 4;
		else if (input == '0' && state == 3) state = 3;
		else if (isnatural(input) && state == 3) state = 3;
		else if (input == '.' && state == 3) state = 4;
		else if (input == '0' && state == 4) state = 5;
		else if (isnatural(input) && state == 4) state = 5;
		else if (isnatural(input) && state == 5) state = 5;
		else break;
		attribute[i++] = input;		//attribute 버퍼에 input 문자를 추가	
	}
	attribute[i++] = '\0';			//attribute 입력 끝
	i = 0;							//attribute 버퍼 재사용을 위해서 첨자를 초기화

	if (state == 5)										//Final state에 도달한 경우
		return classifier(tokenname, attribute, input);	//classifier함수 작동. 현재state가 final state에 포함되면 true반환
	else
		return rewinder(input, nget);					//Final state에 도달하지 않은 경우 되감기
}

bool isID()
{
	char tokenname[] = "id";	//token 이름
	int state = 0;					//처음 State 초기화
	int nget = 0;					//false를 반환할 경우 token분류가 안된 처음 위치로 돌아가기 위해, 얼만큼 문자를 읽었는지 세어준다

	while (1) {						//Documentation에 설명한 DFA를 따라서, 문자를 하나 읽고, state를 바꾸는 걸 반복한다
		read(nget);
		if (isletter(input) && state == 0) state = 1;
		else if (input == '_' && state == 0) state = 1;
		else if (isletter(input) && state == 1) state = 1;
		else if (input == '_' && state == 1) state = 1;
		else if (input == '0' && state == 1) state = 1;
		else if (isnatural(input) && state == 1) state = 1;
		else break;
		attribute[i++] = input;		//attribute 버퍼에 input 문자를 추가	
	}
	attribute[i++] = '\0';			//attribute 입력 끝
	i = 0;							//attribute 버퍼 재사용을 위해서 첨자를 초기화

	if (state == 1)										//Final state에 도달한 경우
		return classifier(tokenname, attribute, input);	//classifier함수 작동. 현재state가 final state에 포함되면 true반환
	else
		return rewinder(input, nget);					//Final state에 도달하지 않은 경우 되감기
}

bool isKEYWORD()
{
	//키워드의 경우 token 이름은 attribute배열의 문자열을 그대로 넣는 방식으로 한다.
	int state = 0;					//처음 State 초기화
	int nget = 0;					//false를 반환할 경우 token분류가 안된 처음 위치로 돌아가기 위해, 얼만큼 문자를 읽었는지 세어준다

	while (1) {						//Documentation에 설명한 DFA를 따라서, 문자를 하나 읽고, state를 바꾸는 걸 반복한다
		read(nget);
		if (input == 'i' && state == 0) state = 1;
		else if (input == 'f' && state == 1) state = 16;
		else if (input == 'e' && state == 0) state = 2;
		else if (input == 'l' && state == 2) state = 3;
		else if (input == 's' && state == 3) state = 4;
		else if (input == 'e' && state == 4) state = 16;
		else if (input == 'w' && state == 0) state = 5;
		else if (input == 'h' && state == 5) state = 6;
		else if (input == 'i' && state == 6) state = 7;
		else if (input == 'l' && state == 7) state = 8;
		else if (input == 'e' && state == 8) state = 16;
		else if (input == 'f' && state == 0) state = 9;
		else if (input == 'o' && state == 9) state = 10;
		else if (input == 'r' && state == 10) state = 16;
		else if (input == 'r' && state == 0) state = 11;
		else if (input == 'e' && state == 11) state = 12;
		else if (input == 't' && state == 12) state = 13;
		else if (input == 'u' && state == 13) state = 14;
		else if (input == 'r' && state == 14) state = 15;
		else if (input == 'n' && state == 15) state = 16;
		else if (isletter(input) && state == 16) state = 17;
		else if (input == '0' && state == 16) state = 17;
		else if (isnatural(input) && state == 16) state = 17;
		else if (input == '_' && state == 16) state = 17;
		else break;
		attribute[i++] = input;
			}
		attribute[i++] = '\0';			//attribute 입력 끝
			i = 0;							//attribute 버퍼 재사용을 위해서 첨자를 초기화

				if (state == 16)									//Final state에 도달한 경우
							return classifier(attribute, NULL, input);		//classifier함수 작동. 현재state가 final state에 포함되면 true반환
					else
								return rewinder(input, nget);					//Final state에 도달하지 않은 경우 되감기
}

bool isOPERATOR()
{
	char tokenname[5][8];			//token 이름
	strcpy(tokenname[0], "addsub");
	strcpy(tokenname[1], "bitwise");
	strcpy(tokenname[2], "comp");
	strcpy(tokenname[3], "assign");
	strcpy(tokenname[4], "multdiv");
	int state = 0;					//처음 State 초기화
	int nget = 0;					//false를 반환할 경우 token분류가 안된 처음 위치로 돌아가기 위해, 얼만큼 문자를 읽었는지 세어준다

	while (1) {						//Documentation에 설명한 DFA를 따라서, 문자를 하나 읽고, state를 바꾸는 걸 반복한다
		read(nget);
		if (input == '+' && state == 0) state = 1;
		else if (input == '-' && state == 0) state = 1;
		else if (input == '*' && state == 0) state = 8;
		else if (input == '/' && state == 0) state = 8;
		else if (input == '&' && state == 0) state = 4;
		else if (input == '|' && state == 0) state = 4;
		else if (input == '<' && state == 0) state = 2;
		else if (input == '>' && state == 0) state = 3;
		else if (input == '=' && state == 0) state = 5;
		else if (input == '!' && state == 0) state = 6;
		else if (input == '<' && state == 2) state = 4;
		else if (input == '>' && state == 3) state = 4;
		else if (input == '=' && state == 2) state = 7;
		else if (input == '=' && state == 3) state = 7;
		else if (input == '=' && state == 5) state = 7;
		else if (input == '=' && state == 6) state = 7;
		else break;
		attribute[i++] = input;		//attribute 버퍼에 input 문자를 추가	
	}
	attribute[i++] = '\0';			//attribute 입력 끝
	i = 0;							//attribute 버퍼 재사용을 위해서 첨자를 초기화

	if (state == 1)												//addsub
		return classifier(tokenname[0], attribute, input);
	else if (state == 8)										//multdiv
		return classifier(tokenname[4], attribute, input);		
	else if (state == 4)										//BITWISE
		return classifier(tokenname[1], attribute, input);
	else if (state == 2 || state == 3 || state == 7)			//COMP
		return classifier(tokenname[2], attribute, input);
	else if (state == 5)										//ASSIGN
		return classifier(tokenname[3], attribute, input);
	else
		return rewinder(input, nget);					//Final state에 도달하지 않은 경우 되감기
}

bool isSYMBOL()
{
	char tokenname[6][7];			//token 이름
	strcpy(tokenname[0], "semi");
	strcpy(tokenname[1], "lbrace");
	strcpy(tokenname[2], "rbrace");
	strcpy(tokenname[3], "lparen");
	strcpy(tokenname[4], "rparen");
	strcpy(tokenname[5], "comma");
	int state = 0;					//처음 State 초기화
	int nget = 0;					//false를 반환할 경우 token분류가 안된 처음 위치로 돌아가기 위해, 얼만큼 문자를 읽었는지 세어준다

	while (1) {						//Documentation에 설명한 DFA를 따라서, 문자를 하나 읽고, state를 바꾸는 걸 반복한다
		read(nget);
		if (input == ';' && state == 0) state = 1;
		else if (input == '{' && state == 0) state = 2;
		else if (input == '}' && state == 0) state = 3;
		else if (input == '(' && state == 0) state = 4;
		else if (input == ')' && state == 0) state = 5;
		else if (input == ',' && state == 0) state = 6;
		else break;
		attribute[i++] = input;		//attribute 버퍼에 input 문자를 추가	
	}
	attribute[i++] = '\0';			//attribute 입력 끝
	i = 0;							//attribute 버퍼 재사용을 위해서 첨자를 초기화

	if (state == 1)												//SEMI
		return classifier(tokenname[0], NULL, input);
	else if (state == 2)										//lbrace
		return classifier(tokenname[1], NULL, input);
	else if (state == 3)										//rbrace
		return classifier(tokenname[2], NULL, input);
	else if (state == 4)										//LPAREN
		return classifier(tokenname[3], NULL, input);
	else if (state == 5)										//RPAREN
		return classifier(tokenname[4], NULL, input);
	else if (state == 6)										//COMMA
		return classifier(tokenname[5], NULL, input);
	else
		return rewinder(input, nget);					//Final state에 도달하지 않은 경우 되감기
}

int main(int argc, char *argv[])
{
	openfile(argv[1]);			//소스 코드의 이름을 인수로 전달받아 파일을 연다
	makeoutputfile(argv[1]);	//output파일을 생성한다

	while (!isEND())			//파일 끝에 도달할 때까지 token 분류를 반복
	{
		if (isWHITESPACE()) continue;
		if (isSYMBOL()) continue;
		if (isVTYPE()) continue;
		if (isKEYWORD()) continue;
		if (isBOOLEAN()) continue;
		if (isLITERAL()) continue;
		if (isREAL()) continue;
		if (isINTEGER()) continue;
		if (isOPERATOR()) continue;
		if (isID()) continue;
		print_error();					//어느 것에도 분류되지 않는다면 에러출력
		break;							//루프 중단			
	}
	closefile();
}
