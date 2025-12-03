#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum TYPE{
    type_c,
    type_cpp,
    ERROR,
};


void slice(char* dest, char* src, int start, int end) {
    int len = end - start;
    memcpy(dest, src + start, len);
    dest[len] = '\0';
}


enum TYPE get_type(char* name){
    int last_char_ix = strlen(name)-2; 
    for(int i = last_char_ix; i >=0; i--){
        if(name[i] == '.'){
            char str_type[50];
            slice(str_type, name, i+1, last_char_ix+2);
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

    char dir[100];
    strcpy(dir, argv[1]);
    char name[100];
    strcpy(name, argv[2]);
    int mode = atoi(argv[3]); 
    // printf("path: %s\nname: %s\nmode: %d\n", dir, name, mode);
    


    enum TYPE type = get_type(name);

    char clean_name[100];
    char compile_with[10];
    if(type == type_c){
        strcpy(compile_with, "gcc");
        slice(clean_name, name, 0, strlen(name)-2);
    }else if(type == type_cpp){
        strcpy(compile_with, "g++");
        slice(clean_name, name, 0, strlen(name)-4);
    }

    char bash_command[1000];
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
    
    return 0;
}



