#include <stdio.h>
#include <string.h>

//definitions
#define TABLE_SIZE 21
#define MAX_TOKEN_NAME_SIZE 51 //one more than in the specification for the null-string-termination character
enum section{INVALID ,BEFORE_SECTIONS, TEXT, DATA};
typedef struct
{
    char mnemonic[10];
    int number_of_operators;
    int code;
    int size;
    int is_instructions;
}
operation;

#define MAX_MACROS 2
#define MAX_MACROS_ARGS 2
#define MAX_MACRO_FILE_NAME_SIZE 25 //.temp_macro_file_<number>
typedef struct
{
    char name[MAX_TOKEN_NAME_SIZE];
    int number_of_args;
    char args_names[MAX_MACROS_ARGS][MAX_TOKEN_NAME_SIZE];
    int exist;
    char file_name[MAX_MACRO_FILE_NAME_SIZE]; 
}
macro;

//global variables
operation operations[TABLE_SIZE];
macro macros[MAX_MACROS];

//prototypes
int my_fscanf(FILE *fp, char string[]);
void miss_usage(void);
int preprocessor(char f_in_name[]);
int assembler(char f_in_name[]);
int preprocessor_single_pass(FILE *fp_out, FILE *fp_in);
void init_operations(void);
void init_macros(void);
int table_position(char key[]);
enum section change_section(enum section current, char name_new_section[]);
int macro_index_finder(char word[]);
int clean_up_and_return(int error_code);
int file_exists(char file_name[]);
FILE* naming_and_opening_macro_file(int macro_index);
unsigned long copy_macro(FILE *fp_in, FILE *fp_macro);
void insert_macro(int index, FILE *fp_out, char parameters[MAX_MACROS_ARGS][MAX_TOKEN_NAME_SIZE]);
void remove_ending_comma(char *string);

int main(int argc, char *argv[])
{    
    //cheking call input
    if (argc != 3)
    {
        miss_usage();
        return 1; //wrong number of inputs
    }

    init_operations();
    init_macros();

    //analizing option
    if (strcasecmp(argv[1], "-p") == 0)
    {
        return preprocessor(argv[2]);
    }
    else if (strcasecmp(argv[1], "-o") == 0)
    {
        return assembler(argv[2]);
    }
    else
    {
        miss_usage();
        return clean_up_and_return(2); //wrong option
    }   
}

//print usage structions
void miss_usage(void)
{
    printf("Usage:\t./montador -p assemble_code_file.asm\n");
    printf("or:\t./montador -o pre_processed_file.pre\n");
    printf("More information at https://github.com/luanharaujo/assembler\n");
}

int preprocessor(char f_in_name[])
{
    //open input file
    FILE *fp_in = fopen(f_in_name, "r");
    if (fp_in == NULL)
    {
        printf("Error when trying to open the file %s\n", f_in_name);
        return clean_up_and_return(3);
    }

    //open output file
    int name_length = strlen(f_in_name);
    char f_out_name[name_length];
    strcpy(f_out_name, f_in_name);
    strcpy(f_out_name + name_length - 3, "pre");
    FILE *fp_out = fopen(f_out_name, "w");
    if (fp_out == NULL)
    {
        printf("Error when trying to create the file %s\n", f_out_name);
        fclose(fp_in);
        return clean_up_and_return(4);
    }

    int ret = preprocessor_single_pass(fp_out, fp_in);
    
    //all done bye bye
    fclose(fp_out);
    fclose(fp_in);
    return ret;
}

