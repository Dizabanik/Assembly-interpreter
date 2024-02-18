#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "asm.h"
#define reg long
#define NUM_REGISTERS 255
#define div divide
reg regs[NUM_REGISTERS];


#define TYPE_INT true
#define TYPE_STRING false
typedef struct AST AST;
struct AST{
    enum op {regi, str, imm, mov, inc, dec, add, sub, mul, div, label, jmp, cmp, jne, je, jge, jg, jle, jl, callret, call, retu, msg, end, pointer, START} op;
    AST** children;
    size_t child_num;
    bool type;
    union {
        reg vali;
        char* vals;
    };
};
enum err_type{
  tokenization_err = 0,
};
char* err_type_arr[] = {"tokenization", "parser", "interpreter"};
char* err_arr[] = {"null", "code is blank", "undefined symbol", "for this instruction expected", "for label expected", "unexpected instruction", "undefined label name", "unexpected instruction of type", "unexpected argument"};
bool ERR = false;
char* ccat(char* dest, const char* source){
  if(!dest && source) {
    dest = malloc(sizeof(char)*(strlen(source) + 1));
    strcpy(dest, source);
   }
   else if(!dest && !source){
    dest = malloc(sizeof(char)*2);
    dest[0] = ' ';
    dest[1] = '\0';
   }
   else if(dest && source){
    dest = realloc(dest, (strlen(dest)+strlen(source) + 1)*sizeof(char));
    strcat(dest, source);
   }
  else{ 
    return dest;
  }
  return dest;
}
char* ccat_c(char* dest, char c){
  if(!dest) {
    dest = malloc(sizeof(char)*2);
    dest[0] = c;
    dest[1] = '\0';
   }
  else{ 
    dest = realloc(dest, (strlen(dest) + 2)*sizeof(char));
    strncat(dest, &c, 1);
  }
  return dest;
}
void prerr(uint8_t type,uint8_t line,uint8_t column,uint8_t id){
  printf("%u:%u\t%s error: %s\n", line, column, err_type_arr[type], err_arr[id]);
}
void prerr_s(uint8_t type,uint8_t line,uint8_t column,uint8_t id, char* s){
  printf("%u:%u\t%s error: %s '%s'\n", line, column, err_type_arr[type], err_arr[id],s);
}
void prerr_c(uint8_t type,uint8_t line,uint8_t column,uint8_t id, char c){
  printf("%u:%u\t%s error: %s '%c'\n", line, column, err_type_arr[type], err_arr[id],c);
}
void nlprerr(uint8_t type,uint8_t id){
  printf("%s error: %s\n", err_type_arr[type], err_arr[id]);
}
void nlprerr_s(uint8_t type,uint8_t id, char* s){
  printf("%s error: %s '%s'\n", err_type_arr[type], err_arr[id],s);
}
void nlprerr_c(uint8_t type,uint8_t id, char c){
  printf("%s error: %s '%c'\n", err_type_arr[type], err_arr[id],c);
}
size_t tok_num = 0;
size_t tok = 0;
uint8_t numPlaces (int n) {
    if (n < 0) n = -n;
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    /*      2147483647 is 2^31-1 - add more ifs as needed
       and adjust this final return as well. */
    return 10;
}
#define isNum(x) (x >= '0' && x <= '9')
bool isAlp(char c){
  return (c != '`' && ((c >= 'A' && c <= 'Z') || (c >= '_' && c <= 'z')));
}
char** tokenize(const char* program){
    if(!program || strcmp(program, "") == 0){
        prerr(tokenization_err, 0, 0, 1);
        return NULL;
    }
    char** ret = malloc(sizeof(char*));
    size_t len = strlen(program);
    size_t numChars = 0;
    char* temp = NULL;
    size_t line = 1;
    size_t column = 0;
    for(size_t i = 0; i < len; i++){
        if(program[i] == ' ' || program[i] == '\t') {column++; continue;}
        if(program[i] == '\n'){ line++; column = 0; continue;}
        if(program[i] == ';') {
            i++;
            while(i<len && program[i] != '\n'){
                i++;
            }
            line++;
            column = 0;
            continue;
        }
        if(isAlp(program[i])){
            tok_num++;
            ret = realloc(ret, sizeof(char*)*tok_num);
            temp = malloc(sizeof(char)*2);
            temp[0] = program[i];
            temp[1] = '\0';
            numChars = 1;
            i++;
            while(i<len && (isAlp(program[i]) || isNum(program[i]))){
                numChars++;
                temp = realloc(temp, sizeof(char) * (numChars + 1));
                temp[numChars-1] = program[i];
                i++;
            }
            temp[numChars] = '\0';
            ret[tok_num-1] = temp;
            temp = NULL;
            i--;
        }
        else if(isNum(program[i])){
            tok_num++;
            ret = realloc(ret, sizeof(char*)*tok_num);
            temp = malloc(sizeof(char)*2);
            temp[0] = program[i];
            temp[1] = '\0';
            numChars = 1;
            i++;
            while(i<len && isNum(program[i])){
                numChars++;
                temp = realloc(temp, sizeof(char) * (numChars + 1));
                temp[numChars-1] = program[i];
                i++;
            }
            temp[numChars] = '\0';
            ret[tok_num-1] = temp;
            temp = NULL;
            i--;
        }
        else if(program[i] == '-'){
            tok_num++;
            ret = realloc(ret, sizeof(char*)*tok_num);
            temp = malloc(sizeof(char)*2);
            temp[0] = program[i];
            temp[1] = '\0';
            numChars = 1;
            i++;
            while(i<len && isNum(program[i])){
                numChars++;
                temp = realloc(temp, sizeof(char) * (numChars + 1));
                temp[numChars-1] = program[i];
                i++;
            }
            temp[numChars] = '\0';
            ret[tok_num-1] = temp;
            temp = NULL;
            i--;
        }
        
        else if(program[i] == '\''){
            tok_num++;
            ret = realloc(ret, sizeof(char*)*tok_num);
            temp = malloc(sizeof(char)*2);
            temp[0] = '\'';
            temp[1] = '\0';
            numChars = 1;
            i++;
            while(i<len){
                if(program[i] == '\'' && program[i-1] != '\\') break;
                numChars++;
                temp = realloc(temp, sizeof(char) * (numChars + 1));
                temp[numChars-1] = program[i];
                i++;
            }
            temp[numChars] = '\0';
            ret[tok_num-1] = temp;
            temp = NULL;
        }
        else if(program[i] == ',' || program[i] == ':'){
            tok_num++;
            ret = realloc(ret, sizeof(char*)*tok_num);
            ret[tok_num-1] = malloc(sizeof(char)*2);
            ret[tok_num-1][0] = program[i];
            ret[tok_num-1][1] = '\0';
        }
        else{
            prerr_c(tokenization_err, line, column, 2, program[i]);
        }
        column++;
    }
    return ret;
}
AST* Init(uint8_t op){
    AST* ret = malloc(sizeof(AST));
    ret->child_num = 0;
    ret->children = NULL;
    ret->op = op;
    ret->type = TYPE_INT;
    ret->vali = 0;
    return ret;
}
AST** labels = NULL;
size_t num_labels = 0;
AST* process_token(char** tokens, bool inLabel){
    AST* ret = NULL;
    if(strcmp(tokens[tok], "mov") == 0){
        tok++;
        ret = Init(mov);
        ret->child_num = 2;
        ret->children = malloc(sizeof(AST*) * 2);
        ret->children[0] = process_token(tokens,inLabel);
        if(tok < tok_num && tokens[tok][0] != ','){
            ERR = true;
            nlprerr_c(1,3,',');tok--;
        }
        tok++;
        ret->children[1] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "inc") == 0){
        tok++;
        ret = Init(inc);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "dec") == 0){
        tok++;
        ret = Init(dec);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "add") == 0){
        tok++;
        ret = Init(add);
        ret->child_num = 2;
        ret->children = malloc(sizeof(AST*) * 2);
        ret->children[0] = process_token(tokens,inLabel);
        if(tok < tok_num && tokens[tok][0] != ','){
            ERR = true;
            nlprerr_c(1,3,',');tok--;
        }
        tok++;
        ret->children[1] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "sub") == 0){
        tok++;
        ret = Init(sub);
        ret->child_num = 2;
        ret->children = malloc(sizeof(AST*) * 2);
        ret->children[0] = process_token(tokens,inLabel);
        if(tok < tok_num && tokens[tok][0] != ','){
            ERR = true;
            nlprerr_c(1,3,',');tok--;
        }
        tok++;
        ret->children[1] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "mul") == 0){
        tok++;
        ret = Init(mul);
        ret->child_num = 2;
        ret->children = malloc(sizeof(AST*) * 2);
        ret->children[0] = process_token(tokens,inLabel);
        if(tok < tok_num && tokens[tok][0] != ','){
            ERR = true;
            nlprerr_c(1,3,',');tok--;
        }
        tok++;
        ret->children[1] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "div") == 0){
        tok++;
        ret = Init(div);
        ret->child_num = 2;
        ret->children = malloc(sizeof(AST*) * 2);
        ret->children[0] = process_token(tokens,inLabel);
        if(tok < tok_num && tokens[tok][0] != ','){
            ERR = true;
            nlprerr_c(1,3,',');tok--;
        }
        tok++;
        ret->children[1] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "jmp") == 0){
        tok++;
        ret = Init(jmp);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "cmp") == 0){
        tok++;
        ret = Init(cmp);
        ret->child_num = 2;
        ret->children = malloc(sizeof(AST*) * 2);
        ret->children[0] = process_token(tokens,inLabel);
        if(tok < tok_num && tokens[tok][0] != ','){
            ERR = true;
            nlprerr_c(1,3,',');tok--;
        }
        tok++;
        ret->children[1] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "jne") == 0){
        tok++;
        ret = Init(jne);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "je") == 0){
        tok++;
        ret = Init(je);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "jge") == 0){
        tok++;
        ret = Init(jge);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "jg") == 0){
        tok++;
        ret = Init(jg);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "jle") == 0){
        tok++;
        ret = Init(jle);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "jl") == 0){
        tok++;
        ret = Init(jl);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "call") == 0){
        tok++;
        ret = Init(call);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
    }
    else if(strcmp(tokens[tok], "ret") == 0){
        tok++;
        ret = Init(retu);
    }
    else if(strcmp(tokens[tok], "msg") == 0){
        tok++;
        ret = Init(msg);
        ret->child_num = 1;
        ret->children = malloc(sizeof(AST*) * 1);
        ret->children[0] = process_token(tokens,inLabel);
        while(tok < tok_num && tokens[tok][0] == ','){
            tok++;
            ret->children = realloc(ret->children, sizeof(AST*) * (ret->child_num+1));
            ret->children[ret->child_num++] = process_token(tokens,inLabel);
        }
    }
    else if(strcmp(tokens[tok], "end") == 0){
        tok++;
        ret = Init(end);
    }
    else if(tokens[tok][0] == '\''){
        ret = Init(str);
        ret->vals = tokens[tok];
        memmove(ret->vals, ret->vals+1, strlen(ret->vals));
        tok++;
    }
    else if(isAlp(tokens[tok][0]) && tokens[tok][1] == '\0'){
        ret = Init(regi);
        ret->vali = tokens[tok][0];
        tok++;
    }
    else if(isNum(tokens[tok][0]) || tokens[tok][0] == '-'){
        ret = Init(imm);
        ret->vali = atol(tokens[tok]);
        tok++;
    }
    else if(isAlp(tokens[tok][0])){
        ret = Init(label);
        ret->vals = tokens[tok];
        tok++;
        if(tok < tok_num && tokens[tok][0] == ':'){
            if(inLabel) {
                AST* temp = ret;
                ret = Init(callret);
                ret->child_num = 1;
                ret->children = malloc(sizeof(AST*) * 1);
                ret->children[0] = temp;
                ret->children[0]->op = pointer;
                tok--;
                return ret;
            }
            tok++;
            num_labels++;
            labels = realloc(labels, sizeof(AST*) * num_labels);
            labels[num_labels-1] = ret;
            while(tok < tok_num){
                ret->children = realloc(ret->children, sizeof(AST*) * (ret->child_num+1));
                ret->children[ret->child_num] = process_token(tokens,true);
                if(ret->children[ret->child_num] == NULL){
                    break;
                }
                ret->child_num++;
                if(ret->children[ret->child_num-1]->op == retu || ret->children[ret->child_num-1]->op == end || ret->children[ret->child_num-1]->op == callret){
                    break;
                }
            }
        }
        else{
            ret->op = pointer;
        }
    }
    else{
        nlprerr_s(1,5,tokens[tok]);
        tok++;
    }
    return ret;
}

