#pragma once
#include <stdio.h>
#include "json.h"

typedef struct IOjson {
    char * filename;
    FILE * fp;
    void (*PrintFile)(struct IOjson * ioreader);
} IOjson;

void PrintFilePoint(IOjson * ioreader);
char* GetStrJSON(IOjson * ioreader);
void PrintNode(Node* node);