int preprocessor_single_pass(FILE *fp_out, FILE *fp_in)
{
    char word[MAX_TOKEN_NAME_SIZE];
    char last_label[MAX_TOKEN_NAME_SIZE];
    unsigned long possition_before_last_label;
    enum section current_section = BEFORE_SECTIONS;

    while (my_fscanf(fp_in, word) != EOF)
    {
        int position = table_position(word);
        if (position == -1) //first word in line isn't on the table
        {
            //let's see if it is a MACRO call...
            int index = macro_index_finder(word);
            if(index != -1)//is a MACRO call!
            {
                char parameters[MAX_MACROS_ARGS][MAX_TOKEN_NAME_SIZE];
                for (int i = 0; i < macros[index].number_of_args; i++)
                {
                    if (my_fscanf(fp_in, word) == EOF)
                    {
                        printf("Error: Missing arguments on a MACRO call in the last line of the file.\n");
                        return clean_up_and_return(13);
                    }
                    strcpy(parameters[i], word);
                    remove_ending_comma(parameters[i]);
                }
                insert_macro(index, fp_out, parameters);
            }
            else //ok, must be a label then.
            {
                int label_length = strlen(word);
                //lookin for the ":" to check if there are any spaces
                if (word[label_length - 1] != ':')
                {
                    char temp[MAX_TOKEN_NAME_SIZE];
                    my_fscanf(fp_in, temp);
                    if (strcasecmp(temp, ":") != 0)
                    {
                        printf("Error: missing ':' after label\n");
                        return clean_up_and_return(5);
                    }

                    //fixing the label
                    word[label_length] = ':';
                    word[label_length + 1] = '\0';
                }
                //storege for future usage
                strcpy(last_label, word);
                //removing the colum (':') character
                last_label[strlen(last_label) - 1] = '\0';
                fflush(fp_out);
                possition_before_last_label =  ftell(fp_out);
                //putting on the pre-precessed file
                fprintf(fp_out, "%s ", word);
            }
        }
        else //word is on table
        {
            if (operations[position].is_instructions)
            {
                //putting on the pre-precessed file
                if (strcasecmp(word, "STOP") == 0)
                {
                    fprintf(fp_out, "%s\n", word);
                }
                else
                {
                    fprintf(fp_out, "%s ", word);
                    //now the operators
                    if (strcasecmp(word, "COPY") != 0)
                    {
                        my_fscanf(fp_in, word);
                        fprintf(fp_out, "%s\n", word);
                    }
                    else //is COPY
                    {
                        my_fscanf(fp_in, word);
                        //lookin for the "," to check if there are any spaces
                        int operator_length = strlen(word);
                        if (word[operator_length - 1] != ',')
                        {
                            char temp[MAX_TOKEN_NAME_SIZE];
                            my_fscanf(fp_in, temp);
                            if (strcasecmp(temp, ",") != 0)
                            {
                                printf("Error: missing ',' after first operator of COPY\n");
                                return clean_up_and_return(6);
                            }

                            //fixing the operator
                            word[operator_length] = ',';
                            word[operator_length + 1] = '\0';
                        }
                        //putting on the pre-precessed file
                        fprintf(fp_out, "%s ", word);
                        my_fscanf(fp_in, word);
                        fprintf(fp_out, "%s\n", word);
                    }
                }
            }
            else //isn't a instruction 
            {
                if (strcasecmp(word, "SECTION") == 0)
                {
                    //putting on the pre-precessed file
                    fprintf(fp_out, "%s ", word);

                    //grab and cheking next token
                    my_fscanf(fp_in, word);
                    current_section = change_section(current_section, word);
                    if(current_section == INVALID)
                    {
                        return clean_up_and_return(7);
                    }

                    //putting on the pre-precessed file
                    fprintf(fp_out, "%s\n", word);
                }
                else if(strcasecmp(word, "SPACE") == 0)
                {
                    //TO DO
                }
                else if(strcasecmp(word, "CONST") == 0)
                {
                    //TO DO
                }
                else if(strcasecmp(word, "EQU") == 0)
                {
                    //TO DO
                }
                else if(strcasecmp(word, "IF") == 0)
                {
                    //TO DO
                }
                else if(strcasecmp(word, "MACRO") == 0)
                {
                    //to delete last label from outpup file
                    fseek(fp_out, possition_before_last_label, SEEK_SET);

                    //finding a blank spot on the macro vector
                    int macro_index = 0;
                    while (macro_index < MAX_MACROS && macros[macro_index].exist)
                    {
                        macro_index++;
                    }
                    if (macro_index == MAX_MACROS)
                    {
                        printf("Error: Exceeded maximum number of MACROS.");
                        return clean_up_and_return(9);
                    }
                    FILE *fp = naming_and_opening_macro_file(macro_index);
                    strcpy(macros[macro_index].name, last_label);
                    unsigned long fp_in_memory;
                    int is_parameter;
                    macros[macro_index].number_of_args = 0;
                    do
                    {
                       fflush(fp_in);
                       fp_in_memory = ftell(fp_in);
                       my_fscanf(fp_in, word);
                       if(word[0] == '&')
                       {
                           is_parameter = 1;
                           //storing with the comman, if it has one
                           strcpy(macros[macro_index].args_names[macros[macro_index].number_of_args++], word); 
                       }
                       else
                       {
                           is_parameter = 0;
                       }
                    }
                    while (is_parameter);

                    //putting the fp_in afther the last parameter
                    fseek(fp_in, fp_in_memory, SEEK_SET);

                    if (macros[macro_index].number_of_args > 2)
                    {
                        printf("Error: Each MACRO can have up to 2 arguments.\n");
                        return clean_up_and_return(10);
                    }
                    unsigned long new_position = copy_macro(fp_in, fp); //copy macro content in a file
                    macros[macro_index].exist = 1;
                    if(new_position == -1)
                    {
                        printf("Error: Unallowed operation within a MACRO.\n");
                        return clean_up_and_return(11);
                    }
                    if(new_position == -2)
                    {
                        printf("Error! MACRO must have a ENDMACRO after it.\n");
                        return clean_up_and_return(12);
                    }
                    fseek(fp_in, new_position, SEEK_SET);
                    fflush(fp);
                    fclose(fp);
                }
                else //ENDMACRO whitout MACRO before
                {
                    printf("Error! ENDMACRO must have a MACRO befor it.\n");
                    return clean_up_and_return(8);
                }
            }
        }
    }

    return clean_up_and_return(0);
}

