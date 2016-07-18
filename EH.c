/*
20160531_JJA
�켱 ȭ���� ������� �����ؾ��Ѵ�.
�ؽ��Լ��� �׽�Ʈ ����
���丮 / ��Ŷ ������ ��� ����
�ؽ� �ε��� ������ ǥ���� �� �ֵ��� �ؾ���

ȭ���� �Է�.


20160603_JJA
�����ؾ��� �� : �ؽ��Լ�, �ؽ��׽�Ʈ ȯ��, ���丮,��Ŷ ���ý���, �ε��� ����ǥ��
������ �� : ��ü���� �⺻ ����. �ؽ��Լ��� ������� int64�� �ξ��� bitstring�� ����� ���� �ϱ� ���� ��Ʈ�����ڸ� �̿��Ͽ���.
            ȭ���� �Է��� �����Ͽ���.
*/

/*ȭ���� �Է�*/
#define _CRT_SECURE_NO_WARNINGS
#define MAX_RECORD 100	//���ڵ��� �ִ� ����
#define MAX_NAME 10		//�̸��� �ִ� ����
#define MAX_PHONE 14	//��ȭ��ȣ�� �ִ� ����
#define MAX_ADDR 50		//�ּ��� �ִ� ����
#define MAX_DEPARTMENT 30	//�����а��� �ִ� ����
#define MAX_BUFFER	1024	//������ �ִ� ����
#define MAX_BUCKET 3		//������ �ִ� ũ��
#define DELIMETER "$"	//�Է��ڷ��� �ʵ� ������ ���� ������ ����
#define BKT_OVERFLOW 1	//��Ŷ �����÷ο�� 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

//�̿��� ���ڵ� Ÿ��
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

//�̿��� �Լ�
void draw();
void initEHI();
unsigned __int64 hashFunc(char* key);
int bkt_Split(BUCKET* bktPtr);
int dir_Double();
int add_To_Bucket(BUCKET* bktPtr, RECORD* recordPtr);
int extendible_Hash_Indexing(RECORD* recordPtr);
unsigned __int64 bitstring(unsigned __int64 key, int depth);
int pow(int x, int y);

