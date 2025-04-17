#include "json.h"


void ArrayPushBack(Array* array, Node* push_node) {
    if (array->size >= array->capacity) {
        array->capacity += array->capacity / 2;
        array->items = realloc(array->items, array->capacity * sizeof(Node*));
    }
    array->items[array->size] = (Node*)malloc(sizeof(push_node));
    memcpy(array->items[array->size], push_node, sizeof(Node));
    array->size++;
}

void MapInsert(Map * map, MapItem * item) {
    if (map->size >= map->capacity) {
        map->capacity += map->capacity / 2;
        map->items = realloc(map->items, map->capacity * sizeof(MapItem*));
    }
    map->items[map->size] = (MapItem*)malloc(sizeof(item));
    memcpy(map->items[map->size], item, sizeof(MapItem));
    map->size++;
}

MapItem* MapAt(Map * map, const char * key) {
    for (size_t i = 0; i < map->size; ++i) {
        if (!(strcmp(map->items[i]->key, key))) {
            return map->items[i];
        }
    }
    return NULL;
}

Node* CreateNode() {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node) {
        memset(node, 0, sizeof(Node));
    }
    return node;
}

void FreeNode(Node* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_STRING:
            free(node->string_value);
            break;
        case NODE_ARRAY:
            for (size_t i = 0; i < node->array_value.size; ++i) {
                FreeNode(node->array_value.items[i]);
            }
            free(node->array_value.items);
            break;
        case NODE_MAP:
            for (size_t i = 0; i < node->map_value.size; ++i) {
                free(node->map_value.items[i]->key);
                FreeNode(node->map_value.items[i]->value);
            }
            free(node->map_value.items);
            break;
        default:
            break;
    }
    //free(node);
    node = NULL;
}

Node* LoadString(char** input) {
    Node* node = CreateNode();
    if (node) {
        node->type = NODE_STRING;
        char raw_str[256] = { 0 };
        size_t i = 0;
        while (true) {
            const char ch = (*input)[i];
            if (ch == '"') {
                *input = &(*input)[++i];
                break;
            /*} else if (ch == ' ') {
                *input = &(*input)[++i];
                continue;*/
            } else if (ch == '\\') {
                ++i;
                const char escaped_char = *(input[i - 1]);
                switch (escaped_char) {
                    case 'n':
                        raw_str[i] = ('\n');
                        break;
                    case 't':
                        raw_str[i] = ('\t');
                        break;
                    case 'r':
                        raw_str[i] = ('\r');
                        break;
                    case '"':
                        raw_str[i] = ('"');
                        break;
                    case '\\':
                        raw_str[i] = ('\\');
                        break;
                    default:
                        perror("Unrecognized escape sequence \\");
                        perror(&escaped_char);
                    }
            }
            else if (ch == '\n' || ch == '\r') {
                perror("Unexpected end of line");
            }
            else {
                raw_str[i] = ch;
            }
            ++i;
        }
        char *str_temp = malloc(sizeof(char) * (i)); // аллоцируем память под временную строку
        str_temp = raw_str; // копируем исходную строку 
        node->string_value = strdup(str_temp); // переносим строку в ноду json
    }
    return node;
}

Node* LoadNumber(char** input) {
    Node* node = CreateNode();
    if (node) {
        char* endptr;
        double value = strtod(*input, &endptr);
        *input = endptr;
        //if (*endptr == '\0') {
            if (value == (int)value) {
                node->type = NODE_INT;
                node->int_value = (int)value;
            } else {
                node->type = NODE_DOUBLE;
                node->double_value = value;
            }
        /*} else {
            FreeNode(node);
            node = NULL;
        }*/
    }
    return node;
}

Node* LoadBool (char** input) {
    Node* node = CreateNode();
    if (node) {
        node->type = NODE_BOOL;
        char raw_str[6] = { 0 };
        size_t i = 0;
        while (true) {
            const char ch = (*input)[i];
            if (strcmp(raw_str, "true") == 0 || strcmp(raw_str, "false") == 0) {
                node->bool_value = strcmp(raw_str, "true") == 0;
                memset(raw_str, 0, 6);
                *input = &(*input)[i];
                break;
            } else {
                raw_str[i] = ch;
            }
            ++i;
        }
    }
    return node;
}

