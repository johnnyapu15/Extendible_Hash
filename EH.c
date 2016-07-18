/*
20160531_JJA
우선 화일의 입출력을 구현해야한다.
해싱함수의 테스트 구현
디렉토리 / 버킷 포인터 계산 구현
해싱 인덱스 구조를 표현할 수 있도록 해야함

화일의 입력.


20160603_JJA
구현해야할 것 : 해싱함수, 해싱테스트 환경, 디렉토리,버킷 계산시스템, 인덱스 구조표현
구현한 것 : 전체적인 기본 설계. 해시함수의 결과값을 int64로 두었고 bitstring의 계산을 쉽게 하기 위해 비트연산자를 이용하였다.
            화일의 입력을 구현하였다.
*/

/*화일의 입력*/
#define _CRT_SECURE_NO_WARNINGS
#define MAX_RECORD 100	//레코드의 최대 개수
#define MAX_NAME 10		//이름의 최대 길이
#define MAX_PHONE 14	//전화번호의 최대 길이
#define MAX_ADDR 50		//주소의 최대 길이
#define MAX_DEPARTMENT 30	//전공학과의 최대 길이
#define MAX_BUFFER	1024	//버퍼의 최대 길이
#define MAX_BUCKET 3		//버켓의 최대 크기
#define DELIMETER "$"	//입력자료의 필드 구분을 위한 구분자 설정
#define BKT_OVERFLOW 1	//버킷 오버플로우시 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

//이용할 레코드 타입
typedef struct record {
	char name[MAX_NAME];
	char phone[MAX_PHONE];
	char addr[MAX_ADDR];
	char department[MAX_DEPARTMENT];
}RECORD;

typedef struct bucket{
	int local_Depth;
	unsigned __int64 key;
	RECORD* record_Ptr[MAX_BUCKET];
}BUCKET;

typedef struct directory {
	BUCKET* bktPtr;
}DIRECTORY;

//이용할 함수
void draw();
void initEHI();
unsigned __int64 hashFunc(char* key);
int bkt_Split(BUCKET* bktPtr);
int dir_Double();
int add_To_Bucket(BUCKET* bktPtr, RECORD* recordPtr);
int extendible_Hash_Indexing(RECORD* recordPtr);
unsigned __int64 bitstring(unsigned __int64 key, int depth);
int pow(int x, int y);

//이용할 전역 변수.
int record_Count;
int global_Depth;
int dir_Max;
int bkt_Count;
DIRECTORY *dir_Ptr;
BUCKET *bkt_Ptr;