int assembler(char f_in_name[])
{
    //TO DO
    return 0;
}

//check if is allowed to change from current section to a section named name_new_section
//if is, returns the new section, if isn't, print a error and return INVALID 
enum section change_section(enum section current, char name_new_section[])
{
    if (strcasecmp(name_new_section, "TEXT") == 0)
    {
        if (current == BEFORE_SECTIONS)
        {
            return TEXT;
        }
        else
        {
            printf("Error: SECTION TEXT defines twice.\n");
            return INVALID;
        }
    }
    else if (strcasecmp(name_new_section, "DATA") == 0)
    {
        if (current == BEFORE_SECTIONS)
        {
            printf("Error: SECTION DATA can't be defined before SECTION TEXT.\n");
            return INVALID;
        }
        else if (current == TEXT)
        {
            return DATA;
        }
        else
        {
            printf("Error: SECTION DATA defines twice.\n");
            return INVALID;
        }
    }
    else //invalid section
    {
        printf("Error: Invalid section token after SECTION.\n");
        return INVALID;
    }
}

//returns the vector position if the key is on the operations vector, return -1 if the key is not on the vector
int table_position(char key[])
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (strcasecmp(key, operations[i].mnemonic) == 0)
        {
            return i;
        }
    }
    return -1;
}

//returns the macro index if word is the name of a predefined macro
//returns -1 otherwise
int macro_index_finder(char word[])
{
    for (int i = 0; i < MAX_MACROS; i++)
    {
        if (macros[i].exist)
        {
            if (strcasecmp(word, macros[i].name) == 0)
            {
                return i;
            }
        }
    }
    return -1;
}

int clean_up_and_return(int error_code)
{
    //deleting all MACRO files
    for (int i = 0; i < MAX_MACROS; i++)
    {
        if (macros[i].exist)
        {
            remove(macros[i].file_name);
        }
    }
    return error_code;
}

//return 1 if the file exist otherwise return 0
int file_exists(char file_name[])
{
    //try to open file to read
    FILE *fp;
    if ((fp = fopen(file_name, "r")))
    {
        fclose(fp);
        return 1;
    }
    return 0;
}

FILE *naming_and_opening_macro_file(int macro_index)
{
    int i = 0;
    do
    {
        sprintf(macros[macro_index].file_name, ".temp_macro_file_%d", i++);
    }
    while (file_exists(macros[macro_index].file_name));

    return fopen(macros[macro_index].file_name, "w");
}

