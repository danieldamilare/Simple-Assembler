//Simple assembler 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define default_return (char *) -1


/* type declaration */
typedef struct label {char * name; int pos;} label_t;
typedef struct dstring{int length; int size; char * content;} dstr;
struct table{ label_t label[100]; int count;};

/* function definition */
void  parse_line(char *);
int count_word(char * word);
void inc(char * instruction);
void dec(char * instruction);
void jmp(char * instruction); 
void cmp(char * instruction); 
void jump(char * label);
void label(char * instruction);
void process_jump(char * instruction); 
char * lstrip(char * word); 
void command(char * instruction);
int append_string(dstr ** str, char * word);
void msg(char * instruction);
void ret(char * instruction);

/* global variables */
dstr * output =  NULL; 
int reg[65536] = {0};
int cur_line = 0;
int stack[100] = {0};
int stack_top = 0;
int cmp_flag = 0;
struct table TABLE = {.count = 0}; 

int append_string(dstr ** str, char * word){
    if(*str == NULL){
        *str = malloc(sizeof(dstr));
        if(*str == NULL) return -1;
        (*str)->size = 50;
        (*str)->content = malloc((*str)->size);
        if((*str)->content == NULL) {
            free(*str);
            return -1;
        }
        (*str)->length = 0;
    }
    int length = (*str)->length + strlen(word);
    if(length > (*str)->size){
       char * new_str = realloc((*str)->content, length + 50);
       if(new_str == NULL) return -1;
       (*str)->content = new_str;
       (*str)->size = length+50;
    }
    memcpy((*str)->content+(*str)->length, word, strlen(word));
    (*str)->length = length;
    (*str)->content[length] ='\0';
    return 0;
}

void add_label(char * name, int pos){
    TABLE.label[TABLE.count].name = name;
    TABLE.label[TABLE.count].pos = pos;
    TABLE.count++;
}

int get_label(char * name){
    for(int i = 0; i < TABLE.count; i++){
        if(strcmp(TABLE.label[i].name, name) == 0)
            return TABLE.label[i].pos;
    }
    return -1;
}


int count_word(char *word){
    char * dup = strdup(word);
    int i = 1;
    strtok(dup, "\n");
    while(strtok(NULL, "\n")) i++;
    free(dup);
    return i;
}

void parse_line(char * instruction){
    printf("In parse line: instruction: %s\n", instruction);
    char * s = lstrip(instruction);

    if(strncmp(s, "mov", 3) == 0 || strncmp(s, "add", 3) == 0|| strncmp(s, "mul", 3) == 0 
       || strncmp(s, "div", 3) == 0 || strncmp(s, "cmp", 3) == 0)
        command(s);

    else if(strncmp(s, "jmp", 3) == 0 || strncmp(s, "jne", 3) == 0 || strncmp(s, "jge", 3) == 0
            || strncmp(s, "call", 3) == 0 || strncmp(s, "je", 2) == 0 || strncmp(s, "jl", 2) == 0
            || strncmp(s, "jle", 3) == 0 || strncmp(s, "jg", 2) == 0)
        process_jump(s);

    else if(strncmp(s, "inc", 3) == 0) inc(s);
    else if(strncmp(s, "dec", 3) == 0) dec(s);
    else if(strncmp(s, "msg", 3) == 0) msg(s);
    else if(strncmp(s, "ret", 3) == 0) ret(s);
    else if(s[0] == ';')
        ; //ignore comment
    else
        if(strchr(s, ':')) label(instruction);
}

void inc(char *instruction) {
    char c;
    sscanf(instruction, "%*s %c", &c);
    reg[(int)c]++;
}

void dec(char *instruction) {
    char c;
    sscanf(instruction, "%*s %c", &c);
    reg[(int)c]--;
}

