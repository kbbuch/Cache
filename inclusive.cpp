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
				//printf("hello here?\n");
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
		//printf("here too for 7410?\n");
		writebacks++;
		
	}
	//printf("dB from within LRU update = %d\n",d_b);		
	
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
	
	cache_empty++;
	bool d_b;
	d_b = dirty_bit[index][evict_block];
	//printf("dB = %d\n", d_b);
	
	if(dirty_bit[index][evict_block] == true){
		writebacks++;
		//printf("doint write back as the dirty bit of the evicted block is high\n");
		char L2_op = 'w';
		
		// convert the address that is being evicted to L2 tag;
		d_b = dirty_bit[index][evict_block];
		long long int tag_temp = tag_array[index][evict_block];
		L2->split_address(this->reconstruct_address(tag_temp));
		
		L2->update_read_write(L2_op);
		//printf("L1->tag = %x\n", tag);
		//printf("L2->tag = %x\n", L2->tag);
		L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, INCLUSIVE, d_b);
	}
	
	char L2_op = 'r';
	L2->split_address(this->reconstruct_address(tag));
	L2->update_read_write(L2_op);
	//printf("L1->tag = %x\n", tag);
	//printf("L2->tag = %x\n", L2->tag);
	//d_b = false;
	//printf("what is getting evicted? ANS: %x\n",tag_temp);
	//printf("what are we placing in L2 and where? ANS: %x in set %d\n",L2->tag, L2->index);
	L2->action_acc_to_inclusion_policy(L2_op, replacement_policy, INCLUSIVE, d_b);
	
	//printf("idex for L1 = %d\n", index);
	long long int original_tag = tag;
	int original_index = index;
	//evicting the L2_eviction from L1
	if(L2->is_evicted){
		L2->is_evicted = false;
		
		this->split_address(L2->address);
		
		if(is_a_hit(this->index, this->tag)){
			//printf("back invalidating %x from %d set in L1\n", tag, index);
			invalidate_in_L1();
			//printf("index = %d and hit_block = %d and its dirty bit = %d\n", index, hit_block, dirty_bit[index][hit_block]);
			if(dirty_bit[index][hit_block] == true){
				//printf("here too?\n");
				writebacks++;
				//writebacks++;
				L2->increment_writebacks();
				dirty_bit[index][hit_block] == false;
			}
		}
	}
	//printf("idex for L1 = %d\n", index);
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