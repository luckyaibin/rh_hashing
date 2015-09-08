//////////////////////////////////////////////////////////////////////////
///From: https://gist.github.com/ssylvan/5538011,
///All rights reverved to the auther,and his file is just one copy.I'll delete it someday.
//////////////////////////////////////////////////////////////////////////

#define USE_ROBIN_HOOD_HASH 1
#define USE_SEPARATE_HASH_ARRAY 1
#include<malloc.h>

#include <functional>
#include<utility>
#include<stdio.h>
using namespace std;

typedef unsigned int uint32_t ;


inline unsigned int hash_function(int v)
{
	v = v + (v << 5);
	v = 255;
	return v;
}
template<class Key, class Value>
class hash_table
{
	
	static const int INITIAL_SIZE = 16;
	static const int LOAD_FACTOR_PERCENT = 90;

	struct elem
	{
		Key key;
		Value value;
		elem(Key&& k, Value&& v) : key(std::move(k)), value(std::move(v)) {}
		uint32_t hash;				
	};

	elem* __restrict buffer;
	int num_elems;
	int capacity;
	int resize_threshold;
	uint32_t mask;

	
	static uint32_t hash_key(const Key& key)
	{				
		//const std::hash<Key> hasher;
		//auto h = static_cast<uint32_t>(hasher(key));

		//// MSB is used to indicate a deleted elem, so
		//// clear it
		//h &= 0x7fffffff;

		//// Ensure that we never return 0 as a hash,
		//// since we use 0 to indicate that the elem has never
		//// been used at all.
		//h |= h==0;

		unsigned int h = hash_function(key);
		h |= h==0;
		return h; 
	}

	static bool is_deleted(uint32_t hash)
	{
		// MSB set indicates that this hash is a "tombstone"
		return (hash >> 31) != 0;
	}

	int desired_pos(uint32_t hash) const
	{
		return hash & mask;
	}

	int probe_distance(uint32_t hash, uint32_t slot_index) const
	{	
		int desired_position = desired_pos(hash);
		int prodist = (slot_index + capacity - desired_position);
		prodist = prodist & mask;
		return  prodist;
	}

	uint32_t& elem_hash(int ix)
	{
	return buffer[ix].hash;
	}

	uint32_t elem_hash(int ix) const
	{
		return const_cast<hash_table*>(this)->elem_hash(ix);
	}

	// alloc buffer according to currently set capacity
	void alloc()
	{		
		buffer = reinterpret_cast<elem*>(_aligned_malloc(capacity*sizeof(elem), __alignof(elem)));
		memset(buffer,0,capacity*sizeof(elem));
		// flag all elems as free
		for( int i = 0; i < capacity; ++i)
		{
			elem_hash(i) = 0;
		}

		resize_threshold = (capacity * LOAD_FACTOR_PERCENT) / 100; 
		mask = capacity - 1;
	}

	void grow()
	{
		elem* old_elems = buffer;
		int old_capacity = capacity;
		capacity *= 2;		
		alloc();

		// now copy over old elems
		for(int i = 0; i < old_capacity; ++i)
		{
			auto& e = old_elems[i];
			uint32_t hash = e.hash;
			if (hash != 0 && !is_deleted(hash))
			{
				insert_helper(hash, std::move(e.key), std::move(e.value));
				e.~elem();
			}
		}

		_aligned_free(old_elems);		
	}

	void construct(int ix, uint32_t hash, Key&& key, Value&& val)
	{
		new (&buffer[ix]) elem(std::move(key), std::move(val));	
		elem_hash(ix) = hash;
	}

	void insert_helper(uint32_t hash, Key&& key, Value&& val)
	{
		int pos = desired_pos(hash);
		int dist = 0;
		for(;;)
		{			
			if(elem_hash(pos) == 0)
			{			
				construct(pos, hash, std::move(key), std::move(val));
				return;
			}

			// If the existing elem has probed less than us, then swap places with existing
			// elem, and keep going to find another slot for that elem.
			int existing_elem_probe_dist = probe_distance(elem_hash(pos), pos);
			if (existing_elem_probe_dist < dist)
			{	
				if(is_deleted(elem_hash(pos)))
				{
					construct(pos, hash, std::move(key), std::move(val));
					return;
				}

				std::swap(hash, elem_hash(pos));
				std::swap(key, buffer[pos].key);
				std::swap(val, buffer[pos].value);
				dist = existing_elem_probe_dist;				
			}

			pos = (pos+1) & mask;
			++dist;			
		}
	}

	int lookup_index(const Key& key) const
	{
		const uint32_t hash = hash_key(key);
		int pos = desired_pos(hash);
		int dist = 0;
		for(;;)
		{							
			if (elem_hash(pos) == 0) 
				return -1;
			else if (dist > probe_distance(elem_hash(pos), pos)) 
				return -1;
			else if (elem_hash(pos) == hash && buffer[pos].key == key) 
				return pos;				

			pos = (pos+1) & mask;
			++dist;
		}
	}

public:
	hash_table() : buffer(nullptr), num_elems(0), capacity(INITIAL_SIZE)
	{
		alloc();
	}

	void insert(Key key, Value val)
	{		
		if (++num_elems >= resize_threshold)
		{
			grow();
		}		
		insert_helper(hash_key(key), std::move(key), std::move(val));		
	}

	void dump()
	{
		for (int i=0;i<capacity;i++)
		{
			elem e = buffer[i];
			printf("	[%d,%d:%d]",e.key,e.value,e.hash);
		}
		printf("\n\n");
	}

	~hash_table()
	{		
		for( int i = 0; i < capacity; ++i)
		{
			if (elem_hash(i) != 0)
			{
				buffer[i].~elem();
			}
		}
		_aligned_free(buffer);
	}

	Value* find(const Key& key)
	{
		const uint32_t hash = hash_key(key);
		const int ix = lookup_index(key);
		return ix != -1 ? &buffer[ix].value : nullptr;
	}

	const Value* find(const Key& key) const
	{
		return const_cast<hash_table*>(this)->lookup(key);
	}

	bool erase(const Key& key)
	{
		const uint32_t hash = hash_key(key);
		const int ix = lookup_index(key);

		if (ix == -1) return false;

		buffer[ix].~elem();
		elem_hash(ix) |= 0x80000000; // mark as deleted
		--num_elems;
		return true;
	}

	int size() const
	{
		return num_elems;
	}

	float average_probe_count() const
	{
		float probe_total = 0;
		for(int i = 0; i < capacity; ++i)
		{
			uint32_t hash = elem_hash(i);
			if (hash != 0 && !is_deleted(hash))
			{
				probe_total += probe_distance(hash, i);
			}
		}
		return probe_total / size() + 1.0f;
	}
};