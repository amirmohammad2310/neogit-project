#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

char neopath[1000];

int run_init(int argc, char *argv[]);
int config_global_name(int argc, char *argv[]);
int config_global_email(int argc, char *argv[]);
int config_global_alias(int argc, char *argv[]);
int config_local_name(int argc, char *argv[]);
int config_local_email(int argc, char *argv[]);
int config_local_alias(int argc, char *argv[]);
int add(char * resolved_path , int _n , int argc , char*argv[]);
int add_file(char token[] , char resolved_path[]);
int is_same(char direc1[] , char direc2[] , char filename1[] , char filename2[]);
int add_directory(char token[], char repo_r[]);
int is_exist_neo(int argc, char *argv[]);
int run_alias_command(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stdout, "please enter a valid command");
        return 1;
    }
    else if (strcmp(argv[1], "init") == 0)
    {
        return run_init(argc, argv);
    }
    else if (strcmp(argv[1], "config") == 0)
    {
        if (strcmp(argv[2], "-global") == 0)
        {
            mkdir("/mnt/e/data", 0755);

            if (strcmp(argv[3], "user.name") == 0)
            {
                config_global_name(argc, argv);
            }
            else if (strcmp(argv[3], "user.email") == 0)
            {
                config_global_email(argc, argv);
            }
            if (strncmp(argv[3], "alias.", 6) == 0)
            {
                config_global_alias(argc, argv);
            }
        }
        else if (strcmp(argv[2], "user.name") == 0)
        {
            config_local_name(argc, argv);
        }
        else if (strcmp(argv[2], "user.email") == 0)
        {
            config_local_email(argc, argv);
        }
        else if (strncmp(argv[2], "alias.", 6) == 0)
        {
            config_local_alias(argc, argv);
        }
    }
    else if (strcmp(argv[1], "add") == 0)
    {
        if(is_exist_neo(argc , argv)==0){
        
        if(strcmp(argv[2] , "-n" )==0)
        {
            
            char resolved_path[1000];
            if(realpath(argv[3] , resolved_path))
            {
                add(resolved_path , 1  , argc , argv);
            }
            else
            {
                perror("path not found");
            }
        }
        else if(strcmp(argv[2] , "-f")==0)
        {
            for (int i = 3 ; i<argc ; i++)
            {
            char resolved_path[1000];
            if(realpath(argv[i] , resolved_path))
            {
                add(resolved_path , 0  , argc , argv);
            }
            else
            {
                perror("path not found");
            }
            }
        }
        else if (argc > 3)
        {
            printf("%d\n" , argc);
            for (int i=2 ; i<argc ; i++){
            char resolved_path[1000];
            if(realpath(argv[i] , resolved_path))
            {
                add(resolved_path , 0 , argc , argv);
            }
            else
            {
                perror("path not found");
            }
            }
        }
        else 
        {
            char resolved_path[1000];
            if(realpath(argv[2] , resolved_path))
            {
                add(resolved_path , 0 , argc , argv);
            }
            else
            {
                perror("path not found");
            }
        }
        }
        else 
        {
            perror("please first initilize a repo");
        }

        
    }
    else
    {
        run_alias_command(argc, argv);
    }

    return 0;
}

