#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp;			//������ ���� �� ����Ű�� ������
FILE *outfp;
char outputname[100];
char input;				//�ҽ��ڵ忡�� �о�� ���� �� ���� �ӽ÷� ���� ����
char attribute[1000];	//attribute�� �Է��ϴ� �뵵�� ����
int i = 0;				//attribute ������ ÷��
int linenumber = 1;		//���� �߻� �� ������ �߻��� ��ġ�� �˸��� ���� �ҽ��ڵ��� ���� �� ��ġ


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
	strncpy(outputname, filename, strlen(filename) - 2);		//�����̸��� Ȯ���� .c�� �����
	strcat(outputname, ".out");									//Ȯ���� .out�� �ٿ��� output�� �̸��� ���´�
	outfp = fopen(outputname, "w");							//output ���� ����
}

void print_error()
{
	
	if (ftell(outfp) != 0)										//���� ���ݱ��� output ���Ͽ� ���� symbol table ������ ���� ���
	{
		fclose(outfp);
		remove(outputname);										//������ ������ output file ����
	}
	outfp = fopen(outputname, "w");							//output ���� �ٽ� ����
	fprintf(outfp, "Error at line %d: none of any tokens\n", linenumber);	//���� ���
}

void closefile()
{
	fclose(fp);
	fclose(outfp);
}

void read(int &nget)
{
	input = fgetc(fp);		//���� �����Ϳ��� �� ���� �о�� input�� ����
	nget++;					//���� ���ڼ� ����
}

bool classifier(char tokenname[], char attribute[], char input) // final state�� ������ ��� �۵��ϴ� �Լ�
{
	if (attribute != NULL)
		fprintf(outfp, "%s\t\t%s\n", tokenname, attribute);	//token�̸��� attribute�� output ���Ͽ� ����Ѵ�
	else
		fprintf(outfp, "%s\n", tokenname);		//keyword�� symbol�� ��� attribute�� ������� �ʴ´�

	if (input != EOF)
		fseek(fp, -1, SEEK_CUR);			//�ҽ��ڵ带 ������ ���� �� �ƴ϶�� ���� �����Ͱ� ����Ű�� ��ġ�� 1��ŭ �մ���
	return true;
}
	
bool rewinder(char input, int nget)	//final state�� ���� ���� ��� �۵��ϴ� �Լ�. token�з��� �ȵ� ó�� ��ġ�� ���ư���	
{
	if (input == EOF)
		fseek(fp, -nget+1, SEEK_CUR);		//�������� ���� ���� ������ �� ��� �� �ڸ� �� �ǰ������� ������ �̿� ���� �ڵ带 �߰��ߴ�.
	else
		fseek(fp, -nget, SEEK_CUR);			//���� ���� Ư���� ��찡 �ƴϸ� ���ڼ��� ���� ��ŭ �ǰ��´�		
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

bool isEND()	//�� ���ڸ� �о���� �� EOF (-1)���� ������ ���� ��ȯ�Ѵ� (������ ���� ���������� �ǹ�)
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
		linenumber++;			//���� �߻� �� ������ �߻��� ��ġ�� �˸��� ���� �� ���� ����.

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
	char tokenname[] = "VTYPE";		//token �̸�
	int state = 0;					//ó�� State �ʱ�ȭ
	int nget = 0;					//false�� ��ȯ�� ��� token�з��� �ȵ� ó�� ��ġ�� ���ư��� ����, ��ŭ ���ڸ� �о����� �����ش�
	
	while (1) {						//Documentation�� ������ DFA�� ����, ���ڸ� �ϳ� �а�, state�� �ٲٴ� �� �ݺ��Ѵ�
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
		attribute[i++] = input;		//attribute ���ۿ� input ���ڸ� �߰�	
	}
	attribute[i++] = '\0';			//attribute �Է� ��
	i = 0;							//attribute ���� ������ ���ؼ� ÷�ڸ� �ʱ�ȭ

	if (state == 13)									//Final state�� ������ ���
		return classifier(tokenname, attribute, input);	//classifier�Լ� �۵�. ����state�� final state�� ���ԵǸ� true��ȯ
	else
		return rewinder(input, nget);					//Final state�� �������� ���� ��� �ǰ���
}

