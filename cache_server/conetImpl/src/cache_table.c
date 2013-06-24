#include "cache_table.h"
#include <conet/conet.h>	//for CONET_DEBUG and CONET_SEVERE_DEBUG

/*
void add_to_cache_tableVecchia (CacheEntry_t* thecache, CacheEntry_t* entry)
{
	struct CacheEntry* tmp_entry;
	
    //add_to_cache_table our structure to the hash table
    HASH_ADD( hh, thecache, csn, CALCULATE_KEY_LEN(entry->nidlength), entry);
    
   fprintf(stderr,"nid=%s, csn=%ld added in cache table. Now cache has %d elements\n", entry->nid,entry->csn,HASH_COUNT(thecache));
    
    
	//prune the cache to MAX_CACHE_SIZE
    if (HASH_COUNT(thecache) > MAX_CACHE_SIZE) 
    {
        HASH_ITER(hh, thecache, entry, tmp_entry) 
        {
            // prune the first entry (loop is based on insertion order so this deletes the oldest item)
            HASH_DELETE(hh, thecache, entry);
           fprintf(stderr,"nid=%s, csn=%ld deleted\n",entry->nid,entry->csn);
			//free(entry->nid);free(entry);									
            break;
        }
    } 
    
   fprintf(stderr,"!!!Prima di uscire ci sono %d elementi\n",HASH_COUNT(thecache));
}
*/

unsigned long long find_on_cache_table(CacheEntry_t* cache,UT_string* nid, unsigned long long csn)
{
	if(CONET_SEVERE_DEBUG && csn>=100)
	{
		fprintf(stderr,"[cache_table.c:%d] csn=%llu. This is very large. It is not necessarily an error but ... are you sure?\n",__LINE__, csn);
		exit(-8776);
	}

	unsigned long long chunk_size=0;
	CacheEntry_t* pointer=NULL;
	Lookup_key_t *lookup_key;
	size_t keylen=CALCULATE_KEY_LEN(utstring_len(nid) );
	ALLOCATE_LOOKUP_KEY(lookup_key,nid,csn);

    if(CONET_DEBUG) fprintf(stderr,"[cache_table.c:%d]I'm going to call HASH_FIND \n",__LINE__);
    HASH_FIND( hh, cache, &lookup_key->csn, keylen, pointer );
    if(CONET_DEBUG) fprintf(stderr,"[cache_table.c:%d] Sono dopo  HASH_FIND \n",__LINE__);
    if (pointer) 
    {
	   	if(CONET_DEBUG) fprintf(stderr,"[cache_table.c:%d] nid=%s, csn=%llu found in cache table\n",
			__LINE__,utstring_body(nid), csn);
		chunk_size= pointer->chunk_size;
	   	if(CONET_DEBUG) fprintf(stderr,"[cache_table.c:%d] chunk_size=%llu\n",__LINE__,chunk_size);

    	//remove and add_to_cache_table it to place it at the head
    	HASH_DEL(cache, pointer);	
    	HASH_ADD( hh, cache, csn, keylen, pointer);
    }
    else{
    	if(CONET_DEBUG) fprintf(stderr,"[cache_table.c:%d]nid=%s, csn=%llu not found in cache table\n",
    			__LINE__,utstring_body(nid), csn);
    	if (CONET_SEVERE_DEBUG) exit(-88);
    }
    free(lookup_key);

    fprintf(stderr,"[cache_table.c:%d] end of find_on_cache_table(..)\n",__LINE__);

    return chunk_size;
}
