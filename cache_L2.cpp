#include "cache.h"

void cache::install_block(int index, long long int tag, char operation, int replacement_policy, int inclusion_policy, cache *L2){
	
	char L2_op = 'r';
	bool d_b = false;
	L2->update_read_write(L2_op);
	if(inclusion_policy == NINE){
		L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy);
	}
	else if(inclusion_policy == EXCLUSIVE){
		L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy, d_b);
	}
	else if(inclusion_policy == INCLUSIVE){
		L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy, d_b);
		
		if(L2->is_evicted){
			//printf("helle here?\n");
			L2->is_evicted = false;
			
			this->split_address(L2->address);
			
			if(is_a_hit(this->index, this->tag)){
				
				//printf("back invalidating %x from %d set in L1\n", this->tag, this->index);
				invalidate_in_L1();
				//printf("index = %d and hit_block = %d\n", this->index, hit_block);
				if(dirty_bit[this->index][hit_block] == true){
					//printf("here too?\n");
					writebacks++;
					//writebacks++;
					L2->increment_writebacks();
					dirty_bit[index][hit_block] == false;
				}
			}
		}
	}
	
	cache_empty++;
	rdmiss_empty_cache++;
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
		dirty_bit[index][empty_block] = false;
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in operation        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}

void cache::LRU_update(int index, long long int tag, char operation, int replacement_policy, int inclusion_policy, cache *L2){
	
	int max = -200, evict_block = 0;
	//printf("L1 index obtaining eviction block = %d\n", index);

	for(int i = 0; i < associativity; i++){
		if(age[index][i] > max){
			max = age[index][i];
			evict_block = i;
		}	
	}
	
	//printf("here to evict one from L1 from block = %d\n", evict_block);
	age_increment(index);
	
	if(inclusion_policy == EXCLUSIVE){
		writebacks++;
		
		char L2_op = 'w';
		bool d_b;
		
		// convert the address that is being evicted to L2 tag;
		long long int tag_temp = tag_array[index][evict_block];
		L2->split_address(this->reconstruct_address(tag_temp));
		L2->update_read_write(L2_op);
		d_b = dirty_bit[index][evict_block];
		L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy, d_b);
		
		L2_op = 'r';
		//convert the original address to look up L2
		L2->split_address(this->reconstruct_address(tag));
		L2->update_read_write(L2_op);
		L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy, d_b);
	}
	else if(inclusion_policy == NINE){
		if(dirty_bit[index][evict_block] == true){
			writebacks++;
			
			char L2_op = 'w';
			
			// convert the address that is being evicted to L2 tag;
			long long int tag_temp = tag_array[index][evict_block];
			L2->split_address(this->reconstruct_address(tag_temp));
			L2->update_read_write(L2_op);
			L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy);
		}
		
		char L2_op = 'r';
		L2->split_address(this->reconstruct_address(tag));
		L2->update_read_write(L2_op);
		L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy);
	}
	else if(inclusion_policy == INCLUSIVE){
		//printf("continuing in LRU for L1\n");
		//printf("L1 index while calling inclusive part = %d\n", index);
		inclusive_LRU_part(L2, operation, replacement_policy, evict_block);
		return;
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in inclusion policy        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
	
	tag_array[index][evict_block] = tag;
	age[index][evict_block] = 1;
	
	if(operation == 'w'){
		write_miss++;
		dirty_bit[index][evict_block] = true;
		if(L2->lru == 1){
			L2->lru = 0;
			//printf("here once?\n");
			//dirty_bit[index][evict_block] = L2->dirty_bit[L2->index][L2->hit_block];
			L2->dirty_bit[L2->index][L2->hit_block] = false;
		}
	}
	else if(operation == 'r'){
		read_miss++;
		if(L2->lru == 1 && inclusion_policy == EXCLUSIVE){
			L2->lru = 0;
			//printf("here once?\n");
			dirty_bit[index][evict_block] = L2->dirty_bit[L2->index][L2->hit_block];
			L2->dirty_bit[L2->index][L2->hit_block] = false;
		}
		else{
			dirty_bit[index][evict_block] = false;
		}
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in operation        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}

void cache::FIFO_update(int index, long long int tag, char operation, int replacement_policy, int inclusion_policy, cache *L2){
	
	int max = -200, evict_block = 0;
	
	for(int i = 0; i < associativity; i++){
		if(age[index][i] > max){
			max = age[index][i];
			evict_block = i;
		}	
	}
	
	age_increment(index);
	
	if(dirty_bit[index][evict_block] == true){
		writebacks++;
		
		char L2_op = 'w';
		
		// convert the address that is being evicted to L2 tag;
		long long int tag_temp = tag_array[index][evict_block];
		L2->split_address(this->reconstruct_address(tag_temp));
		
		L2->update_read_write(L2_op);
		L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy);
	}
	
	char L2_op = 'r';
	L2->split_address(this->reconstruct_address(tag));
	L2->update_read_write(L2_op);
	L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy);
	
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

