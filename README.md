# Cache
Cache Hierarchy simulator


Steps:
1)use a make command to generate executable
2) ./sim_cache <BLOCKSIZE> <L1_SIZE> <L1_ASSOC> <L2_SIZE> <L2_ASSOC> <REPL_POLICY> <INCLUSION> <TRACE_FILE>

  o BLOCKSIZE: Positive int. Block size in bytes (assumed to be same for all caches)
  o L1_SIZE: Positive int. L1 cache size in bytes.
  o L1_ASSOC: Positive int. L1 set-associativity (1 is direct-mapped).
  o L2_SIZE: Positive int. L2 cache size in bytes; 0 signifies that there is no L2 cache.
  o L2_ASSOC: Positive int. L2 set-associativity (1 is direct-mapped).
  o REPL_POLICY: Positive int. 0 for LRU, 1 for FIFO, 2 for LFU.
  o INCLUSION: Positive int. 0 for non-inclusive, 1 for inclusive and 2 for exclusive.
  o TRACE_FILE: Character string. Full name of trace file including any extensions.
  
 
