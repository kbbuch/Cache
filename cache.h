#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <string.h>
#include<stdbool.h>

#define LRU 0
#define FIFO 1
#define LFU 2
#define psuedo_LRU 3

#define NINE 0
#define INCLUSIVE 1
#define EXCLUSIVE 2

class cache{
	
	private:
		
		int reads;
		int read_miss;
		
		int writes;
		int write_miss;
		
		float miss_rate;
		
		int writebacks;
		
		//parameters
		int number_of_sets;
		int index_bits;
		int offset_bits;
		
		int mask;
		
		//2-D arrays
		bool **valid_bit;
		bool **dirty_bit;
		long long int **tag_array;
		int **age;
		
		int blockshift;
		
		int empty_block;
		int hit_block;
		
		char* name;
	
	public:
	
		//public members
		int size;
		int associativity;
		
		long long int tag;
		int index;
		
		//constructor
		cache(int size_in, int assoc_in, int blocksize);
		
		//destructor
		~cache();
		
		//splitting the address and extracting index
		void split_address(long long int address);

		//updating read and writes for L1
		void update_read_write(char operation);
		
		//increment age of all blocks of a given cache line
		void age_increment(int index);
		
		//checking for a hit while reading L1
		bool is_a_hit(int index, long long int tag);
		
		//updating the cache stats on a hit
		void update_on_hit(int index, char operation, int replacement_policy);
	
		//checking if the cacheline has any space left
		bool is_cacheline_empty(int index);
		
		//installing block when line found empty
		void install_block(int index, long long int tag, char operation, int replacement_policy);
		
		//updating according to LRU
		void LRU_update(int index, long long int tag, char operation);
		
		//updating according to FIFO
		void FIFO_update(int index, long long int tag, char operation);
		
		//updating according to LFU
		void LFU_update(int index, long long int tag, char operation);
		
		//updating according to psuedo LRU
		
		
		//printing stats
		void print_stats(const char* name);
		
		//debug print cache
		void debug_print();
		
		//reconstructing the address for the evicted block from L1
		long long int reconstruct_address(long long int tag_temp);
		
		//Full read/write operation on a cache
		void operate_on_cache(char operation, int replacement_policy);
		
		//act according to inclusion policy
		void action_acc_to_inclusion_policy(char L2_op, int replacement_policy, int inclusion_policy);
		
		//installing block along with check for L2
		void install_block(int index, long long int tag, char operation, int replacement_policy, int inclusion_policy, cache *L2);
		
		//updating according to LRU for L1+L2
		void LRU_update(int index, long long int tag, char operation, int replacement_policy, int inclusion_policy, cache *L2);
};

