#include <stdio.h>
#include "ioreader.h"
#include "json.h"
#include <math.h>
Node* TestNode(char* json) {
    Node* root = LoadNode(&json);
    if (root) {
        // Обработка JSON
        //PrintNode(root);
        //FreeNode(root);
    } else {
        printf("Failed to parse JSON\n");
    }
    return root;
}

void test0() {
    char* json_str = "\"monitoring\"";
    Node* node = TestNode(json_str);
}

void test1() {
    char* json_str = "[\"monitoring\",\"maximters\"]";
    TestNode(json_str);
}


void test2() {
    char* json_str = "{\"scripts\":[\"monitoring\",\"maximters\"]}";
    TestNode(json_str);
}

void test3() {
    char* json_str = "{\"map\":[\"monitoring\",\"maximters\"],\"key\":\"value\"}";
    TestNode(json_str);
}

void test4() {
    char* json_str = "{\"map\":[\"monitoring\",\"maximters\"],\"empty\":{},\"key\":\"value\"}";
    TestNode(json_str);
}

void test5() {
    char* json_str =   "{\
                            \"monitoring\": {},\
                            \"maximters\": {},\
                            \"flexLogic\": {\
                                \"scripts\":[0,1,2],\
                                \"functions\":[0,1,2],\
                                \"setpoint\":{\
                                    \"bool\":[true,false,true],\
                                    \"int\":[0,1,2],\
                                    \"float\":[0.3,1.4,2.5]\
                                    }\
                                }\
                        }";
    TestNode(json_str);
}

void test6() {
    char * filename = "../example copy.json";
    FILE * fp = fopen(filename, "r"); //открываем файл
    IOjson examle_json = {filename, fp, &PrintFilePoint};
    if (fp == NULL) {
        perror("Error occured while opening \"");
        perror(filename);
        perror("\"\n\r");
    } else {
        //examle_json.PrintFile(&examle_json);
    }
    char * json = GetStrJSON(&examle_json);
    TestNode(json);
    fclose(fp);
}

void test7() {
    char * filename = "../example.json";
    FILE * fp = fopen(filename, "r"); //открываем файл
    IOjson examle_json = {filename, fp, &PrintFilePoint};
    if (fp == NULL) {
        perror("Error occured while opening \"");
        perror(filename);
        perror("\"\n\r");
    } else {
        examle_json.PrintFile(&examle_json);
    }
    char * json = GetStrJSON(&examle_json);
    TestNode(json);
    fclose(fp);
}

int CalcGrafSumm(size_t i, const Array points) {
    Node* root = points.items[i];
    const char* value_str = AsString(MapAt(&root->map_value, "type")->value);
    if (!strcmp(value_str, "RESULT")) {
        return CalcGrafSumm(AsInt(MapAt(&root->map_value, "node")->value) - 1, points);
    } else if (!strcmp(value_str, "SUM")) {
        double nodes = AsDouble(MapAt(&root->map_value, "node")->value);
        int first_node = (int)nodes - 1;
        int second_node = ceil(modf(nodes, &nodes) * 10) - 1;
        return CalcGrafSumm(first_node, points) + CalcGrafSumm(second_node, points);
    } else if (!strcmp(value_str, "CONST")) {
        return AsInt(MapAt(&root->map_value, "val")->value);
    }
}

void test8() {
    char* json_str = "{\
        \"elements\": [\
            {\
                \"id\": 1,\
                \"type\": \"CONST\",\
                \"val\": 42,\
            },\
            {\
                \"id\": 2,\
                \"type\": \"CONST\",\
                \"val\": 18,\
            },\
            {\
                \"id\": 3,\
                \"type\": \"CONST\",\
                \"val\": 3,\
            },\
            {\
                \"id\": 4,\
                \"type\": \"SUM\",\
                \"node\": 2.3\
            },\
            {\
                \"id\": 5,\
                \"type\": \"SUM\",\
                \"node\": 1.4\
            },\
            {\
                \"id\": 6,\
                \"type\": \"RESULT\"\
                \"node\": 5\
            }\
        ]\
    }";
    Node* doc = TestNode(json_str);
    int res = 0;

    for (int i = 0; i < AsMap(doc).size; i++) {
        const Array points = AsArray(AsMap(doc).items[i]->value);
        printf("%d", CalcGrafSumm(points.size - 2, points)); // следует поправить size почему то на 1 больше
    }
}

int main() {
    //test0();
    //test1();
    //test2();
    //test3();
    //test4();
    //test5();
    //test6();
    //test7();
    test8();
    return 0;
}