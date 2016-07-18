/*
20160606_Extendible hash index 에 이용할 해시함수 테스트.
여러 해시함수에 대해서, 같은 화일을 해싱했을 때 충돌률을 비교하여
가장 좋은(적합한) 해시함수를 도출한다.
*/
#define _CRT_SECURE_NO_WARNINGS
#define MAX_BUFFER 1024
#define MAX_NAME 10 //이용할 키의 최대 길이
#define MAX_KEY 50000	//이용할 키들의 최대 개수

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
		printf("화일을 읽는데 실패했습니다. 프로그램을 종료합니다.\n");
		return 0;
	}

	else {
		while (fgets(buffer, sizeof(char)*MAX_BUFFER, fPtr) != NULL) {
			strcpy_s(Memory[count].name, MAX_NAME, strtok(buffer, "$"));
			count++;
		}
	}
	//Memory 에 모든 키 값을 저장함.
	printf("키 값의 개수 : %d.\n", count);
	printf("  Hash_charToInt : 단순히 char형 변수를 int형으로 바꿔 값을 모두 더함\n");
	collision_Count = collision_Test(Hash_charToInt, count, Memory);
	draw(count, collision_Count);

	printf("  Hash_seqProductToFirstName : '이름'에 비중을 주어 값을 모두 더함\n");
	collision_Count = collision_Test(Hash_seqProductToFirstName, count, Memory);
	draw(count, collision_Count);

	printf("  Hash_seqProductToLastName : '성'에 비중을 주어 값을 모두 더함\n");
	collision_Count = collision_Test(Hash_seqProductToLastName, count, Memory);
	draw(count, collision_Count);

	printf("  Hash_seqPFN : '이름'에 비중을 다른 비율로 주어 값을 모두 더함.\n");
	collision_Count = collision_Test(Hash_seqPFN, count, Memory);
	draw(count, collision_Count);

	system("pause");

}
//해시함수를 이용해 충돌횟수를 받았을 때 그 결과를 텍스트로 출력
void draw(int count, int collision_Count) {
	printf("충돌 횟수 : %d 회\n", collision_Count);
	printf("충돌 비율 : %.2f%%\n", ((float)collision_Count / count) * 100);
}
//x를 y만큼 제곱한 수를 반환한다. Recursive func.
int pow(int x, int y) {
	if (y > 1) return x * pow(x, y - 1);
	else return x;
}

//함수 포인터를 받아서 해당 함수로 해시인덱스를 만들었을 때 충돌하는 키의 개수를 반환하도록 설계.
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
	return count - index;	//충돌 개수.
}

int collision_Check(unsigned __int64 *hashIndex, int index_Count, unsigned __int64 key) {
	//정상일 때 return 0;
	for (int i = 0; i < index_Count; i++) {
		if (hashIndex[i] == key) {
			return -1;
		}
	}
	return 0;
}

//키의 코드값을 하나하나 정수형으로 변환하여 더한 후 출력한다. char형 변수의 코드값을 그대로 int형 변환.
unsigned __int64 Hash_charToInt(char* key) {
	unsigned __int64 tmpRtn=0;
	int strLength;
	strLength = strlen(key);
	for (int i = 0; i < strLength;i++)
		tmpRtn += key[i];
	return tmpRtn;
}

//키의 char형 코드값을 더하되 점점 더 많은 비율을 할당함.
//한글 이름의 특성상 비슷한 성이 많은 것에 착안하여 이름에 대한(키 뒷부분에 대한) '키값의 비율'을 높인다.
unsigned __int64 Hash_seqProductToFirstName(char* key) {
	unsigned __int64 tmpRtn = 0;
	int strLength;
	strLength = strlen(key);
	for (int i = 0; i < strLength; i++)
		tmpRtn += key[i]*(pow(10,i));
	return tmpRtn;
}

//비슷한 성이 많은 것에 착안하여 성에 대한(키 앞부분에 대한) '키값의 비율'을 높인다.
//seqProductToFirstName 의 반대.
unsigned __int64 Hash_seqProductToLastName(char* key) {
	unsigned __int64 tmpRtn = 0;
	int strLength;
	strLength = strlen(key);
	for (int i = 0; i < strLength; i++)
		tmpRtn += key[i] * (pow(10, strLength - i));
	return tmpRtn;
}



//키의 char형 코드값을 더하되 점점 더 많은 비율을 할당함.
//한글 이름의 특성상 비슷한 성이 많은 것에 착안하여 이름에 대한(키 뒷부분에 대한) '키값의 비율'을 높인다.
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