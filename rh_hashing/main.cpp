#include "rhhashing.h"
#include "rh_hahs_table.hpp"

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
	insert(ht,hash_table_size,5);
	insert(ht,hash_table_size,6);
	insert(ht,hash_table_size,7);

	dump_hash_table(ht,hash_table_size);

	insert(ht,hash_table_size,7);
	insert(ht,hash_table_size,7);
	insert(ht,hash_table_size,7);
	insert(ht,hash_table_size,7);
	insert(ht,hash_table_size,7);
	insert(ht,hash_table_size,7);
	dump_hash_table(ht,hash_table_size);

	return 0;
}