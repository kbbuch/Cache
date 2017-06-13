#include "cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
	char *inputfile;
	int inclusion_policy, replacement_policy, block_size;
	
	block_size = atoi(argv[1]);
	replacement_policy = atoi(argv[6]);
	inclusion_policy = atoi(argv[7]);
	
    inputfile = argv[8];
	
	cache *L1 = new cache(atoi(argv[2]), atoi(argv[3]), block_size);
	cache *L2 = new cache(atoi(argv[4]), atoi(argv[5]), block_size);
	
	FILE *fp = fopen(inputfile, "r");

	char operation;
	long long int address;
	int k = 0;
	
	while(1){
		
		k++;
		int c = fscanf(fp,"%c %x\n", &operation, &address);
		//if(k>10) break;
		//printf("\ninput no. C = %d\n",k);
		if(c == EOF) break;
		
		//address split to get index
		L1->split_address(address);
		L2->split_address(address);
		
		//upadting L1 read and writes
		L1->update_read_write(operation);
		
		if(L2->size == 0 || L2->associativity == 0){
			L1->operate_on_cache(operation, replacement_policy);
		}
		else{
	
			if(L1->is_a_hit(L1->index, L1->tag)){
				L1->update_on_hit(L1->index, operation, replacement_policy);
			}
			else{
				if(L1->is_cacheline_empty(L1->index)){
					L1->install_block(L1->index, L1->tag, operation, replacement_policy, inclusion_policy, L2);
				}
				else{
					if(replacement_policy == LRU){
						L1->LRU_update(L1->index, L1->tag, operation, replacement_policy, inclusion_policy, L2);
					}
					else if(replacement_policy == FIFO){
						L1->FIFO_update(L1->index, L1->tag, operation, replacement_policy, inclusion_policy, L2);
					}
					else if(replacement_policy == LFU){
						L1->LFU_update(L1->index, L1->tag, operation, replacement_policy, inclusion_policy, L2);
					}
					else if(replacement_policy == psuedo_LRU){
						printf("\n\n-x-x-x-x-x-x-x-x-x-x-x-------- psuedo_LRU under development-------x-x-x-x-x-x-x-x-x-x-x\n\n");
						exit(0);
					}
					else{
						printf("-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x\n\n\n\n\n     error in replacement policy    \n\n\n\n\nx-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x\n");
						exit(0);
					}
				}
			}
		}
		
		//L2->print_valid_bit();
		/*printf("\n");
		L1->debug_print();
		L2->debug_print();*/
	}
	
	const char* L1_name = "L1";
	const char* L2_name = "L2";
	
	//L1->debug_print();
	//printf("\n\n");
	
	L1->print_stats(L1_name);
	if(L2->size != 0 && L2->associativity != 0) L2->print_stats(L2_name);
	
	//printf("L2 rd_misses for when L2 is still vacant = %d\n", L2->rdmiss_empty_cache);
	//printf("L2 wr_misses for when L2 is still vacant = %d\n", L2->wrmiss_empty_cache);
	//printf("no. of times L2 found vacant = %d\n", L2->cache_empty);
	//printf("no. of times L1 had to do LRU operation = %d\n", L1->lru);
	
	if(L1 != NULL) delete L1;
	if(L2 != NULL) delete L2;
	
	return 0;
}
	