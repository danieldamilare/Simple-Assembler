//Simple assembler 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define default_return (char *) -1


/* type declaration */
typedef struct dstring{int length; int size; char * content;} dstr;

struct abuf {
  char *b;
  int len;
};
/* function definition */
void  parse_line(char *);
int count_word(char * word);
void inc(char * instruction);
void dec(char * instruction);
void jmp(char * instruction); 
void cmp(char * instruction); 
void jump(char * label);
void process_jump(char * instruction); 
char * lstrip(char * word); 
void command(char * instruction);
void msg(char * instruction);
void ret(char * instruction);

/* global variables */

struct abuf output = {NULL, 0};
int reg[128] = {0};
int cur_line = 0;
int stack[100] = {0};
int stack_top = 0;
int total_line = 0;
int cmp_flag = 0;
char ** code;

void append_string(struct abuf *ab, const char *s) {
  int len = strlen(s); 
  char *new = realloc(ab->b, ab->len + len);
  if (new == NULL) return;
  memcpy(&new[ab->len], s, len);
  ab->b = new;
  ab->len += len;
}
void abFree(struct abuf *ab) {
    if(ab->b)
        free(ab->b);
}
int count_word(char *word){
    if(word == NULL) return 0;
    char * dup = strdup(word);
    if(dup == NULL) return 0;
    int i = 1;
    strtok(dup, "\n");
    while(strtok(NULL, "\n")) i++;
    free(dup);
    return i;
}

void parse_line(char * instruction){
    if(instruction == NULL)return; 
    char * s = lstrip(instruction);

    if(strncmp(s, "mov", 3) == 0 || strncmp(s, "add", 3) == 0|| strncmp(s, "mul", 3) == 0 
       || strncmp(s, "div", 3) == 0 || strncmp(s, "cmp", 3) == 0 || strncmp(s, "sub", 3) == 0)
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
    char * temp = strdup(instruction); 
    if(temp == NULL) return;
    char * temp_arr[3];
    temp_arr[0] = lstrip(strtok(temp, " "));
    temp_arr[1] = lstrip(strtok(NULL, " ,"));
    temp_arr[2] = lstrip(strtok(NULL, " ,"));
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
        reg[(int)temp_arr[1][0]] += value2;
    }
    else if(strncmp(temp_arr[0], "sub", 3) == 0)
    {
        reg[(int)temp_arr[1][0]] -= value2;
    }
    else if(strncmp(temp_arr[0], "mul", 3) == 0)
    {
        reg[(int)temp_arr[1][0]] *= value2;
    }
    else if(strncmp(temp_arr[0], "mov", 3) == 0)
    {
        reg[(int)temp_arr[1][0]] = value2;
    }
    else if(strncmp(temp_arr[0], "div", 3) == 0)
    {
        reg[(int)temp_arr[1][0]] /= (value2 != 0? value2 : 1);
    }
    else if (strncmp(temp_arr[0], "cmp", 3) == 0)
    {
        cmp_flag = value1 < value2? 1: value1 > value2 ? 2: 0;
   }

    free(temp);
}

void msg(char * instruction){
    int size = 100;
    char * word =malloc(size);
    if(word ==NULL) return;
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
    else cur_line = stack[--stack_top]; 
}

void jump(char * label){
    int pos;
    char new_word[strlen(label) + 2]; 
    sprintf(new_word, "%s:", label);
    for( pos = 0; pos < total_line; pos++){
        if(strcmp(code[pos], new_word) == 0){
            cur_line = pos-1;
            return;
        }
    }
}

void process_jump(char * instruction){
    char * temp = strdup(instruction);
    if(temp == NULL)return;
    char * jmp_cmd = strtok(temp, " ");
    char * label = strtok(NULL, " ");
    label = lstrip(label);

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
    if(word == NULL || strlen(word) ==0)  return NULL;
    while(isspace(*word))word++;
    return word;
} 

char * assembler_interpreter(const char * program){
    int end_flag = 0;
    char * temp  = strdup(program);
    if(temp == NULL) return NULL;
    total_line = count_word(temp);
    int i = 0;
    char * codefile[total_line];
    code = codefile;
    char * word = strtok(temp, "\n");
    while(word){
        codefile[i++] = word;
        word = strtok(NULL, "\n");
    }

    for(cur_line = 0; cur_line < total_line; cur_line++){
        if(strncmp(codefile[cur_line], "end", 3) == 0){
            end_flag=1;
            break;
        } 
        parse_line(codefile[cur_line]);
    }
    free(temp);

    if(end_flag){ //if end in code return output 
        if(output.b)
            return output.b;
        else return strdup("");
    }
    abFree(&output);
    return default_return; 
   
}


int main(void){
    char * word = assembler_interpreter(\
"mov h, 15   ; instruction mov h, 15\n\
mov p, 7   ; instruction mov p, 7\n\
call func\n\
msg 'Random result: ', v\n\
end\n\
func:\n\
	cmp h, p\n\
	jl exit\n\
	mov v, h\n\
	sub v, p\n\
	ret\n\
; Do nothing\n\
exit:\n\
	msg 'Do nothing'\n"
);
    printf("reg a: %d\n", reg[(int)'a']);
    if(word == default_return){
        printf("Passed Test!\n");
        return 0;
    }
    printf("word: %s\n", word);
    free(word);
}
