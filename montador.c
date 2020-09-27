#include <stdio.h>
#include <string.h>

//definitions
#define TABLE_SIZE 21
enum section{BEFORE_SECTIONS, TEXT, DATA};
typedef struct
{
    char mnemonic[10];
    int number_of_operators;
    int code;
    int size;
    int is_instructions;
}
operation;

//global variables
operation operations[TABLE_SIZE];

//prototypes
void miss_usage(void);
int pre_processor(char f_in_name[]);
int assembler(char f_in_name[]);
int pre_processor_single_pass(FILE *fp_out, FILE *fp_in);
void init_operations(void);
int table_position(char key[]);

int main(int argc, char *argv[])
{    
    //cheking call input
    if (argc != 3)
    {
        miss_usage();
        return 1; //wrong number of inputs
    }

    //analizing option
    if (strcasecmp(argv[1], "-p") == 0)
    {
        return pre_processor(argv[2]);
    }
    else if (strcasecmp(argv[1], "-o") == 0)
    {
        return assembler(argv[2]);
    }
    else
    {
        miss_usage();
        return 2; //wrong option
    }   
}

//print usage structions
void miss_usage(void)
{
    printf("Usage:\t./montador -p assemble_code_file.asm\n");
    printf("or:\t./montador -o pre_processed_file.pre\n");
    printf("More information at https://github.com/luanharaujo/assembler\n");
}

int pre_processor(char f_in_name[])
{
    //open input file
    FILE *fp_in = fopen(f_in_name, "r");
    if (fp_in == NULL)
    {
        printf("Error when trying to open the file %s\n", f_in_name);
        return 3;
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
        return 4;
    }

    int ret = pre_processor_single_pass(fp_out, fp_in);
    
    //all done bye bye
    fclose(fp_out);
    fclose(fp_in);
    return ret;
}

int pre_processor_single_pass(FILE *fp_out, FILE *fp_in)
{
    char word[64];
    enum section current_section = BEFORE_SECTIONS;
    int last_of_line = 0;

    while (fscanf(fp_in, "%s", word) != EOF)
    {
        int position = table_position(word);
        if (position == -1) //word is not on table
        {
            //must be a label
            int label_length = strlen(word);
            //lookin for the ":" to check if there are any spaces
            if (word[label_length - 1] != ':')
            {
                char temp[64];
                fscanf(fp_in, "%s", temp);
                if (strcasecmp(temp, ":") != 0)
                {
                    printf("Ërror: missing ':' after label\n");
                    return 5;
                }

                //fixing the label
                word[label_length] = ':';
                word[label_length + 1] = '\0';
            }

            //putting on the pre-precessed file
            fprintf(fp_out, "%s ", word);
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
                        fscanf(fp_in, "%s", word);
                        fprintf(fp_out, "%s\n", word);
                    }
                    else //is COPY
                    {
                        fscanf(fp_in, "%s", word);
                        //lookin for the "," to check if there are any spaces
                        int operator_length = strlen(word);
                        if (word[operator_length - 1] != ',')
                        {
                            char temp[64];
                            fscanf(fp_in, "%s", temp);
                            if (strcasecmp(temp, ",") != 0)
                            {
                                printf("Ërror: missing ',' after first operator of COPY\n");
                                return 6;
                            }

                            //fixing the operator
                            word[operator_length] = ',';
                            word[operator_length + 1] = '\0';
                        }
                        fprintf(fp_out, "%s ", word);
                        fscanf(fp_in, "%s", word);
                        fprintf(fp_out, "%s\n", word);
                    }
                }
            }
            else //isn't a instruction 
            {
                //TO DO
            }
        }
    }

    return 0;
}

int assembler(char f_in_name[])
{
    //TO DO
    return 0;
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