//�̿��� ���� ����.
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

	//�켱 �Է�ȭ�� ��ü�� �о ���ڿ� �迭�� �ִ´�. �� �迭�� ������ second memory �̴�.
	fPtr = fopen("input file.txt", "r");
	if (fPtr == NULL) {
		printf("ȭ���� �дµ� �����߽��ϴ�. ���α׷��� �����մϴ�.\n");
		return 0;
	}
	//ȭ���� �дµ� ������ ���, second_Memory �迭�� �Է�ȭ���� ���ڵ��� ���Ŀ� �°� �����Ѵ�.
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

	//EH �ε����� �����Ѵ�.
	for (int i = 0; i < count;i++){
		if(extendible_Hash_Indexing(&second_Memory[i])==-1) break;
	}
	draw();
	system("pause");
	return 0;
}
//Ȯ�强 �ؽ� �ε��� ���Ͻý����� �ʱ� ����. 2���� ���丮,��Ŷ�� �ʱ�ȭ.
void initEHI() {
	//���� ���̴� 1, ���丮�� �ε��� ũ��� 2�� �̴ϼȶ���¡.
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
//Ȯ�强 �ؽ� �ε��� �Լ�.
int extendible_Hash_Indexing(RECORD* recordPtr) {
	unsigned __int64 bitstringedKey;
	unsigned __int64 hashed_Key = hashFunc(recordPtr->name);
	int resultAdd;	//addToBucket �� ����� ������ ����.
	BUCKET* targeted_Bkt = NULL;	//���ڵ尡 �� ���� �ּ�.

	//���� ���̿� ���� ���� Ű�� ��Ʈ��Ʈ���� �߶� �����Ѵ�.
	bitstringedKey = bitstring(hashed_Key, global_Depth);

	targeted_Bkt = dir_Ptr[bitstringedKey].bktPtr;

	if (targeted_Bkt == NULL) return -1;	//��ġ�Ǵ� ��Ʈ��Ʈ���� ���丮�� ���� ���. �������� ȯ�濡���� ���� �� ���� ���.

	resultAdd = add_To_Bucket(targeted_Bkt, recordPtr);

	if (resultAdd == 0);

	//��Ŷ�� �����÷ο� �� ��� split.
	else if (resultAdd == BKT_OVERFLOW) {
		if (bkt_Split(targeted_Bkt) == -1) {
			printf("Memory realloc ������ ������ ������ϴ�.\n ����� �޸𸮸� Ȯ�� �� �ٽ� �����Ͻʽÿ�.\n");
			return -1;
		}
		extendible_Hash_Indexing(recordPtr);
	}

	else return 0;
}
//��Ŷ ���� ������ش�.
void draw() {
	for (int i = 0; i < bkt_Count; i++) {
		printf("\nBucket %d---------------------------------------------------------------\n", i + 1);
		printf("   Bucket Key : %llu, Local depth : %d, Bucket : [%s, %s, %s]", bkt_Ptr[i].key, bkt_Ptr[i].local_Depth, bkt_Ptr[i].record_Ptr[0]->name, bkt_Ptr[i].record_Ptr[1]->name, bkt_Ptr[i].record_Ptr[2]->name);
	}
	printf("\n\nTotal directory : %d, Splitted : %d, Doubled : %d, Bucket usage : %.2f%%.\n", dir_Max, bkt_Count - 2, global_Depth - 1, (float)record_Count / (3 * bkt_Count) * 100.0);
}
//�ش� ��Ŷ�� ���ڵ带 �߰��Ѵ�. ��Ŷ�� �� á�ٸ� overflow ���� ��ȯ�Ѵ�.
int add_To_Bucket(BUCKET* bktPtr, RECORD* recordPtr) {
	
	for (int i = 0; i < 3; i++) {
		if (bktPtr->record_Ptr[i] == NULL) {
			bktPtr->record_Ptr[i] = recordPtr;
			return 0;
		}
	}
	return BKT_OVERFLOW;
}
//���丮�� ������ ���.
int dir_Double() {
	DIRECTORY* tmpPtr = NULL;
	global_Depth++;
	tmpPtr = (DIRECTORY*)realloc(dir_Ptr, sizeof(DIRECTORY) * dir_Max * 2);
	if (tmpPtr == NULL) {
		printf("Error_realloc.\n");
		return -1;
	}
	dir_Ptr = tmpPtr;
	//���� ���丮�� ���� �߰��� ���丮�� �����Ѵ�.
	for (int i = 0; i < dir_Max; i++) {
		dir_Ptr[i + dir_Max].bktPtr = dir_Ptr[i].bktPtr;
	}
	dir_Max *= 2;
	printf("Directory is doubled. Total directory count : %d\n",dir_Max);
	return 0;
}
//������ ���ø� �� ���.
int bkt_Split(BUCKET* bktPtr) {
	printf("Bucket(%d) is splitted.\n", (int)(bktPtr - bkt_Ptr)+1);
	BUCKET tmpBkt;
	BUCKET* tmpBktPtr = NULL;
	BUCKET* tmpPtr = NULL;
	int count=0;

	//���� ���� == ���� ���� �� ���, doubling directory.
	if (bktPtr->local_Depth == global_Depth) {
		if(dir_Double(bktPtr)==-1) return -1;
	}

	bkt_Count++;
	bktPtr->local_Depth++;
	tmpBkt = *bktPtr;
	for (int i = 0; i < MAX_BUCKET; i++) bktPtr->record_Ptr[i] = NULL;
	/*-----------------------------------------------------------------------------------------------
	�ش� ������ ���ø� ��Ų��. ������ �迭�� +1 ��Ų �迭�� realloc.
	�̷��� 1�� ������Ű�� realloc �� ���⵵�� O(n^2) �� ����. ������ ���ҵ��� ��� �����ϱ� �����̴�.
	�׷��Ƿ� ��ȿ�����̴�.
	-----------------------------------------------------------------------------------------------*/
	tmpPtr = (BUCKET*)realloc(bkt_Ptr, sizeof(BUCKET) * bkt_Count);
	if (tmpPtr == NULL) {
		printf("Error_realloc.\n");
		return -1;
	}
	bkt_Ptr = tmpPtr;
	//�� ��Ŷ�� ���� ���̸� ���ø��� �߱��� ��Ŷ ���ñ��� + 1 �� ���� �Ѵ�.
	bkt_Ptr[bkt_Count - 1].local_Depth = tmpBkt.local_Depth;
	bkt_Ptr[bkt_Count - 1].key = tmpBkt.key + pow(2, tmpBkt.local_Depth - 1);

	//�� ������ ���ڵ带 �ʱ�ȭ�Ѵ�.
	for (int i = 0; i < MAX_BUCKET; i++) bkt_Ptr[bkt_Count - 1].record_Ptr[i] = NULL;
	
	//realloc�� ���� bkt_Ptr�� �ּҰ� �ٲ���� ��� ���丮�ε��� ���� �ּҸ� ����ȭ�����ش�.
	if (bkt_Ptr != dir_Ptr[0].bktPtr) {
		for (int j = 0; j < bkt_Count; j++) {
			dir_Ptr[bkt_Ptr[j].key].bktPtr = &bkt_Ptr[j];
			if (bkt_Ptr[j].local_Depth < global_Depth) {
				for (int i = 0; i < pow(2,global_Depth - bkt_Ptr[j].local_Depth) - 1; i++)
					dir_Ptr[bkt_Ptr[j].key + (i + 1) * pow(2,bkt_Ptr[j].local_Depth)].bktPtr = &bkt_Ptr[j];
			}
		}
	}
	
	//���� �߰��� ��Ŷ�� ����� ���丮 �ε����� �����Ѵ�.
	for (int i = 0; i < dir_Max; i++) {
		if(bitstring(i,bkt_Ptr[bkt_Count-1].local_Depth)==bkt_Ptr[bkt_Count-1].key) 
			dir_Ptr[i].bktPtr = &bkt_Ptr[bkt_Count - 1];
	}

	//���ø��� �߻���Ų ������ ���ڵ���� ���ġ��Ų��.
	for (int j = 0; j < MAX_BUCKET; j++)	
		extendible_Hash_Indexing(tmpBkt.record_Ptr[j]);
	return 0;
}
//depth�� ���� key�� ������ bit ���� �߶� bitstring�� ��ȯ�Ѵ�.
unsigned __int64 bitstring(unsigned __int64 key, int depth) {
	int rtn = key & (pow(2, depth) - 1);
	return rtn;
}
/*-------------------------------------------------------------
�ؽ� �Լ�. �ѱ��� ��+�̸��̸� ���� ��κ� ����� ���� ����Ͽ�
key�� �պκ�(��)�� �� ū ������ �־� ������ ������ ��ȯ�Ѵ�.
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
//x�� y��ŭ ������ ���� ��ȯ�Ѵ�. Recursive func.
int pow(int x, int y) {
	if (y > 1) return x * pow(x, y - 1);
	else return x;
}