int main() {
	FILE *fPtr;
	RECORD second_Memory[MAX_RECORD];
	char buffer[MAX_BUFFER];
	char* tokenPtr = NULL;
	int count = 0;

	//우선 입력화일 전체를 읽어서 문자열 배열에 넣는다. 이 배열은 가상의 second memory 이다.
	fPtr = fopen("input file.txt", "r");
	if (fPtr == NULL) {
		printf("화일을 읽는데 실패했습니다. 프로그램을 종료합니다.\n");
		return 0;
	}
	//화일을 읽는데 성공할 경우, second_Memory 배열에 입력화일을 레코드의 형식에 맞게 저장한다.
	else {

			while (fgets(buffer, sizeof(char)*MAX_BUFFER, fPtr) != NULL) {
				strcpy_s(second_Memory[count].name, (rsize_t)sizeof(char)*MAX_NAME, strtok_s(buffer, DELIMETER, &tokenPtr));
				if(*tokenPtr != NULL)
				strcpy_s(second_Memory[count].phone, (rsize_t)sizeof(char)*MAX_PHONE, strtok_s(NULL, DELIMETER, &tokenPtr));
				if (*tokenPtr != NULL)
				strcpy_s(second_Memory[count].addr, (rsize_t)sizeof(char)*MAX_ADDR, strtok_s(NULL, DELIMETER, &tokenPtr));
				if (*tokenPtr != NULL)
				strcpy_s(second_Memory[count].department, (rsize_t)sizeof(char)*MAX_DEPARTMENT, tokenPtr);
				tokenPtr = NULL;
				count++;
			}
	}
	record_Count = count;
	initEHI();

	//EH 인덱싱을 진행한다.
	for (int i = 0; i < count;i++){
		if(extendible_Hash_Indexing(&second_Memory[i])==-1) break;
	}
	draw();
	system("pause");
	return 0;
}
//확장성 해시 인덱스 파일시스템의 초기 설정. 2개의 디렉토리,버킷을 초기화.
void initEHI() {
	//전역 깊이는 1, 디렉토리의 인덱스 크기는 2로 이니셜라이징.
	global_Depth = 1;
	bkt_Count = 2;
	dir_Ptr = (DIRECTORY*)calloc(2, sizeof(DIRECTORY));
	bkt_Ptr = (BUCKET*)calloc(2, sizeof(BUCKET));

	dir_Max = 2;
	bkt_Ptr[0].key = 0;
	dir_Ptr[0].bktPtr = &bkt_Ptr[0];

	bkt_Ptr[1].key = 1;
	dir_Ptr[1].bktPtr = &bkt_Ptr[1];

	bkt_Ptr[0].local_Depth = 1;
	bkt_Ptr[1].local_Depth = 1;
	for (int i = 0; i < 3; i++) {
		bkt_Ptr[0].record_Ptr[i] = NULL;
		bkt_Ptr[1].record_Ptr[i] = NULL;
	}
}
//확장성 해시 인덱스 함수.
int extendible_Hash_Indexing(RECORD* recordPtr) {
	unsigned __int64 bitstringedKey;
	unsigned __int64 hashed_Key = hashFunc(recordPtr->name);
	int resultAdd;	//addToBucket 의 결과를 저장할 변수.
	BUCKET* targeted_Bkt = NULL;	//레코드가 들어갈 버켓 주소.

	//전역 깊이에 따라 들어온 키의 비트스트링을 잘라서 저장한다.
	bitstringedKey = bitstring(hashed_Key, global_Depth);

	targeted_Bkt = dir_Ptr[bitstringedKey].bktPtr;

	if (targeted_Bkt == NULL) return -1;	//매치되는 비트스트링이 디렉토리에 없는 경우. 정상적인 환경에서는 있을 수 없는 경우.

	resultAdd = add_To_Bucket(targeted_Bkt, recordPtr);

	if (resultAdd == 0);

	//버킷이 오버플로우 된 경우 split.
	else if (resultAdd == BKT_OVERFLOW) {
		if (bkt_Split(targeted_Bkt) == -1) {
			printf("Memory realloc 과정중 오류가 생겼습니다.\n 충분한 메모리를 확보 후 다시 실행하십시오.\n");
			return -1;
		}
		extendible_Hash_Indexing(recordPtr);
	}

	else return 0;
}
//버킷 값을 출력해준다.
void draw() {
	for (int i = 0; i < bkt_Count; i++) {
		printf("\nBucket %d---------------------------------------------------------------\n", i + 1);
		printf("   Bucket Key : %llu, Local depth : %d, Bucket : [%s, %s, %s]", bkt_Ptr[i].key, bkt_Ptr[i].local_Depth, bkt_Ptr[i].record_Ptr[0]->name, bkt_Ptr[i].record_Ptr[1]->name, bkt_Ptr[i].record_Ptr[2]->name);
	}
	printf("\n\nTotal directory : %d, Splitted : %d, Doubled : %d, Bucket usage : %.2f%%.\n", dir_Max, bkt_Count - 2, global_Depth - 1, (float)record_Count / (3 * bkt_Count) * 100.0);
}
//해당 버킷에 레코드를 추가한다. 버킷이 꽉 찼다면 overflow 값을 반환한다.
int add_To_Bucket(BUCKET* bktPtr, RECORD* recordPtr) {
	
	for (int i = 0; i < 3; i++) {
		if (bktPtr->record_Ptr[i] == NULL) {
			bktPtr->record_Ptr[i] = recordPtr;
			return 0;
		}
	}
	return BKT_OVERFLOW;
}
//디렉토리를 더블링할 경우.
int dir_Double() {
	DIRECTORY* tmpPtr = NULL;
	global_Depth++;
	tmpPtr = (DIRECTORY*)realloc(dir_Ptr, sizeof(DIRECTORY) * dir_Max * 2);
	if (tmpPtr == NULL) {
		printf("Error_realloc.\n");
		return -1;
	}
	dir_Ptr = tmpPtr;
	//이전 디렉토리를 새로 추가된 디렉토리에 복사한다.
	for (int i = 0; i < dir_Max; i++) {
		dir_Ptr[i + dir_Max].bktPtr = dir_Ptr[i].bktPtr;
	}
	dir_Max *= 2;
	printf("Directory is doubled. Total directory count : %d\n",dir_Max);
	return 0;
}
//버켓을 스플릿 할 경우.
int bkt_Split(BUCKET* bktPtr) {
	printf("Bucket(%d) is splitted.\n", (int)(bktPtr - bkt_Ptr)+1);
	BUCKET tmpBkt;
	BUCKET* tmpBktPtr = NULL;
	BUCKET* tmpPtr = NULL;
	int count=0;

	//로컬 깊이 == 전역 길이 일 경우, doubling directory.
	if (bktPtr->local_Depth == global_Depth) {
		if(dir_Double(bktPtr)==-1) return -1;
	}

	bkt_Count++;
	bktPtr->local_Depth++;
	tmpBkt = *bktPtr;
	for (int i = 0; i < MAX_BUCKET; i++) bktPtr->record_Ptr[i] = NULL;
	/*-----------------------------------------------------------------------------------------------
	해당 버켓을 스플릿 시킨다. 버켓의 배열을 +1 시킨 배열로 realloc.
	이렇게 1씩 증가시키는 realloc 은 복잡도가 O(n^2) 로 간다. 기존의 원소들을 모두 복사하기 때문이다.
	그러므로 비효율적이다.
	-----------------------------------------------------------------------------------------------*/
	tmpPtr = (BUCKET*)realloc(bkt_Ptr, sizeof(BUCKET) * bkt_Count);
	if (tmpPtr == NULL) {
		printf("Error_realloc.\n");
		return -1;
	}
	bkt_Ptr = tmpPtr;
	//새 버킷의 로컬 깊이를 스플릿을 야기한 버킷 로컬깊이 + 1 과 같게 한다.
	bkt_Ptr[bkt_Count - 1].local_Depth = tmpBkt.local_Depth;
	bkt_Ptr[bkt_Count - 1].key = tmpBkt.key + pow(2, tmpBkt.local_Depth - 1);

	//새 버켓의 레코드를 초기화한다.
	for (int i = 0; i < MAX_BUCKET; i++) bkt_Ptr[bkt_Count - 1].record_Ptr[i] = NULL;
	
	//realloc에 의해 bkt_Ptr의 주소가 바뀌었을 경우 디렉토리인덱스 내의 주소를 동기화시켜준다.
	if (bkt_Ptr != dir_Ptr[0].bktPtr) {
		for (int j = 0; j < bkt_Count; j++) {
			dir_Ptr[bkt_Ptr[j].key].bktPtr = &bkt_Ptr[j];
			if (bkt_Ptr[j].local_Depth < global_Depth) {
				for (int i = 0; i < pow(2,global_Depth - bkt_Ptr[j].local_Depth) - 1; i++)
					dir_Ptr[bkt_Ptr[j].key + (i + 1) * pow(2,bkt_Ptr[j].local_Depth)].bktPtr = &bkt_Ptr[j];
			}
		}
	}
	
	//새로 추가된 버킷에 연결될 디렉토리 인덱스를 연결한다.
	for (int i = 0; i < dir_Max; i++) {
		if(bitstring(i,bkt_Ptr[bkt_Count-1].local_Depth)==bkt_Ptr[bkt_Count-1].key) 
			dir_Ptr[i].bktPtr = &bkt_Ptr[bkt_Count - 1];
	}

	//스플릿을 발생시킨 버켓의 레코드들을 재배치시킨다.
	for (int j = 0; j < MAX_BUCKET; j++)	
		extendible_Hash_Indexing(tmpBkt.record_Ptr[j]);
	return 0;
}
//depth에 따라 key를 오른쪽 bit 부터 잘라 bitstring을 반환한다.
unsigned __int64 bitstring(unsigned __int64 key, int depth) {
	int rtn = key & (pow(2, depth) - 1);
	return rtn;
}
/*-------------------------------------------------------------
해쉬 함수. 한글이 성+이름이며 성은 대부분 비슷한 점을 고려하여
key의 앞부분(성)에 더 큰 비중을 주어 정수형 변수로 변환한다.
-------------------------------------------------------------*/
unsigned __int64 hashFunc(char* key) {
	unsigned __int64 tmpRtn = 0;
	int strLength;
	strLength = strlen(key);

	for (int i = 0; i < strLength; i++)
		tmpRtn += key[i] * (pow(10, strLength - i)) + i;
	tmpRtn = tmpRtn;
	return tmpRtn;
}
//x를 y만큼 제곱한 수를 반환한다. Recursive func.
int pow(int x, int y) {
	if (y > 1) return x * pow(x, y - 1);
	else return x;
}
