#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum TYPE{
    type_c,
    type_cpp,
    ERROR,
};


char *slice(const char *s, int start, int end) {
    int len = end - start;
    char *out = malloc(len + 1);   // +1 for '\0'
    memcpy(out, s + start, len);
    out[len] = '\0';
    return out;
}


enum TYPE get_type(char* name){
    int last_char_ix = strlen(name)-2; 
    for(int i = last_char_ix; i >=0; i--){
        if(name[i] == '.'){
            char* str_type = slice(name, i+1, last_char_ix+2);
            //printf("sliced: %s\n", str_type);
            enum TYPE type;
            if(strcmp(str_type, "c") == 0){
                type = type_c;
            }else if(strcmp(str_type, "cpp") == 0){
                type = type_cpp;
            }else{
                printf(".type error\n");
                type = ERROR;
            }
            free(str_type);
            return(type);
        }
    }
}



int main(int argc, char** argv){
    if(argc < 4){
        printf("argv error\n");
        for(int i=0; i<argc; i++){
            printf("argc[%d] = %s\n", i, argv[i]);
        }
        return 1;
    }

    char* dir = malloc(sizeof(*dir)*200);
    strcpy(dir, argv[1]);
    char* name = malloc(sizeof(*name)*200);
    strcpy(name, argv[2]);
    int mode = atoi(argv[3]); 
    // printf("path: %s\nname: %s\nmode: %d\n", dir, name, mode);
    


    enum TYPE type = get_type(name);

    char* clean_name;
    char* compile_with = malloc(sizeof(*compile_with)*10);
    if(type == type_c){
        strcpy(compile_with, "gcc");
        clean_name = slice(name, 0, strlen(name)-2);
    }else if(type == type_cpp){
        strcpy(compile_with, "g++");
        clean_name = slice(name, 0, strlen(name)-4);
    }

    char* bash_command = malloc(sizeof(*bash_command)*500);
    if(mode == 0){
        sprintf(bash_command, "cd '%s' && %s '%s' -o '%s' && '%s'/'%s'", dir, compile_with, name, clean_name, dir, clean_name);
    }else if(mode == 1){
        sprintf(bash_command,
        "cd '%s' && %s '%s' -o '%s' -fsanitize=address -fsanitize=leak -g && '%s'/'%s'",
        dir, compile_with, name, clean_name, dir, clean_name);
    }else if(mode == 2){
        sprintf(bash_command,
        "cd '%s' && %s '%s' -o '%s' -g && valgrind --leak-check=full --track-origins=yes '%s'/'%s'",
        dir, compile_with, name, clean_name, dir, clean_name);
    }
    for(int i =4; i <argc; i++){
        strcat(bash_command, " ");
        strcat(bash_command, argv[i]);
    }
    int r = system(bash_command);

    free(compile_with);
    free(clean_name);
    free(dir);
    free(name);
    
    return 0;
}