bool isINTEGER()
{
	char tokenname[] = "INTEGER";	//token �̸�
	int state = 0;					//ó�� State �ʱ�ȭ
	int nget = 0;					//false�� ��ȯ�� ��� token�з��� �ȵ� ó�� ��ġ�� ���ư��� ����, ��ŭ ���ڸ� �о����� �����ش�

	while (1) {						//Documentation�� ������ DFA�� ����, ���ڸ� �ϳ� �а�, state�� �ٲٴ� �� �ݺ��Ѵ�
		read(nget);
		if (input == '0' && state == 0) state = 1;
		else if (input == '-' && state == 0) state = 2;
		else if (isnatural(input) && state == 0) state = 3;
		else if (isnatural(input) && state == 2) state = 3;
		else if (input == '0' && state == 3) state = 3;
		else if (isnatural(input) && state == 3) state = 3;
		else break;
		attribute[i++] = input;		//attribute ���ۿ� input ���ڸ� �߰�
	}
	attribute[i++] = '\0';			//attribute �Է� ��
	i = 0;							//attribute ���� ������ ���ؼ� ÷�ڸ� �ʱ�ȭ

	if (state == 1 || state == 3)						//Final state�� ������ ���
		return classifier(tokenname, attribute, input);	//classifier�Լ� �۵�. ����state�� final state�� ���ԵǸ� true��ȯ
	else
		return rewinder(input, nget);					//Final state�� �������� ���� ��� �ǰ���
}

bool isLITERAL()
{
	char tokenname[] = "LITERAL";	//token �̸�
	int state = 0;					//ó�� State �ʱ�ȭ
	int nget = 0;					//false�� ��ȯ�� ��� token�з��� �ȵ� ó�� ��ġ�� ���ư��� ����, ��ŭ ���ڸ� �о����� �����ش�

	while (1) {						//Documentation�� ������ DFA�� ����, ���ڸ� �ϳ� �а�, state�� �ٲٴ� �� �ݺ��Ѵ�
		read(nget);
		if (input == '\"' && state == 0) state = 1;
		else if (input == '\"' && state == 1) state = 2;
		else if (isletter(input) && state == 1) state = 1;
		else if (isnatural(input) && state == 1) state = 1;
		else if (input == '0' && state == 1) state = 1;
		else if (input == ' ' && state == 1) state = 1;
		else break;
		attribute[i++] = input;		//attribute ���ۿ� input ���ڸ� �߰�	
	}
	attribute[i++] = '\0';			//attribute �Է� ��
	i = 0;							//attribute ���� ������ ���ؼ� ÷�ڸ� �ʱ�ȭ

	if (state == 2)										//Final state�� ������ ���
		return classifier(tokenname, attribute, input);	//classifier�Լ� �۵�. ����state�� final state�� ���ԵǸ� true��ȯ
	else
		return rewinder(input, nget);					//Final state�� �������� ���� ��� �ǰ���
}

bool isBOOLEAN()
{
	char tokenname[] = "BOOLEAN";	//token �̸�
	int state = 0;					//ó�� State �ʱ�ȭ
	int nget = 0;					//false�� ��ȯ�� ��� token�з��� �ȵ� ó�� ��ġ�� ���ư��� ����, ��ŭ ���ڸ� �о����� �����ش�

	while (1) {						//Documentation�� ������ DFA�� ����, ���ڸ� �ϳ� �а�, state�� �ٲٴ� �� �ݺ��Ѵ�
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
		attribute[i++] = input;		//attribute ���ۿ� input ���ڸ� �߰�	
	}
	attribute[i++] = '\0';			//attribute �Է� ��
	i = 0;							//attribute ���� ������ ���ؼ� ÷�ڸ� �ʱ�ȭ

	if (state == 8)										//Final state�� ������ ���
		return classifier(tokenname, attribute, input);	//classifier�Լ� �۵�. ����state�� final state�� ���ԵǸ� true��ȯ
	else
		return rewinder(input, nget);					//Final state�� �������� ���� ��� �ǰ���
}

