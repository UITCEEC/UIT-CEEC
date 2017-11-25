#ifndef __DIRECTION_DATA_H__
#define __DIRECTION_DATA_H__

#include <stdlib.h>

#define DIRECTION_DATA_LIST_NODE_LIMIT 150 // Change the datatype of count variable in DirectionDataListT to ensure the overflow do not happens.

typedef enum AddResultCodeEnum {
    STATUS_OK = 0,
    STATUS_EXCEED_LIMIT = 1,
    STATUS_NOT_ENOUGH_MEMORY = 2
} AddResultCode;

typedef struct
{
    unsigned int nextDirection : 2;
    unsigned int longtitude : 30;
    int __reserveBits : 2;
    unsigned int latetitude : 30;
} DirectionDataT;

typedef struct DirectionDataNode_Tag
{
    DirectionDataT *data;
    struct DirectionDataNode_Tag *pNext;
} DirectionDataNodeT;

typedef struct
{
    DirectionDataNodeT *pHead;
    DirectionDataNodeT *pTail;
    int count;
} DirectionDataListT;

void DirectionDataList_Init(DirectionDataListT *);
AddResultCode DirectionDataList_Put(DirectionDataListT *, int, int, int);
DirectionDataT *DirectionDataList_Get(DirectionDataListT *);
void DirectionDataList_Clear(DirectionDataListT *);

#endif /* __DIRECTION_DATA_H__ */