int is_exist_neo(int argc, char *argv[])
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;
    char tmp_cwd[1024];
    bool exists = false;
    struct dirent *entry;
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        perror("Error opening current directory");
        return 1;
    }

    do
    {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            perror("Error opening current directory");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
            {
                exists = true;
                getcwd(neopath, sizeof(neopath));
                break;
            }
        }
        closedir(dir);

        // update current working directory
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
        {
            perror("error in getting cwd");
            return 1;
        }

        // change cwd to parent
        if (strcmp(tmp_cwd, "/") != 0)
        {
            if (chdir("..") != 0)
            {
                perror("error in changing directory");
                return 1;
            }
        }

    } while (strcmp(tmp_cwd, "/") != 0);

    if (!exists)
    {
        strcmp(neopath, " ");
        chdir(cwd);
        return -1;
    }
    chdir(cwd);
    return 0;
}
int run_init(int argc, char *argv[])
{
    bool exists = false;
    if (is_exist_neo(argc, argv) == -1)
    {
        exists = true;
    }
    if (exists)
    {
        mkdir(".neogit", 0755);
        mkdir(".neogit/config", 0755);
        mkdir(".neogit/config/aliases", 0755);
    }
    else
    {
        perror("neogit repository has already initialized");
    }
    return 0;
}
int config_global_name(int argc, char *argv[])
{
    FILE *globalname = fopen("/mnt/e/data/nameconfig.txt", "w");
    if (fprintf(globalname, "%s", argv[4]) < 0)
    {
        perror("error in write global name\n");
        return 1;
    }
    fclose(globalname);
    return 0;
}
int config_global_email(int argc, char *argv[])
{

    FILE *globalemail = fopen("/mnt/e/data/emailconfig.txt", "w");
    if (fprintf(globalemail, "%s", argv[4]) < 0)
    {
        perror("error in write global name\n");
        return 1;
    }
    fclose(globalemail);
    return 0;
}
int config_global_alias(int argc, char *argv[])
{
    mkdir("/mnt/e/data/aliases", 0755);
    char aliascommand[1000];
    sscanf(argv[3], "alias.%s", aliascommand);
    strcat(aliascommand, ".txt");
    char aliascommandpath[1000];
    strcpy(aliascommandpath, "/mnt/e/data/aliases/");
    strcat(aliascommandpath, aliascommand);
    FILE *globalaliases = fopen(aliascommandpath, "w");
    if (fprintf(globalaliases, "%s", argv[4]) < 0)
    {
        perror("error in write global name\n");
        return 1;
    }
    fclose(globalaliases);

    return 0;
}
int config_local_name(int argc, char *argv[])
{
    if (is_exist_neo(argc, argv) == -1)
    {
        perror("please first intialized a repsitory!");
        return 1;
    }
    else if (is_exist_neo(argc, argv) == 0)
    {
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        chdir(neopath);
        chdir("./.neogit");
        FILE *nameconfig = fopen("./config/username.txt", "w");
        if (fprintf(nameconfig, "%s", argv[3]) < 0)
        {
            perror("please enter the username correct");
            return 1;
        }
        chdir(cwd);
        fclose(nameconfig);
    }
    return 0;
}
int config_local_email(int argc, char *argv[])
{
    if (is_exist_neo(argc, argv) == -1)
    {
        perror("please first intialized a repsitory!");
        return 1;
    }
    else if (is_exist_neo(argc, argv) == 0)
    {
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        chdir(neopath);
        chdir("./.neogit");
        FILE *emailconfig = fopen("./config/useremail.txt", "w");
        if (fprintf(emailconfig, "%s", argv[3]) < 0)
        {
            perror("please enter the username correct");
            return 1;
        }
        chdir(cwd);
        fclose(emailconfig);
    }
    return 0;
}
int config_local_alias(int argc, char *argv[])
{
    if (is_exist_neo(argc, argv) == -1)
    {
        perror("please first intialized a repsitory!");
        return 1;
    }
    else if (is_exist_neo(argc, argv) == 0)
    {
        char aliasname[1000];
        sscanf(argv[2], "alias.%s", aliasname);
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        if (chdir(neopath) != 0)
        {
            perror("error in changing directory");
            return 1;
        }
        if (chdir("./.neogit/config/aliases") != 0)
        {
            perror("error in changing directory");
            return 1;
        }
        strcat(aliasname, ".txt");
        FILE *aliasconfig = fopen(aliasname, "w");
        if (fprintf(aliasconfig, "%s", argv[3]) < 0)
        {
            perror("please enter the username correct");
            return 1;
        }
        chdir(cwd);
        fclose(aliasconfig);
    }
    return 0;
}
int run_alias_command(int argc, char *argv[])
{

    if (is_exist_neo(argc, argv) == 0)
    {
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        if (chdir(neopath) != 0)
        {
            perror("error in changing directory");
            return 1;
        }
        if (chdir("./.neogit/config/aliases") != 0)
        {
            perror("error in changing directory");
            return 1;
        }
        char nameoffile[1000];
        strcpy(nameoffile, argv[1]);
        strcat(nameoffile, ".txt");
        FILE *alias = fopen(nameoffile, "r");
        if (alias == NULL)
        {

            if (chdir(cwd) != 0)
            {
                perror("error in changing directory");
                return 1;
            }
            char aliaspath[1000];
            strcpy(aliaspath, "/mnt/e/data/aliases/");
            strcat(aliaspath, argv[1]);
            strcat(aliaspath, ".txt");
            FILE *galias = fopen(aliaspath, "r");
            if (galias == NULL)
            {
                perror("command not found");
                return 1;
            }
            else
            {
                char command[1000];
                fgets(command, sizeof(command) - 1, galias);
                fclose(galias);
                system(command);
            }
        }
        else
        {
            char command[1000];
            fgets(command, sizeof(command) - 1, alias);
            fclose(alias);
            system(command);
        }
        return 0;
    }
}
int add(char * resolved_path , int _n , int argc , char*argv[])
{               is_exist_neo(argc,argv);
                char temp[1000];
                getcwd(temp, sizeof(temp));
                chdir(neopath);
                chdir(".neogit");
                mkdir("stagearea", 0755);
                char relative[1000];
                memmove(relative, resolved_path + strlen(neopath) + 1, strlen(resolved_path + strlen(neopath) + 1));
                relative[strlen(resolved_path + strlen(neopath))] = '\0';
                    if(_n){
                    chdir("stagearea");
                     char direc1[1000];
                    getcwd(direc1, sizeof(direc1));                    
                    mkdir(relative , 0755);
                    chdir(relative);
                    getcwd(direc1, sizeof(direc1));
                    chdir(resolved_path);
                    struct dirent *entry2;
                    DIR *dir2 = opendir(".");
                    char direc2[1000];
                    getcwd(direc2, sizeof(direc2));
                    while((entry2 = readdir(dir2)) != NULL){
                    if(entry2->d_name[0] == '.' || entry2->d_type == DT_DIR)
                    continue;
                    int staged = 0;
                    struct dirent *entry1;
                    DIR *dir1 = opendir(direc1);
                    while((entry1 = readdir(dir1)) != NULL){
                        if(strcmp(entry1->d_name, entry2->d_name) == 0){
                             staged = 1;
                            
                        }
                    }
                    
                    
                if(staged){
                printf("%s : staged\n", entry2->d_name);
                }else{
                printf("%s : not staged\n", entry2->d_name);
                }
                }
                return 0;
                }
                    
                chdir(temp);
                char *token = strtok(relative, "/");
                chdir(neopath);
                strcat(neopath, "/");
                strcat(neopath, token);
                struct stat entry;
                if(stat(token, &entry) == 0){
                if(S_ISDIR(entry.st_mode)){
                chdir(".neogit/stagearea");
                add_directory(token, neopath);
                }else{
                add_file(token, neopath);
                }
                }else perror("File not found");
                chdir(temp);
                return 0 ;
            
    }