bool isREAL()
{
	char tokenname[] = "REAL";		//token �̸�
	int state = 0;					//ó�� State �ʱ�ȭ
	int nget = 0;					//false�� ��ȯ�� ��� token�з��� �ȵ� ó�� ��ġ�� ���ư��� ����, ��ŭ ���ڸ� �о����� �����ش�

	while (1) {						//Documentation�� ������ DFA�� ����, ���ڸ� �ϳ� �а�, state�� �ٲٴ� �� �ݺ��Ѵ�
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
		attribute[i++] = input;		//attribute ���ۿ� input ���ڸ� �߰�	
	}
	attribute[i++] = '\0';			//attribute �Է� ��
	i = 0;							//attribute ���� ������ ���ؼ� ÷�ڸ� �ʱ�ȭ

	if (state == 5)										//Final state�� ������ ���
		return classifier(tokenname, attribute, input);	//classifier�Լ� �۵�. ����state�� final state�� ���ԵǸ� true��ȯ
	else
		return rewinder(input, nget);					//Final state�� �������� ���� ��� �ǰ���
}

bool isID()
{
	char tokenname[] = "ID";	//token �̸�
	int state = 0;					//ó�� State �ʱ�ȭ
	int nget = 0;					//false�� ��ȯ�� ��� token�з��� �ȵ� ó�� ��ġ�� ���ư��� ����, ��ŭ ���ڸ� �о����� �����ش�

	while (1) {						//Documentation�� ������ DFA�� ����, ���ڸ� �ϳ� �а�, state�� �ٲٴ� �� �ݺ��Ѵ�
		read(nget);
		if (isletter(input) && state == 0) state = 1;
		else if (input == '_' && state == 0) state = 1;
		else if (isletter(input) && state == 1) state = 1;
		else if (input == '_' && state == 1) state = 1;
		else if (input == '0' && state == 1) state = 1;
		else if (isnatural(input) && state == 1) state = 1;
		else break;
		attribute[i++] = input;		//attribute ���ۿ� input ���ڸ� �߰�	
	}
	attribute[i++] = '\0';			//attribute �Է� ��
	i = 0;							//attribute ���� ������ ���ؼ� ÷�ڸ� �ʱ�ȭ

	if (state == 1)										//Final state�� ������ ���
		return classifier(tokenname, attribute, input);	//classifier�Լ� �۵�. ����state�� final state�� ���ԵǸ� true��ȯ
	else
		return rewinder(input, nget);					//Final state�� �������� ���� ��� �ǰ���
}

bool isKEYWORD()
{
	//Ű������ ��� token �̸��� attribute�迭�� ���ڿ��� �빮��ȭ �ϴ� ������� �Ѵ�.
	int state = 0;					//ó�� State �ʱ�ȭ
	int nget = 0;					//false�� ��ȯ�� ��� token�з��� �ȵ� ó�� ��ġ�� ���ư��� ����, ��ŭ ���ڸ� �о����� �����ش�

	while (1) {						//Documentation�� ������ DFA�� ����, ���ڸ� �ϳ� �а�, state�� �ٲٴ� �� �ݺ��Ѵ�
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
		attribute[i++] = input - 32;	//attribute ���ۿ� input ���ڸ� ***�빮��ȭ�Ͽ�*** �߰�	
			}
		attribute[i++] = '\0';			//attribute �Է� ��
			i = 0;							//attribute ���� ������ ���ؼ� ÷�ڸ� �ʱ�ȭ

				if (state == 16)									//Final state�� ������ ���
							return classifier(attribute, NULL, input);		//classifier�Լ� �۵�. ����state�� final state�� ���ԵǸ� true��ȯ
					else
								return rewinder(input, nget);					//Final state�� �������� ���� ��� �ǰ���
}