AST* pass1(char** tokens){
    AST* ret = Init(START);
    for(tok = 0; tok < tok_num;){
        ret->children = realloc(ret->children, sizeof(AST*) * (ret->child_num+1));
        ret->children[ret->child_num++] = process_token(tokens,false);
    }
    return ret;
}
AST* label_pass(AST* ast){
    AST* ret = ast;
    if(ret->op == pointer){
        for(size_t i = 0; i < num_labels;i++){
            if(strcmp(labels[i]->vals, ret->vals) == 0){
                ret->children = malloc(sizeof(AST*));
                ret->children[0] = labels[i];
            }
        }
        if(!ret->children){
            nlprerr_s(1,6,ret->vals);
        }
    }
    for(size_t i = 0; i < ret->child_num; i++){
        if(ret->children && ret->children[i]){
            ret->children[i] = label_pass(ret->children[i]);
        }
    }
    return ret;
}
AST* pass2(AST* ast){
    AST* ret = label_pass(ast);
    if(labels) free(labels);
    return ret;
}
reg getVali(AST* ast){
    if(ast->op == imm) return ast->vali;
    else{ return regs[ast->vali]; }
}
char* getVals(AST* ast){
    char* ret = malloc(sizeof(char));
    if(ast->op == imm) {
        ret = realloc(ret,sizeof(char)*(numPlaces(ast->vali)+2));
        sprintf(ret, "%ld", ast->vali);
    }
    else if(ast->op == regi) {
        ret = realloc(ret,sizeof(char)*(numPlaces(regs[ast->vali])+2));
        sprintf(ret, "%ld", regs[ast->vali]);
    }
    else if(ast->op == str) {
        ret = ast->vals;
    }
    else{
        nlprerr(2, 8);
    }
    return ret;
}

