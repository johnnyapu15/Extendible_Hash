/*
20160606_Extendible hash index �� �̿��� �ؽ��Լ� �׽�Ʈ.
���� �ؽ��Լ��� ���ؼ�, ���� ȭ���� �ؽ����� �� �浹���� ���Ͽ�
���� ����(������) �ؽ��Լ��� �����Ѵ�.
*/
#define _CRT_SECURE_NO_WARNINGS
#define MAX_BUFFER 1024
#define MAX_NAME 10 //�̿��� Ű�� �ִ� ����
#define MAX_KEY 50000	//�̿��� Ű���� �ִ� ����

#include <stdio.h>
#include <string.h>
#include <Windows.h>

typedef struct key {
	char name[MAX_NAME];
}Key;

int pow(int x, int y);
int collision_Check(unsigned __int64 *hashIndex, int index_Count, unsigned __int64 key);
int collision_Test(unsigned __int64(*funcPtr)(char*), int count, Key* Memory);
unsigned __int64 Hash_charToInt(char* key);
unsigned __int64 Hash_seqProductToFirstName(char* key);
unsigned __int64 Hash_seqProductToLastName(char* key);
unsigned __int64 Hash_seqPFN(char* key);
void draw(int count, int collision_Count);

int main() {
	FILE *fPtr;
	char buffer[MAX_BUFFER];
	int count = 0;
	Key Memory[MAX_KEY];
	int collision_Count;
	//unsigned __int64 hashIndex[MAX_KEY];

	collision_Count = 0;

	fPtr = fopen("hash test file.txt", "r");
	if (fPtr == NULL) {
		printf("ȭ���� �дµ� �����߽��ϴ�. ���α׷��� �����մϴ�.\n");
		return 0;
	}

	else {
		while (fgets(buffer, sizeof(char)*MAX_BUFFER, fPtr) != NULL) {
			strcpy_s(Memory[count].name, MAX_NAME, strtok(buffer, "$"));
			count++;
		}
	}
	//Memory �� ��� Ű ���� ������.
	printf("Ű ���� ���� : %d.\n", count);
	printf("  Hash_charToInt : �ܼ��� char�� ������ int������ �ٲ� ���� ��� ����\n");
	collision_Count = collision_Test(Hash_charToInt, count, Memory);
	draw(count, collision_Count);

	printf("  Hash_seqProductToFirstName : '�̸�'�� ������ �־� ���� ��� ����\n");
	collision_Count = collision_Test(Hash_seqProductToFirstName, count, Memory);
	draw(count, collision_Count);

	printf("  Hash_seqProductToLastName : '��'�� ������ �־� ���� ��� ����\n");
	collision_Count = collision_Test(Hash_seqProductToLastName, count, Memory);
	draw(count, collision_Count);

	printf("  Hash_seqPFN : '�̸�'�� ������ �ٸ� ������ �־� ���� ��� ����.\n");
	collision_Count = collision_Test(Hash_seqPFN, count, Memory);
	draw(count, collision_Count);

	system("pause");

}
//�ؽ��Լ��� �̿��� �浹Ƚ���� �޾��� �� �� ����� �ؽ�Ʈ�� ���
void draw(int count, int collision_Count) {
	printf("�浹 Ƚ�� : %d ȸ\n", collision_Count);
	printf("�浹 ���� : %.2f%%\n", ((float)collision_Count / count) * 100);
}
//x�� y��ŭ ������ ���� ��ȯ�Ѵ�. Recursive func.
int pow(int x, int y) {
	if (y > 1) return x * pow(x, y - 1);
	else return x;
}

//�Լ� �����͸� �޾Ƽ� �ش� �Լ��� �ؽ��ε����� ������� �� �浹�ϴ� Ű�� ������ ��ȯ�ϵ��� ����.
int collision_Test(unsigned __int64 (*funcPtr)(char*), int count, Key* Memory){
	unsigned __int64 hashIndex[MAX_KEY];
	int index = 0;
	unsigned __int64 tempKey=0;
	for (int i = 0; i < count; i++) {
		if (!collision_Check(hashIndex, index, tempKey = funcPtr(Memory[i].name))) {
			hashIndex[index] = tempKey;
			index++;
		}
	}
	return count - index;	//�浹 ����.
}

int collision_Check(unsigned __int64 *hashIndex, int index_Count, unsigned __int64 key) {
	//������ �� return 0;
	for (int i = 0; i < index_Count; i++) {
		if (hashIndex[i] == key) {
			return -1;
		}
	}
	return 0;
}

//Ű�� �ڵ尪�� �ϳ��ϳ� ���������� ��ȯ�Ͽ� ���� �� ����Ѵ�. char�� ������ �ڵ尪�� �״�� int�� ��ȯ.
unsigned __int64 Hash_charToInt(char* key) {
	unsigned __int64 tmpRtn=0;
	int strLength;
	strLength = strlen(key);
	for (int i = 0; i < strLength;i++)
		tmpRtn += key[i];
	return tmpRtn;
}

//Ű�� char�� �ڵ尪�� ���ϵ� ���� �� ���� ������ �Ҵ���.
//�ѱ� �̸��� Ư���� ����� ���� ���� �Ϳ� �����Ͽ� �̸��� ����(Ű �޺κп� ����) 'Ű���� ����'�� ���δ�.
unsigned __int64 Hash_seqProductToFirstName(char* key) {
	unsigned __int64 tmpRtn = 0;
	int strLength;
	strLength = strlen(key);
	for (int i = 0; i < strLength; i++)
		tmpRtn += key[i]*(pow(10,i));
	return tmpRtn;
}

//����� ���� ���� �Ϳ� �����Ͽ� ���� ����(Ű �պκп� ����) 'Ű���� ����'�� ���δ�.
//seqProductToFirstName �� �ݴ�.
unsigned __int64 Hash_seqProductToLastName(char* key) {
	unsigned __int64 tmpRtn = 0;
	int strLength;
	strLength = strlen(key);
	for (int i = 0; i < strLength; i++)
		tmpRtn += key[i] * (pow(10, strLength - i));
	return tmpRtn;
}



//Ű�� char�� �ڵ尪�� ���ϵ� ���� �� ���� ������ �Ҵ���.
//�ѱ� �̸��� Ư���� ����� ���� ���� �Ϳ� �����Ͽ� �̸��� ����(Ű �޺κп� ����) 'Ű���� ����'�� ���δ�.
unsigned __int64 Hash_seqPFN(char* key) {
	unsigned __int64 tmpRtn = 0;
	int strLength;
	strLength = strlen(key);
	tmpRtn += key[0];
	tmpRtn += key[1];
	for (int i = 2; i < strLength; i++)
		tmpRtn += key[i] * (pow(2, (i-2)*8));
	return tmpRtn;
}