#include "rhhashing.h"
#include "rh_hahs_table.hpp"
#include<Windows.h>
#include<map>
using namespace  std;
multimap<int,int> mm;
typedef map<int,int> IntMapType;

int g_serial_id =0;
int main()
{	
	int arry[] = {1,57,20,81,59,};//48,36,90,83,75,18,86,72,52,31,2,10,37,15,17,99,45,12,1,1,38,54,58,61,61,17,67,46,36,6,61,79,81,52,31,88,73,96,93,54,15,47,24,87,21,78,85,100,100,62,40,27,30,85,3,38,10,68,6,1,92,28,28,59,70,84,73,49,21,75,47,46,95,75,11,60,39,74,61,58,37,16,23,43,81,52,99,76,35,17,66,50,7,70,51};

	int hash_table_size = 10;
	hash_table * ht = create_hash_table(6);
	IntMapType imt;

	printf("%d 's hash value is:%d \n",1,hash_function(1,6));
	printf("%d 's hash value is:%d \n",57,hash_function(57,6));
	printf("%d 's hash value is:%d \n",20,hash_function(20,6));
	printf("%d 's hash value is:%d \n",81,hash_function(81,6));
	printf("%d 's hash value is:%d \n",59,hash_function(59,6));


	unsigned long curr_t = GetTickCount();
	for(int times  = 0;times<10;times++)
		for (int i=0;i<250;i++)
		{
			int key = arry[i % 5];

			//printf("%d 's hash value is:%d \n",key,hash_function(key,6));
			int value = key * 10;
			if (ht->element_num==100)
			{
				printf("stop..");
			}
			if(key==20)
				printf("times is %d,i is %d,index is %d,key is %d\n",times,i,i%5,key);
			rhht_unique_overwrite_insert(ht,key,value);
			if (key % 2 == 0)
			{
				rhht_remove(ht,key);
			}		
		}
	printf(" ht size:	%d\n ",ht->element_num);
	printf("my hash table elipsed time is %d \n",GetTickCount() - curr_t);
	dump_hash_table(ht);

	curr_t = GetTickCount();
	for(int times  = 0;times<10;times++)
		for (int i=0;i<250;i++)
		{
			int key = arry[i % 5];
			int value = key * 10;
			imt.insert(make_pair(key,value));
			if (key % 2 == 0)
			{
				imt.erase(key);
			}		
		}

	printf(" imt size:	%d\n ",imt.size());
	printf("elipsed time is %d \n",GetTickCount() - curr_t);
 

	return 0;
}