char* toPrint = NULL;

enum COMPARE { notEq, eq, greater, less} lastComp = 0;
bool interpret(AST* ast);
bool interpret_label(AST* ast){
    for(size_t i = 0; i < ast->child_num; i++){
        if(ast->children && ast->children[i]){
            if(interpret(ast->children[i])) return true;
        }
    }
    ERR = true;
    return false;
}
bool interpret(AST* ast){
    if(ast->op == end || ast->op == retu){
        return true;
    }
    if(ast->op == mov){
        regs[ast->children[0]->vali] = getVali(ast->children[1]);
    }
    else if(ast->op == inc){
        regs[ast->children[0]->vali]++;
    }
    else if(ast->op == dec){
        regs[ast->children[0]->vali]--;
    }
    else if(ast->op == add){
        regs[ast->children[0]->vali] += getVali(ast->children[1]);
    }
    else if(ast->op == sub){
        regs[ast->children[0]->vali] -= getVali(ast->children[1]);
    }
    else if(ast->op == mul){
        regs[ast->children[0]->vali] *= getVali(ast->children[1]);
    }
    else if(ast->op == div){
        reg test = getVali(ast->children[1]);
        if(test != 0) {
            regs[ast->children[0]->vali] /= getVali(ast->children[1]);
        }
        else{
            ERR = true;
        }
    }
    else if(ast->op == jmp){
        if(interpret_label(ast->children[0]->children[0])) return true;
    }
    else if(ast->op == call){
        if(!interpret_label(ast->children[0]->children[0])) { return false;}
    }
    else if(ast->op == callret){
        return interpret_label(ast->children[0]->children[0]);
    }
    else if(ast->op == jne){
        if(lastComp == notEq || lastComp == less || lastComp == greater){
            if(interpret_label(ast->children[0]->children[0])) return true;
        }
    }
    else if(ast->op == je){
        if(lastComp == eq){
            if(interpret_label(ast->children[0]->children[0])) return true;
        }
    }
    else if(ast->op == jge){
        if(lastComp == eq || lastComp == greater){
            if(interpret_label(ast->children[0]->children[0])) return true;
        }
    }
    else if(ast->op == jg){
        if(lastComp == greater){
            if(interpret_label(ast->children[0]->children[0])) return true;
        }
    }
    else if(ast->op == jle){
        if(lastComp == eq || lastComp == less){
            if(interpret_label(ast->children[0]->children[0])) return true;
        }
    }
    else if(ast->op == jl){
        if(lastComp == less){
            if(interpret_label(ast->children[0]->children[0])) return true;
        }
    }
    else if(ast->op == cmp){
        reg c1 = getVali(ast->children[0]);
        reg c2 = getVali(ast->children[1]);
        if(c1 == c2){
            lastComp = eq;
        }
        else if(c1 > c2){
            lastComp = greater;
        }
        else if(c1 < c2){
            lastComp = less;
        }
        else{
            lastComp = notEq;
        }
    }
    else if(ast->op == msg){
        for(size_t i = 0; i < ast->child_num; i++){
            if(ast->children && ast->children[i]){
                toPrint = ccat(toPrint, getVals(ast->children[i]));
            }
        }
    }
    else if(ast->op == label){
        return interpret_label(ast);
    }
    else{
        ERR = true;
        nlprerr_c(2, 7, ast->op);
        return false;
    }
    return false;
}
char* pass3(AST* ast){
    if(!interpret_label(ast)) return NULL;
    return toPrint;
}
char* assembler_interpreter (const char* program) {
    char* ret = NULL;
    char** tokens = tokenize(program);
    if(!tokens || ERR) return (char*)-1;
    AST* ast = pass1(tokens);
    if(!ast || ERR) return (char*)-1;
    ast = pass2(ast);
    ret = pass3(ast);
    if(ret && !ERR)
    return ret;
    return (char*)-1;
}
