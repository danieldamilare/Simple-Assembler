//Simple assembler 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define default_retun (char *) -1

/* function definition */
void  parse_line(char *);
int count_word(char * word);
void mov(char * instructions);
void inc(char * instructions);
void dec(char * instructions);
void add(char * instructions); 
void sub(char * instructions); 
void mul(char * instructions); 
void div_reg(char * instructions); 
void jmp(char * instructions); 
void cmp(char * instructions); 
void jne(char * instructions); 
void jge(char * instructions); 
void je(char * instructions); 
void jg(char * instructions); 
void jle(char * instructions); 
void jl(char * instructions); 
void call(char * instructions);
void call(char * instructions);
void call(char * instructions);
void call(char * instructions);
void command(char * instructions, char * op);


/* global variables */
char * output =  NULL; 
int reg[65536] = {0};
int cur_line = 0;
int stack[100] = {0};
int stack_top = 0;
int cmp_flag = 0;

typedef struct label {char * name; int pos;} label_t;
struct table{ label_t label[100]; int count;};
struct table TABLE = {.count = 0}; 

void add_label(char * name, int pos){
    TABLE.label[TABLE.count].name = name;
    TABLE.label[TABLE.count].pos = pos;
    TABLE.count++;
}

int get_label(char * name){
    for(int i = 0; i < TABLE.count; i++){
        if(strcmp(TABLE.label[i].name, name))
            return TABLE.label[i]. pos;
    }
    return -1;
}

enum TOKEN {
    MOV, INC, DEC, ADD, SUB, MUL, DIV, JMP, CMP, JNE, JE, JGE, JG, JLE, JL,
    CALL, RET, MSG, END, COMMENT
};

char const * const token_txt[] = {
    "mov", "inc", "dec", "add", "sub", "mul", "div", "jmp", "cmp", "jne", "je", "jge", "jg", "jle", "jl", "call", "ret", "msg", "end"
}; 


int count_word(char *word){
    char * dup = strdup(word);
    int i = 1;
    strtok(dup, "\n");
    while(strtok(NULL, "\n")) i++;
    free(dup);
    return i;
}

void parse_line(char * instruction){
}

void mov(char * instruction){
    command(instruction, "mov");
}

void inc(char *instruction) {
    char c;
    sscanf(instruction, "%*s %c", &c);
    reg[c]++;
}

void dec(char *instruction) {
    char c;
    sscanf(instruction, "%*s %c", &c);
    reg[c]--;
}

void command(char * instruction, char * op){
    char * temp = strdup(instruction); 
    char * temp_arr[3];
    temp_arr[0] = strtok(temp, " ");
    temp_arr[1] = strtok(NULL, " ");
    temp_arr[2] = strtok(NULL, " ");
    int value2; 
    int value1;

    if(isalpha(temp_arr[1][0]))
        value1 = reg[(int) temp_arr[1][0]];
    else
        value1 = atoi(temp_arr[1]);

    if(isalpha(temp_arr[2][0]))
        value2 = reg[(int) temp_arr[2][0]];
    else
      value2= atoi(temp_arr[2]);

    if(strcmp(op,  "add") == 0) 
        reg[(int)temp_arr[1][0]] += value2;
    if(strcmp(op, "sub") == 0)
        reg[(int)temp_arr[1][0]] -= value2;
    if(strcmp(op, "mul") == 0)
        reg[(int)temp_arr[1][0]] *= value2;
    if(strcmp(op, "mov") == 0)
        reg[(int)temp_arr[1][0]] = value2;
    if(strcmp(op, "div") == 0)
        reg[(int)temp_arr[1][0]] /= (value2 != 0? value2 : 1);
    if (strcmp(op, "cmp") == 0)
        cmp_flag = value1 < value2? 1: value1 > value2 ? 2: 0;


    free(temp);
}

void sub(char * instruction){ command(instruction, "sub");}
void add(char * instruction){ command(instruction, "add"); }
void mul(char * instruction){ command(instruction, "mul"); }
void div_reg(char * instruction){ command(instruction, "div");}
void cmp(char * instruction) {command(instruction, "cmp");}

void label(char * instruction){
    char * ptr = strchr(instruction, ':');
    char  word[ptr-instruction];
    for(char * s = instruction, * w = word; s < ptr; s++)
       *w = *s; 
    if(get_label(word) == -1)return;
    char * new = strdup(word);
    add_label(new, cur_line);
}


char * simple_assembler(const char * program){
    char * temp  = strdup(program);
    int lines = count_word(temp), i = 0;
    char * codefile[lines];
    char * word = strtok(temp, "\n");
    while(word){
        codefile[i++] = word;
        word = strtok(NULL, "\n");
    }

    for(int i = 0; i < lines; i++){
        cur_line = i;
        parse_line(codefile[i]);
    }
}