//copy a macro insade a temporary file
//return -1 if same thing goes wrong insade the MACRO
//returns -2 if the file ands before the MACRO
//otherwise return the fp_in offset afther the ANDMACRO  
unsigned long copy_macro(FILE *fp_in, FILE *fp_macro)
{
    char word[MAX_TOKEN_NAME_SIZE];
    if (my_fscanf(fp_in, word) == EOF)
    {
        return -2;
    }
    while (strcasecmp(word, "ENDMACRO") != 0)
    {
        int position = table_position(word);
        if (position == -1 || !operations[position].is_instructions)
        {
            printf("Error: All lines inside a MACRO must begin whit a intruction mnemonic.\n");
            return -1;
        }
        fprintf(fp_macro, "%s ", word);
        for(int i = 0; i < operations[position].number_of_operators; i++)
        {
            if (my_fscanf(fp_in, word) == EOF)
            {
                return -2;
            }
            fprintf(fp_macro,"%s ", word);
        }
        fprintf(fp_macro,"\n");
        if (my_fscanf(fp_in, word) == EOF)
        {
            return -2;
        }
    }
    fflush(fp_in);
    return ftell(fp_in);
}

//same as fscanf(fp_in, "%s", word) but ignores comments marked by ';'
int my_fscanf(FILE *fp, char string[])
{
    int comment = 0;
    char c;

    fflush(fp);
    unsigned long fp_memory = ftell(fp);

    if (fscanf(fp, "%s", string) == EOF)
    {
        return EOF;
    }

    //looking for the comment marker
    for (int i = 0; string[i] != '\0'; i++)
    {
        if(string[i] == ';')
        {
            comment = 1;
            string[i] = '\0';
            break;
        }
    }

    if (comment)
    {
       fseek(fp, fp_memory, SEEK_SET);
       do
       {
           fflush(fp);
           fp_memory = ftell(fp);
           if (fscanf(fp, "%c", &c) == EOF)
           {
               //comment in the last line of the code whitout \n in the end
               //ou em bom português, uma puta falta de sacanagem
               return 1;
           }
       }
       while (c != '\n');
    }
    return 1; 
}

//append a macro in the position index on the macro vector
//in the file poited by fp
void insert_macro(int index, FILE *fp_out, char parameters[MAX_MACROS_ARGS][MAX_TOKEN_NAME_SIZE])
{
    FILE *fp_in = fopen(macros[index].file_name, "r");
    char word[MAX_TOKEN_NAME_SIZE];
    
    while (fscanf(fp_in, "%s", word) != EOF)
    {
        fprintf(fp_out,"%s ", word);
        int position = table_position(word);
        for (int i = 0; i < operations[position].number_of_operators; i++)
        {
            fscanf(fp_in, "%s", word);
            
            //checking if is a argument
            for(int j = 0; j < macros[index].number_of_args; j++)
            {
                char tmp[MAX_TOKEN_NAME_SIZE];
                strcpy(tmp, macros[index].args_names[j]);
                remove_ending_comma(tmp);
                remove_ending_comma(word);
                if(strcasecmp(word, tmp) == 0)
                {
                    strcpy(word, parameters[j]);
                }
            }
            if((strcasecmp(operations[position].mnemonic, "COPY") == 0 ) && (i == 0))
            {
                fprintf(fp_out,"%s, ", word);
            }
            else
            {
                fprintf(fp_out,"%s ", word);
            }
            
        }
        fprintf(fp_out,"\n");
    }

    fclose(fp_in);
}

void remove_ending_comma(char *string)
{
    if (string[strlen(string) - 1] == ',')
    {
        string[strlen(string) - 1] = '\0';
    }
}

void init_macros(void)
{
    for (int i = 0; i < MAX_MACROS; i++)
    {
        macros[i].exist = 0;
    }
}

