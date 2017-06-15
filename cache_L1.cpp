#include "cache.h"

cache::cache(int size_in, int assoc_in, int blocksize){
	
	size = size_in;
	associativity = assoc_in;
	
	reads = 0;
	read_miss = 0;
	
	writes = 0;
	write_miss = 0;
	cache_empty = 0;
	
	lru = 0;
	
	miss_rate = 0;
	writebacks = 0;
	address = 0;
	is_evicted = false;
	
	if(associativity == 0)
		number_of_sets = 0;
	else
		number_of_sets=(size)/(associativity * blocksize); 
	
	//printf("Number of sets = %d and associativity = %d\n", number_of_sets, associativity);

	index_bits = (log(number_of_sets))/log(2);
	offset_bits = (log(blocksize))/log(2);
	
	mask = pow(2,index_bits)-1;
	
	tag_array = new long long int*[number_of_sets];
	valid_bit = new bool*[number_of_sets];
	dirty_bit = new bool*[number_of_sets];
	age = new int*[number_of_sets];
	
	for(int i=0; i < number_of_sets; i++){
		
		tag_array[i] = new long long int[associativity];
		valid_bit[i] = new bool[associativity];
		dirty_bit[i] = new bool[associativity];
		age[i] = new int[associativity];
	}
	if(associativity == 16)
	for(int i = 0; i < number_of_sets; i++){
		for(int j = 0; j < associativity; j++){
			tag_array[i][j] = 0;
			age[i][j] = 0;
			valid_bit[i][j] = false;
			dirty_bit[i][j] = false;
		}
	}
	//printf("Size of tag array = %d\t\n",sizeof(tag_array));
}

cache:: ~cache(){
	
	free(tag_array);
	free(valid_bit);
	free(dirty_bit);
	free(age);
	
}

void cache::split_address(long long int address){
	
	tag = address >> (offset_bits + index_bits);
	blockshift = address >> offset_bits;
	index = blockshift & mask;	
	
}

void cache::update_read_write(char operation){
	
	if(operation == 'r')
		reads++;
	else if(operation == 'w')
		writes++;
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          error in tracefile          \n-x-x-x-x-x--x-x-x-x-x-x-x\n");
		exit(0);
	}
}

bool cache::is_a_hit(int index, long long int tag){
	for(int i=0; i < associativity; i++){
		if(valid_bit[index][i] == true && tag_array[index][i] == tag){
			hit_block = i;			
			return true;
		}
	}
	
	return false;
}

void cache::update_on_hit(int index, char operation, int replacement_policy){
	
	if(replacement_policy != 2)
		age_increment(index);
	
	if(valid_bit[index][hit_block] == true && tag_array[index][hit_block] == tag){
		if(replacement_policy == 0){
			
			age[index][hit_block] = 1;
			
//printf("y no dirty bit high?\n");
			if(operation == 'w'){
				dirty_bit[index][hit_block] = true;
				//printf("dirty bit[%d][%d] = %d", index, hit_block, dirty_bit[index][hit_block]);
			}
				//dirty_bit[index][hit_block] = true;
		}
		else if(replacement_policy == 1){
			//do nothing to the age
			if(operation == 'w')
				dirty_bit[index][hit_block] = true;
		}
		else if(replacement_policy == 2){
			
			age[index][hit_block]++;
			
			if(operation == 'w')
				dirty_bit[index][hit_block] = true;
		}
		else if(replacement_policy == 3){
			
		}
	}
}

bool cache::is_cacheline_empty(int index){

	for(int i=0; i < associativity; i++){
		if(valid_bit[index][i] == false){
			empty_block = i;
			return true;
		}
	}
	
	return false;
}

