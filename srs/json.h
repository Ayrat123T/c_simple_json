#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ARRAY_INIT_CAPACITY 4 //стандартный размер массива

typedef struct Node Node; // объявляем узел

//тип узла
typedef enum {
    NODE_NULL,
    NODE_BOOL,
    NODE_INT,
    NODE_DOUBLE,
    NODE_STRING,
    NODE_ARRAY,
    NODE_MAP
} NodeType;

//узел - массив
typedef struct {
    Node** items;
    size_t size;
    size_t capacity;
} Array; 

// добавить элемент в конец массива
void ArrayPushBack(Array* array, Node* push_node);

//узел - пара (элемент) словаря
typedef struct {
    char* key;
    Node* value;
} MapItem;

//узел - словарь
typedef struct {
    MapItem** items;
    size_t size;
    size_t capacity;
} Map;

//добавить элемент в словарь
void MapInsert(Map * map, MapItem * item );

//найти ключ
MapItem* MapAt(Map * map, const char* key);

// Узел
typedef struct Node {
    NodeType type;
    union { //храним данные в виде объединения для экономии памяти
        bool bool_value;
        int int_value;
        double double_value;
        char* string_value;
        Array array_value;
        Map map_value;
    };
} Node;

// Напечатать узел
void PrintNode(Node* node);

// освободить память, выделенную по узел
void FreeNode(Node* node);

//создать узел
Node* CreateNode();

//Загрузить узел-строку
Node* LoadString(char** input);

//Загрузить узел-число
Node* LoadNumber(char** input);

//Загрузить узел-массив
Node* LoadArray(char** input);

//Загрузить узел-словарь
Node* LoadMap(char** input);

//Загрузить Узел
Node* LoadNode(char** input);

// Возвращает true, если в Node хранится Int
bool IsInt(Node* node);

// Возвращает true, если в Node хранится Double или Int
bool IsDouble(Node* node);

// Возвращает true, если в Node хранится Double
bool IsPureDouble(Node* node);

bool IsBool(Node* node);

bool IsString(Node* node);

bool IsNull(Node* node);

bool IsArray(Node* node);

bool IsMap(Node* node);

/* Ниже перечислены методы, которые возвращают хранящееся внутри Node значение заданного типа.
Если внутри содержится значение другого типа, должно выбрасываться исключение logic_error.*/

int AsInt(Node* node);
bool AsBool(Node* node);

/* Возвращает значение типа double, если внутри хранится double либо int
В последнем случае возвращается приведённое в double значение.*/
double AsDouble(Node* node);
const char* AsString(Node* node);
const Array* AsArray(Node* node);
const Map* AsMap(Node* node);