void command(char * instruction){
    printf("In command\n");
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

    if(strncmp(temp_arr[0],  "add", 3) == 0) 
    {
        printf("Adding\n");
        reg[(int)temp_arr[1][0]] += value2;
    }
    else if(strncmp(temp_arr[0], "sub", 3) == 0)
    {
        printf("Subtracting\n");
        reg[(int)temp_arr[1][0]] -= value2;
    }
    else if(strncmp(temp_arr[0], "mul", 3) == 0)
    {
        printf("MUltiplying\n");
        reg[(int)temp_arr[1][0]] *= value2;
    }
    else if(strncmp(temp_arr[0], "mov", 3) == 0)
    {
        printf("Moving\n");
        reg[(int)temp_arr[1][0]] = value2;
    }
    else if(strncmp(temp_arr[0], "div", 3) == 0)
    {
        printf("Dividing\n");
        reg[(int)temp_arr[1][0]] /= (value2 != 0? value2 : 1);
    }
    else if (strncmp(temp_arr[0], "cmp", 3) == 0)
    {
        printf("comparing\n");
        cmp_flag = value1 < value2? 1: value1 > value2 ? 2: 0;
   }

    free(temp);
}

void label(char * instruction){
    char * ptr = strchr(instruction, ':');
    char  word[ptr-instruction];
    for(char * s = instruction, * w = word; s < ptr; s++)
       *w = *s; 
    if(get_label(word) != -1)return;
    char * new = strdup(word);
    add_label(new, cur_line);
}

void msg(char * instruction){
    int size = 100;
    char * word =malloc(size);
    int index = 0;   
    char * ptr = instruction+3;
    int in_string = 0;

    while(*ptr != '\n' && *ptr != ';' && *ptr != '\0'){
        if(*ptr == '\'' && in_string){
            in_string = 0;
        }
        else if(*ptr =='\'' && !in_string){
            in_string = 1; 
        }
        else if (in_string) word[index++] = *ptr;
        else if(!in_string){ 
            if(*ptr != ',' && *ptr != ' '){
                int count = sprintf(word+index, "%d", reg[(int)*ptr]);
                index+=count;
            }
        }
        ptr++;
        if(index >= size){
            char * new_word = realloc(word, size*2);
            if(new_word == NULL) break;
            word = new_word;
            size *= 2;
        }
    }
    word[index] = '\0';
    append_string(&output, word);
    free(word);
}

void ret(char * instruction){
    if(stack_top == 0) return;
    else cur_line = stack[stack_top] -1; 
}

void jump(char * label){
    int pos;
    if((pos = get_label(label)) == -1) return;
    cur_line = pos -1;
}

void process_jump(char * instruction){
    char * temp = strdup(instruction);
    char * jmp_cmd = strtok(temp, " ");
    char * label = strtok(NULL, " ");

    if(get_label(label) == -1) return;

    if(strncmp(jmp_cmd, "jne", 3) == 0){
        if(cmp_flag != 0)jump(label);
    }
    else if(strncmp(jmp_cmd, "jmp", 3) == 0)
        jump(label);
    else if(strncmp(jmp_cmd, "je", 2) == 0){
        if(cmp_flag == 0) jump(label);
    }
    else if(strncmp(jmp_cmd, "jge", 3) == 0){
        if(cmp_flag == 0 || cmp_flag == 2) jump(label);
    }
    else if(strncmp(jmp_cmd, "jle", 3) == 0){
        if(cmp_flag == 1 || cmp_flag == 0) jump(label);
    }
    else if(strncmp(jmp_cmd, "jl", 2) == 0){
        if(cmp_flag == 1) jump(label);
    }
    else if(strncmp(jmp_cmd, "jg", 2) == 0){
        if(cmp_flag == 2) jump(label);
    } 
    else if(strncmp(jmp_cmd, "call", 3) == 0){
        stack[stack_top++] = cur_line;
        jump(label);
    }
    free(temp);
}

char * lstrip(char * word){
    while(*word == ' ') word++;
    return word;
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

    for(cur_line = 0; cur_line < lines; cur_line++){
        if(strncmp(codefile[cur_line], "end", 3) == 0) break;
        parse_line(codefile[cur_line]);
    }
    free(temp);
    if(output == NULL) return default_return; 
    char * return_str = strdup(output->content); //to be free by caller
    free(output->content);
    free(output);
    return return_str; 
}

int main(void){
    char * word = simple_assembler("; My first program\nmov  a, 5\ninc  a\ncall function\nmsg  '(5+1)/2 = ', a    ; output message\nend\nfunction:\ndiv  a, 2\nret");
    printf("reg a: %d\n", reg[(int)'a']);
    printf("word: %s\n", word);
}