bool isOPERATOR()
{
	char tokenname[4][8];			//token �̸�
	strcpy(tokenname[0], "ARITHM");
	strcpy(tokenname[1], "BITWISE");
	strcpy(tokenname[2], "COMP");
	strcpy(tokenname[3], "ASSIGN");
	int state = 0;					//ó�� State �ʱ�ȭ
	int nget = 0;					//false�� ��ȯ�� ��� token�з��� �ȵ� ó�� ��ġ�� ���ư��� ����, ��ŭ ���ڸ� �о����� �����ش�

	while (1) {						//Documentation�� ������ DFA�� ����, ���ڸ� �ϳ� �а�, state�� �ٲٴ� �� �ݺ��Ѵ�
		read(nget);
		if (input == '+' && state == 0) state = 1;
		else if (input == '-' && state == 0) state = 1;
		else if (input == '*' && state == 0) state = 1;
		else if (input == '/' && state == 0) state = 1;
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
		attribute[i++] = input;		//attribute ���ۿ� input ���ڸ� �߰�	
	}
	attribute[i++] = '\0';			//attribute �Է� ��
	i = 0;							//attribute ���� ������ ���ؼ� ÷�ڸ� �ʱ�ȭ

	if (state == 1)												//ARITHM
		return classifier(tokenname[0], attribute, input);	
	else if (state == 4)										//BITWISE
		return classifier(tokenname[1], attribute, input);
	else if (state == 2 || state == 3 || state == 7)			//COMP
		return classifier(tokenname[2], attribute, input);
	else if (state == 5)										//ASSIGN
		return classifier(tokenname[3], attribute, input);
	else
		return rewinder(input, nget);					//Final state�� �������� ���� ��� �ǰ���
}

bool isSYMBOL()
{
	char tokenname[6][7];			//token �̸�
	strcpy(tokenname[0], "SEMI");
	strcpy(tokenname[1], "LBKRT");
	strcpy(tokenname[2], "RBKRT");
	strcpy(tokenname[3], "LPAREN");
	strcpy(tokenname[4], "RPAREN");
	strcpy(tokenname[5], "COMMA");
	int state = 0;					//ó�� State �ʱ�ȭ
	int nget = 0;					//false�� ��ȯ�� ��� token�з��� �ȵ� ó�� ��ġ�� ���ư��� ����, ��ŭ ���ڸ� �о����� �����ش�

	while (1) {						//Documentation�� ������ DFA�� ����, ���ڸ� �ϳ� �а�, state�� �ٲٴ� �� �ݺ��Ѵ�
		read(nget);
		if (input == ';' && state == 0) state = 1;
		else if (input == '{' && state == 0) state = 2;
		else if (input == '}' && state == 0) state = 3;
		else if (input == '(' && state == 0) state = 4;
		else if (input == ')' && state == 0) state = 5;
		else if (input == ',' && state == 0) state = 6;
		else break;
		attribute[i++] = input;		//attribute ���ۿ� input ���ڸ� �߰�	
	}
	attribute[i++] = '\0';			//attribute �Է� ��
	i = 0;							//attribute ���� ������ ���ؼ� ÷�ڸ� �ʱ�ȭ

	if (state == 1)												//SEMI
		return classifier(tokenname[0], NULL, input);
	else if (state == 2)										//LBKRT
		return classifier(tokenname[1], NULL, input);
	else if (state == 3)										//RBKRT
		return classifier(tokenname[2], NULL, input);
	else if (state == 4)										//LPAREN
		return classifier(tokenname[3], NULL, input);
	else if (state == 5)										//RPAREN
		return classifier(tokenname[4], NULL, input);
	else if (state == 6)										//COMMA
		return classifier(tokenname[5], NULL, input);
	else
		return rewinder(input, nget);					//Final state�� �������� ���� ��� �ǰ���
}

int main(int argc, char *argv[])
{
	openfile(argv[1]);			//�ҽ� �ڵ��� �̸��� �μ��� ���޹޾� ������ ����
	makeoutputfile(argv[1]);	//output������ �����Ѵ�

	while (!isEND())			//���� ���� ������ ������ token �з��� �ݺ�
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
		print_error();					//��� �Ϳ��� �з����� �ʴ´ٸ� �������
		break;							//���� �ߴ�			
	}
	closefile();
}