void cache::LFU_update(int index, long long int tag, char operation, int replacement_policy, int inclusion_policy, cache *L2){
	
	int min = age[index][0], evict_block = 0;
	
	for(int j=1; j < associativity; j++)
	{
		if(age[index][j] < min){
			min = age[index][j];
			evict_block = j;
		}
	}
	
	//printf("index = %d, evict_block = %d\n", index, evict_block);
	
	if(dirty_bit[index][evict_block] == true){
		writebacks++;
		
		char L2_op = 'w';
		
		// convert the address that is being evicted to L2 tag;
		long long int tag_temp = tag_array[index][evict_block];
		L2->split_address(this->reconstruct_address(tag_temp));
		
		L2->update_read_write(L2_op);
		L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy);
	}
	//printf("here in LFU_update\n");
	
	char L2_op = 'r';
	L2->split_address(this->reconstruct_address(tag));
	L2->update_read_write(L2_op);
	L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy);
	
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

void cache::update_on_hit_exclusive(int index){

	valid_bit[index][hit_block] = false;
}

void cache::operate_on_cache_exclusive(char operation, int replacement_policy, int inclusion_policy, bool d_b){
	
	if(this->is_a_hit(this->index, this->tag)){
		this->update_on_hit_exclusive(this->index);
		this->lru = 1;
	}
	else{
		if(operation == 'w'){
			if(this->is_cacheline_empty(this->index)){
				this->install_block(this->index, this->tag, operation, replacement_policy, inclusion_policy, d_b);
			}
			else{
				if(replacement_policy == LRU){
					this->LRU_update(this->index, this->tag, operation, d_b);
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
		else if(operation == 'r'){
			this->read_miss++;
		}
	}
} 

long long int cache::reconstruct_address(long long int tag_temp){
	
	long long int tag_temp1 = tag_temp << index_bits;
	tag_temp = tag_temp1 + index;
	//printf("index = %d and index_bits = %d\n", index, index_bits);
	tag_temp1 = tag_temp << offset_bits;
	
	return tag_temp1;
}

void cache::action_acc_to_inclusion_policy(char L2_op, int replacement_policy, int inclusion_policy){
	
	if(inclusion_policy == NINE){
		this->operate_on_cache(L2_op, replacement_policy);
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in inclusion policy        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}

void cache::action_acc_to_inclusion_policy(char L2_op, int replacement_policy, int inclusion_policy, bool d_b){
	
	if(inclusion_policy == EXCLUSIVE){
		this->operate_on_cache_exclusive(L2_op, replacement_policy, EXCLUSIVE, d_b);		
	}
	else if(inclusion_policy == INCLUSIVE){
		//printf("operation = %c\n", L2_op);
		this->operate_on_cache_inclusive(L2_op, replacement_policy, d_b);
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in inclusion policy        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}

void cache::increment_writebacks(){
	
	writebacks++;
}