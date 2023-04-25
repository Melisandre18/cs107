#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "search.h"

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
  assert(elemSize > 0 && initialAllocation >= 0);
  v->allocLen=  (initialAllocation > 0) ? initialAllocation : 5; 
  v->size = elemSize;
  v->freeFunc = freeFn;
  v->logLen= 0;
  v->elems = malloc(elemSize * v->allocLen);
  assert(v->elems != NULL);
}

void VectorDispose(vector *v)
{
    if (v->freeFunc!=NULL) {
        for (int i = 0; i < v->logLen; i++)
            v->freeFunc(VectorNth(v, i));
    }
  
  free(v->elems);
}

int VectorLength(const vector *v)
{  
    return v->logLen; 
}

void *VectorNth(const vector *v, int position)
{ 
    assert (position >= 0 && position < v->logLen);
    return (char*)v->elems + position * v->size; 
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
    assert (position >= 0 && position < v->logLen);
    assert(elemAddr!=NULL);
    void *index=VectorNth(v, position);
    if (v->freeFunc != NULL)  v->freeFunc(index);

    memcpy(index, elemAddr, v->size);
}

static void grow(vector *v)
{
  v->allocLen *= 2;
  v->elems = realloc(v->elems, v->size * v->allocLen);
  assert(v->elems != NULL);
  
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
    assert (position >= 0 && position <= v->logLen);
    assert( elemAddr != NULL);
    if (v->logLen == v->allocLen) grow(v);
    void *ptr =(char*)v->elems + position * v->size;
    memmove((char *)ptr + v->size, ptr, (v->logLen - position) * v->size);
    memcpy(ptr, elemAddr, v->size);  
    v->logLen++;  
}

void VectorAppend(vector *v, const void *elemAddr)
{
    if (v->logLen == v->allocLen) grow(v);
    void *ptr = (char*)v->elems + v->logLen * v->size;
    memcpy(ptr, elemAddr, v->size);
    v->logLen++; 
}

void VectorDelete(vector *v, int position)
{
    assert (position >= 0 && position < v->logLen);
    void *ptr = VectorNth(v, position);
    if (v->freeFunc != NULL)  v->freeFunc(ptr);
    v->logLen--;
	memmove(ptr, (char *)ptr + v->size, (v->logLen - position) * v->size);
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
    assert(compare != NULL);
	qsort(v->elems, v->logLen, v->size, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
    assert(mapFn != NULL);
    for (int i = 0; i < v->logLen; i++)
    mapFn(VectorNth(v, i), auxData);
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ 
    assert(startIndex <= v->logLen && startIndex >=0 && key != NULL && searchFn != NULL);
    void *found;
	size_t index = v->logLen - startIndex;
	if(isSorted)  found = bsearch(key, (char*)v->elems+ v->size* startIndex, index, v->size, searchFn);
	else found = lfind(key, (char*)v->elems+ v->size* startIndex, &index, v->size, searchFn);
    return found ? ((char*)found - (char*)v->elems)/v->size :  kNotFound;
} 