// int is_same(char direc1[] , char direc2[] , char filename1[] , char filename2[])
// {
//     char direc1_c[1000];
//     strcpy(direc1_c , direc1);
//     char direc2_c[1000];
//     strcpy(direc2_c , direc2);
//     char filename1_c[1000];
//     strcpy(filename1_c , filename1);
//     char filename2_c[1000];

    
//     strcat(direc1_c ,"/" );
//     strcat(direc1_c ,filename1 );
//     strcat(direc2_c ,"/" );
//     strcat(direc2_c ,filename2 );
   
    
//     FILE * file1 , *file2 ;
//     if((file1 = fopen(direc1_c , "r"))==NULL) {perror("mistake in openint "); return 0 ;};
//     if((file2 = fopen(direc2_c , "r"))==NULL) {perror("mistake in openint "); return 0 ;};   
//     char ch1 , ch2 ;
//     int issame = 1 ;
//     while ((ch1 = fgetc(file1)) != EOF && (ch2 = fgetc(file2)) != EOF) {
//         if (ch1 != ch2) {
//             printf("%c   %c\n" , ch1  , ch2);
//             issame = 0;
//             break;
//         }
//     }
//     if (ch1 != ch2) {
//         printf("%c   %c\n" , ch1  , ch2);
//         issame = 0;
//     }
//     if (issame) {
//         puts("same");
//         return 1 ;
//     } else {
//         puts("not same");
//         return 0 ;
//     }

//     fclose(file1);
//     fclose(file2);


// }
        


int add_file(char filename[], char file[]){
    char current[1000];
    getcwd(current, 1000);
    char command[1000];
    strcat(command , "cp ");
    strcat(command ,file);
    strcat(command ," ");
    strcat(command , current);
    strcat(command , "/.neogit/stagearea");
    system(command);
}
int add_directory(char token[], char repo_r[]){
    mkdir(token, 0755);
    chdir(token);
    struct stat entry;
    char prev_token[100];
    strcpy(prev_token, token);
    token = strtok(NULL, "/");    
    if(token == NULL){
        char current[1000];
        getcwd(current, 1000);
        current[strlen(current) - strlen(prev_token) - 1] = '\0';
        char command[1000];
        strcat(command , "cp -r ");
        strcat(command , repo_r);
        strcat(command , " ");
        strcat(command , current);
        system(command);
        return 0;
    }
    strcat(repo_r, "/");
    strcat(repo_r, token);
    if(stat(repo_r, &entry) == 0){
        
        if(S_ISDIR(entry.st_mode)){
            add_directory(token, repo_r);
        }else{
            add_file(token, repo_r);
        }
    }else perror("File not found");
}

