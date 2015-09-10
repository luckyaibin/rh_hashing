#include "rhhashing.h"
#include "rh_hahs_table.hpp"
#include<map>
using namespace  std;
multimap<int,int> mm;


int g_serial_id =0;
int main()
{
	/*hash_table<int,int> ht;
	ht.insert(5,99);
	ht.insert(6,999);
	ht.insert(7,9999);
	ht.insert(7,99999);
	ht.dump();
	ht.erase(7);
	ht.dump();
	ht.find(7);
	ht.dump();
	int *v_find = ht.find(5);
	int *v = ht.find(4);*/
	for (int i=0;i<10;i++)
	{
		int v = hash_function(i,10);
		printf("[%d]=%d,\n ",i,v);
	}
	int hash_table_size = 10;
	hash_node * ht = create_hash_table(hash_table_size);

	dump_hash_table(ht,hash_table_size);
	rhht_insert(ht,hash_table_size,5);
	rhht_insert(ht,hash_table_size,6);
	rhht_insert(ht,hash_table_size,7);

	dump_hash_table(ht,hash_table_size);

	rhht_insert(ht,hash_table_size,7);
	rhht_insert(ht,hash_table_size,7);
	rhht_insert(ht,hash_table_size,7);
	rhht_insert(ht,hash_table_size,7);
	rhht_insert(ht,hash_table_size,7);
	rhht_insert(ht,hash_table_size,7);
	dump_hash_table(ht,hash_table_size);
	int index = __rhht_find(ht,hash_table_size,7);
	index = __rhht_find(ht,hash_table_size,6);
	index = __rhht_find(ht,hash_table_size,5);
	index = __rhht_find(ht,hash_table_size,555);
	return 0;
}