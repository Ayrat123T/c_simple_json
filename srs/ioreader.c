#include "ioreader.h"

void PrintFilePoint(IOjson * ioreader)  {
    char c;
    FILE * fp = fopen(ioreader->filename, "r"); //открываем файл
    // считываем посимвольно из файла
    printf("%s", "\n\r");
    while ((c = getc(fp)) != EOF) {
        printf("%c", c);
    }
    printf("%s", "\n\r");
}

char* GetStrJSON(IOjson * ioreader) {
    char raw_str_json[1024] = { 0 };
    char c;
    size_t i = 0;
    while ((c = getc(ioreader->fp)) != EOF) {
        raw_str_json[i] = c;
        ++i;
    }
    char *json = malloc(sizeof(char) * (i)); // аллоцируем память под временную строку
    json = strdup(raw_str_json); // копируем исходную строку 
    return json;
}

void PrintNode(Node* node) {
    //char* offset = " ";
    switch (node->type)
        {
        case NODE_BOOL:
            printf("%s", (AsBool(node) ? "true" : "false"));
            break;
        case NODE_INT:
            printf("%d", AsInt(node));
            break;
        case NODE_DOUBLE:
            printf("%f", AsDouble(node));
            break;
        case NODE_STRING:
            //char *str = malloc(sizeof(char) * (strlen(node->string_value) + 1));
            //strcpy(str, node->string_value);
            printf("        %s", AsString(node));
            break;
        case NODE_ARRAY:
            printf("    %c\n\r", '[');
            for (size_t i = 0; i < node->array_value.size; ++i) {
                PrintNode((AsArray(node)).items[i]);
                if (i != node->array_value.size - 1) {
                    printf("%c", ',');
                }
                printf("%s", "\n\r");
            }
            printf("    %c", ']');
            break;
        case NODE_MAP:
            printf("%c\n\r", '{');
            for (size_t i = 0; i < (AsMap(node)).size; ++i) {
                printf("    %s : ", node->map_value.items[i]->key);
                PrintNode(node->map_value.items[i]->value);
                if (i != node->map_value.size - 1) {
                    printf("%c", ',');
                }
                printf("%s", "\n\r");
            }
            printf("%c", '}');
            break;
        default:
            break;
    }
}