Node* LoadArray(char** input) {
    Node* node = CreateNode();
    if (node) {
        node->type = NODE_ARRAY;
        node->array_value.capacity = ARRAY_INIT_CAPACITY;
        node->array_value.size = 0;
        node->array_value.items = (Node **) malloc(ARRAY_INIT_CAPACITY * sizeof(Node*));
        size_t i = 0;
        char raw_str[256] = { 0 };
        while (true) {
            const char ch = (*input)[i];
            if (ch == ']') {
                //char *str_temp = malloc(sizeof(char) * (i)); // аллоцируем память под временную строку
                //str_temp = raw_str; // копируем исходную строку
                ArrayPushBack(&node->array_value, LoadNode(input));
                *input = &(*input)[1];
                break;
            } else if (ch == ',') {
                if (i == 0) {
                    *input = &(*input)[++i];
                } else {
                    ArrayPushBack(&node->array_value, LoadNode(input));
                }
                //char *str_temp = malloc(sizeof(char) * (i)); // аллоцируем память под временную строку
                //Node* temp_node = LoadNode(input);
                //str_temp = raw_str; // копируем исходную строку
                //memset(raw_str, 0, 256);
                //*input = &(*input)[i];
                i = 0;
                continue;
            } else {
                raw_str[i] = ch;
            }
            ++i;
        }
    }
    return node;
}

Node* LoadMap(char** input) {
    Node* node = CreateNode();
    if (node) {
        node->type = NODE_MAP;
        node->map_value.capacity = ARRAY_INIT_CAPACITY;
        node->map_value.size = 0;
        node->map_value.items = (MapItem **) malloc(ARRAY_INIT_CAPACITY * sizeof(MapItem*));
        size_t i = 0;
        char raw_str[256] = { 0 };
        while (true) {
            const char ch = (*input)[i];
            char *key;
            if ((*input)[0] == '\0') {
                *input = &(*input)[i];
                break;
            }
            if (ch == '"') {
                *input = &(*input)[++i];
                key = LoadString(input)->string_value;
                if ((*input)[0] == ':') {
                    *input = &(*input)[1];
                    MapItem item = { key, LoadNode(input) };
                    MapInsert(&node->map_value, &item);
                    i = 0;
                    continue;
                }
            }
            /*} else if (ch == ' ') {
                input = &(input[i]);
                continue;
            } else if (ch == ':') {
                char *temp = malloc(sizeof(char) * (i));
                strcpy(temp, raw_str);
                char *key = LoadString(&temp)->string_value;
                //char* key = malloc(sizeof(char) * (i)); // аллоцируем память под временную строку
                //key = strdup(raw_str); // копируем исходную строку
                *input = &(*input)[++i];
                MapItem item = { key, LoadNode(input) };
                MapInsert(&node->map_value, &item);
                i = 0;
                continue;
            }*/
            if (ch == '}') {
                *input = &(*input)[++i];
                break;
            } else {
                raw_str[i] = ch;
            }
            ++i;
        }
        
    }
    return node;
}

Node* LoadNode(char** input) {
    const char c = (*input)[0];
    switch (c)
        {
        case '"':
            *input = &(*input)[1];
            return LoadString(input);
        case '[':
            *input = &(*input)[1];
            return LoadArray(input);
        case '{':
            *input = &(*input)[1];
            return LoadMap(input);
        case 't':
            [[fallthrough]]; // переходим к следующему case
        case 'f':
            return LoadBool(input);
        case ' ':
            *input = &(*input)[1];
            return LoadNode(input);
        default:
            return LoadNumber(input);
        }
    return NULL;
}


// --------------NODE------------------------

bool IsInt(Node* node) {
    return node->type == NODE_INT;
}

bool IsDouble(Node* node) {
    return node->type == NODE_INT || node->type == NODE_DOUBLE;
}

bool IsPureDouble(Node* node) {
    return node->type == NODE_DOUBLE;
}

bool IsBool(Node* node) {
    return node->type == NODE_BOOL;
}

bool IsString(Node* node) {
    return node->type == NODE_STRING;
}

bool IsNull(Node* node) {
    return node->type == NODE_NULL;
}

bool IsArray(Node* node) {
    return node->type == NODE_ARRAY;
}

bool IsMap(Node* node) {
    return node->type == NODE_MAP;
}

const Array AsArray(Node* node) {
    if (IsArray(node)) {
        return node->array_value;
    }
    else {
        perror("Node is not Array");
    }
}

const Map AsMap(Node* node) {
    if (IsMap(node)) {
        return node->map_value;
    }
    else {
        perror("Node is not Map");
    }
}

int AsInt(Node* node) {
    if (IsInt(node)) {
        return node->int_value;
    }
    else {
        perror("Node is not int");
    }
}

const char* AsString(Node* node) {
    if (IsString(node)) {
        return node->string_value;
    }
    else {
        perror("Node is not string");
    }
}

bool AsBool(Node* node) {
    if (IsBool(node)) {
        return node->bool_value;
    }
    else {
        perror("Node is not bool");
    }
}

double AsDouble(Node* node) {
    if (IsInt(node)) {
        return node->int_value;
    }
    else if (IsPureDouble(node)) {
        return node->double_value;
    }
    else {
        perror("Node is not double");
    }
}
