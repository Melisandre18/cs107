#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0 && numBuckets > 0 && hashfn != NULL && comparefn != NULL);
  h->size = elemSize;
  h->numBuckets = numBuckets;
  h->logLen = 0;
  h->hashFunc = hashfn;
  h->compFunc= comparefn;
  h->freeFunc= freefn;

  h->elems = malloc(sizeof(vector)*numBuckets);
  assert(h->elems != NULL);

  for(int i = 0; i < numBuckets; i++)
		VectorNew(&(h->elems[i]), elemSize, freefn, 0);
}

void HashSetDispose(hashset *h)
{
		for(int i = 0; i < h->numBuckets; i++)
			VectorDispose(&h->elems[i]);
		
	free(h->elems);
}

int HashSetCount(const hashset *h)
{ 
	return h->size;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
  assert(h!=NULL);
  assert(mapfn != NULL);
  for (int i = 0; i < h->numBuckets; i++) 
    VectorMap(&h->elems[i], mapfn, auxData);
}

void HashSetEnter(hashset *h, const void *elemAddr)
{

  assert(elemAddr != NULL && h!=NULL) ;
  int hash = h->hashFunc(elemAddr, h->numBuckets);
  assert(hash >= 0 && hash < h->numBuckets);
  int index = VectorSearch(&h->elems[hash], elemAddr, h->compFunc, 0, false);
  if (index < 0)  
  {
    VectorAppend(&h->elems[hash], elemAddr);
    h->size++;
  } 
  else VectorReplace(&h->elems[hash], elemAddr, index); 
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
	assert(elemAddr != NULL);
	int hash = h->hashFunc(elemAddr, h->numBuckets);
	assert(hash >= 0 && hash < h->numBuckets);
	int index = VectorSearch(&h->elems[hash], elemAddr, h->compFunc, 0, false);
	return index >= 0 ? VectorNth(&h->elems[hash], index ) : NULL;
}
