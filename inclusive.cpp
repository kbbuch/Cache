#include "cache.h"

void cache::operate_on_cache_inclusive(char operation, int replacement_policy, bool d_b){
	
	if(this->is_a_hit(this->index, this->tag)){
		this->update_on_hit(this->index, operation, replacement_policy);
	}
	else{
		if(this->is_cacheline_empty(this->index)){
			this->install_block(this->index, this->tag, operation, replacement_policy);
		}
		else{
			if(replacement_policy == LRU){
				address = this->LRU_update_inclusive(this->index, this->tag, operation, d_b);
				if(operation == 'r') is_evicted = true;
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

long long int cache::LRU_update_inclusive(int index, long long int tag, char operation, bool d_b){
	
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
		
	}
	
	long long int tag_temp = tag_array[index][evict_block];
	
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
	long long int address = reconstruct_address(tag_temp);

	return address;
}

void cache::invalidate_in_L1(){
	
	valid_bit[index][hit_block] = false;
	tag_array[index][hit_block] = 0;
}

void cache::inclusive_LRU_part(cache *L2, char operation, int replacement_policy, int evict_block){
	
	bool d_b;
	d_b = dirty_bit[index][evict_block];
	
	if(dirty_bit[index][evict_block] == true){
		writebacks++;
		char L2_op = 'w';
		
		// convert the address that is being evicted to L2 tag;
		d_b = dirty_bit[index][evict_block];
		long long int tag_temp = tag_array[index][evict_block];
		L2->split_address(this->reconstruct_address(tag_temp));
		
		L2->update_read_write(L2_op);
		L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, INCLUSIVE, d_b);
	}
	
	char L2_op = 'r';
	L2->split_address(this->reconstruct_address(tag));
	L2->update_read_write(L2_op);
	L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, INCLUSIVE, d_b);
	
	long long int original_tag = tag;
	int original_index = index;
	//evicting the L2_eviction from L1
	if(L2->is_evicted){
		L2->is_evicted = false;
		
		this->split_address(L2->address);
		
		if(is_a_hit(this->index, this->tag)){
			invalidate_in_L1();
			if(dirty_bit[index][hit_block] == true){
				writebacks++;
				L2->increment_writebacks();
				dirty_bit[index][hit_block] == false;
			}
		}
	}
	tag_array[original_index][evict_block] = original_tag;
	valid_bit[original_index][evict_block] = true;
	age[original_index][evict_block] = 1;
	
	if(operation == 'w'){
		write_miss++;
		dirty_bit[original_index][evict_block] = true;
	}
	else if(operation == 'r'){
		read_miss++;
		dirty_bit[original_index][evict_block] = false;
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in operation        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}