#include "cache.h"

void cache::install_block(int index, long long int tag, char operation, int replacement_policy, int inclusion_policy, cache *L2){
	
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
	
	char L2_op = 'r';
	L2->update_read_write(L2_op);
	L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, inclusion_policy);
}

void cache::LRU_update(int index, long long int tag, char operation, int replacement_policy, int inclusion_policy, cache *L2){
	
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

long long int cache::reconstruct_address(long long int tag_temp){
	
	long long int tag_temp1 = tag_temp << index_bits;
	tag_temp = tag_temp1 + index;
	tag_temp1 = tag_temp << offset_bits;
	
	return tag_temp1;
}

void cache::action_acc_to_inclusion_policy(char L2_op, int replacement_policy, int inclusion_policy){
	
	if(inclusion_policy == NINE){
		operate_on_cache(L2_op, replacement_policy);
	}
	else if(inclusion_policy == INCLUSIVE){
		printf("\n\n-x-x-x-x-x-x-x-x-x-x-x--------under development-------x-x-x-x-x-x-x-x-x-x-x\n\n");
		exit(0);
	}
	else if(inclusion_policy == EXCLUSIVE){
		printf("\n\n-x-x-x-x-x-x-x-x-x-x-x--------under development-------x-x-x-x-x-x-x-x-x-x-x\n\n");
		exit(0);
	}
	else{
		printf("-x-x-x-x-x--x-x-x-x-x-x-x\n          Error in inclusion policy        \n-x-x-x-x-x--x-x-x-x-x-x-x");
		exit(0);
	}
}