void cache::install_block(int index, long long int tag, char operation, int replacement_policy){
	
	if(replacement_policy != 2)
		age_increment(index);

	tag_array[index][empty_block] = tag;
	valid_bit[index][empty_block] = true;
	age[index][empty_block] = 1;
	
	if(operation == 'w'){
		write_miss++;
		dirty_bit[index][empty_block] = true;
	}
	else if(operation == 'r'){
		read_miss++;
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in operation        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
	
}

void cache::install_block(int index, long long int tag, char operation, int replacement_policy, int inclusion_policy, bool d_b){
	
	if(replacement_policy != 2)
		age_increment(index);
	
	tag_array[index][empty_block] = tag;
	valid_bit[index][empty_block] = true;
	age[index][empty_block] = 1;
	dirty_bit[index][empty_block] = d_b;
	
	if(operation == 'w'){
		write_miss++;
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in operation        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}

void cache::LRU_update(int index, long long int tag, char operation){
	
	int max = -200, evict_block = 0;
	
	for(int i = 0; i < associativity; i++){
		if(age[index][i] > max){
			max = age[index][i];
			evict_block = i;
		}	
	}
	
	age_increment(index);
	
	if(dirty_bit[index][evict_block] == true)
		writebacks++;
	
	tag_array[index][evict_block] = tag;
	age[index][evict_block] = 1;
	
	if(operation == 'w'){
		write_miss++;
		dirty_bit[index][evict_block] = true;
	}
	else if(operation == 'r'){
		read_miss++;
		dirty_bit[index][evict_block] = false;
	}
	else{		
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in operation        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}

void cache::LRU_update(int index, long long int tag, char operation, bool d_b){
	
	int max = -200, evict_block = 0;
	
	for(int i = 0; i < associativity; i++){
		if(age[index][i] > max){
			max = age[index][i];
			evict_block = i;
		}	
	}
	
	age_increment(index);
	
	if(dirty_bit[index][evict_block] == true)
		writebacks++;
	
	tag_array[index][evict_block] = tag;
	age[index][evict_block] = 1;
	dirty_bit[index][evict_block] = d_b;
	
	if(operation == 'w'){
		write_miss++;
	}
	else{		
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in operation        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}

void cache::FIFO_update(int index, long long int tag, char operation){
	
	int max = -200, evict_block = 0;
	
	for(int i = 0; i < associativity; i++){
		if(age[index][i] > max){
			max = age[index][i];
			evict_block = i;
		}	
	}
	
	age_increment(index);
	
	if(dirty_bit[index][evict_block] == true)
		writebacks++;
	
	tag_array[index][evict_block] = tag;
	age[index][evict_block] = 1;
	
	if(operation == 'w'){
		write_miss++;
		dirty_bit[index][evict_block] = true;
	}
	else if(operation == 'r'){
		read_miss++;
		dirty_bit[index][evict_block] = false;
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in operation        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}

void cache::LFU_update(int index, long long int tag, char operation){
	
	int min = age[index][0], evict_block = 0;
	
	for(int j=1; j < associativity; j++)
	{
		if(age[index][j] < min){
			min = age[index][j];
			evict_block = j;
		}
	}
	
	if(dirty_bit[index][evict_block] == true)
		writebacks++;
	
	tag_array[index][evict_block] = tag;
	age[index][evict_block] = 1 + min;
	
	if(operation == 'w'){
		write_miss++;
		dirty_bit[index][evict_block] = true;
	}
	else if(operation == 'r'){
		read_miss++;
		dirty_bit[index][evict_block] = false;
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in operation        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}

void cache::operate_on_cache(char operation, int replacement_policy){
	
	if(this->is_a_hit(this->index, this->tag)){
		this->update_on_hit(this->index, operation, replacement_policy);
	}
	else{
		if(this->is_cacheline_empty(this->index)){
			this->install_block(this->index, this->tag, operation, replacement_policy);
		}
		else{
			if(replacement_policy == LRU){
				this->LRU_update(this->index, this->tag, operation);
			}
			else if(replacement_policy == FIFO){
				this->FIFO_update(this->index, this->tag, operation);
			}
			else if(replacement_policy == LFU){
				this->LFU_update(this->index, this->tag, operation);
			}
			else if(replacement_policy == psuedo_LRU){
				printf("\n\n-x-x-x-x-x-x-x-x-x-x-x-------- psuedo_LRU under development-------x-x-x-x-x-x-x-x-x-x-x\n\n");				
			}
			else{
				printf("-x-x-x-x-x-x-x-x\n     error in replacement policy    \n-x-x-x-x-x-x-x-x");
				exit(0);
			}
		}
	}
} 

void cache::age_increment(int index){
	
	for(int j = 0; j < associativity; j++){
		age[index][j]++;
	}
}

void cache::print_stats(const char* name){
	
	printf("=> number of %s reads 			= 	%d\n", name, reads);
	printf("=> number of %s read_misses 		= 	%d\n", name, read_miss);
	printf("=> number of %s writes 			= 	%d\n", name, writes);
	printf("=> number of %s write_misses 		= 	%d\n", name, write_miss);
	printf("=> number of %s writebacks 		= 	%d\n\n", name, writebacks);
	
}

void cache::debug_print(){
	printf("printing tag array:\n\n");
	for(int i = 0; i < number_of_sets; i++){
		for(int j = 0; j < associativity; j++){
			printf("V = %d\tT = %x\tD = %d\t",valid_bit[i][j], tag_array[i][j], dirty_bit[i][j]);
			//printf("T = %x\t",tag_array[i][j]);
		}
		printf("\n");
	}
}

void cache::print_valid_bit(){
	
	for(int i = 0; i < number_of_sets; i++){
		for(int j = 0; j < associativity; j++){
			printf("%x\t",valid_bit[i][j]);
		}
		printf("\n");
	}
}