//filling in the operations vector
void init_operations(void)
{
    strcpy(operations[0].mnemonic, "ADD");
    operations[0].number_of_operators = 1;
    operations[0].code = 1;
    operations[0].size = 2;
    operations[0].is_instructions = 1;

    strcpy(operations[1].mnemonic, "SUB");
    operations[1].number_of_operators = 1;
    operations[1].code = 2;
    operations[1].size = 2;
    operations[1].is_instructions = 1;

    strcpy(operations[2].mnemonic, "MULT");
    operations[2].number_of_operators = 1;
    operations[2].code = 3;
    operations[2].size = 2;
    operations[2].is_instructions = 1;

    strcpy(operations[3].mnemonic, "DIV");
    operations[3].number_of_operators = 1;
    operations[3].code = 4;
    operations[3].size = 2;
    operations[3].is_instructions = 1;

    strcpy(operations[4].mnemonic, "JMP");
    operations[4].number_of_operators = 1;
    operations[4].code = 5;
    operations[4].size = 2;
    operations[4].is_instructions = 1;

    strcpy(operations[5].mnemonic, "JMPN");
    operations[5].number_of_operators = 1;
    operations[5].code = 6;
    operations[5].size = 2;
    operations[5].is_instructions = 1;

    strcpy(operations[6].mnemonic, "JMPP");
    operations[6].number_of_operators = 1;
    operations[6].code = 7;
    operations[6].size = 2;
    operations[6].is_instructions = 1;

    strcpy(operations[7].mnemonic, "JMPZ");
    operations[7].number_of_operators = 1;
    operations[7].code = 8;
    operations[7].size = 2;
    operations[7].is_instructions = 1;

    strcpy(operations[8].mnemonic, "COPY");
    operations[8].number_of_operators = 2;
    operations[8].code = 9;
    operations[8].size = 3;
    operations[8].is_instructions = 1;

    strcpy(operations[9].mnemonic, "LOAD");
    operations[9].number_of_operators = 1;
    operations[9].code = 10;
    operations[9].size = 2;
    operations[9].is_instructions = 1;

    strcpy(operations[10].mnemonic, "STORE");
    operations[10].number_of_operators = 1;
    operations[10].code = 11;
    operations[10].size = 2;
    operations[10].is_instructions = 1;

    strcpy(operations[11].mnemonic, "INPUT");
    operations[11].number_of_operators = 1;
    operations[11].code = 12;
    operations[11].size = 2;
    operations[11].is_instructions = 1;

    strcpy(operations[12].mnemonic, "OUTPUT");
    operations[12].number_of_operators = 1;
    operations[12].code = 13;
    operations[12].size = 2;
    operations[12].is_instructions = 1;

    strcpy(operations[13].mnemonic, "STOP");
    operations[13].number_of_operators = 0;
    operations[13].code = 14;
    operations[13].size = 1;
    operations[13].is_instructions = 1;

    strcpy(operations[14].mnemonic, "SECTION");
    operations[14].number_of_operators = 1;
    operations[14].code = -1;
    operations[14].size = 0;
    operations[14].is_instructions = 0;

    strcpy(operations[15].mnemonic, "SPACE");
    operations[15].number_of_operators = 0;
    operations[15].code = -1;
    operations[15].size = 1;
    operations[15].is_instructions = 0;

    strcpy(operations[16].mnemonic, "CONST");
    operations[16].number_of_operators = 1;
    operations[16].code = -1;
    operations[16].size = 1;
    operations[16].is_instructions = 0;

    strcpy(operations[17].mnemonic, "EQU");
    operations[17].number_of_operators = 1;
    operations[17].code = -1;
    operations[17].size = 0;
    operations[17].is_instructions = 0;

    strcpy(operations[18].mnemonic, "IF");
    operations[18].number_of_operators = 1;
    operations[18].code = -1;
    operations[18].size = 0;
    operations[18].is_instructions = 0;

    strcpy(operations[19].mnemonic, "MACRO");
    operations[19].number_of_operators = 0;
    operations[19].code = -1;
    operations[19].size = 0;
    operations[19].is_instructions = 0;

    strcpy(operations[20].mnemonic, "ENDMACRO");
    operations[20].number_of_operators = 0;
    operations[20].code = -1;
    operations[20].size = 0;
    operations[20].is_instructions = 0;
}
