#include <stdio.h>
#include <string.h>

//prototypes
void miss_usage(void);
int pre_processor(char f_in_name[]);
int assembler(char f_in_name[]);
int pre_processor_single_pass(FILE *fp_out, FILE *fp_in);

int main(int argc, char *argv[])
{    
    //cheking call input
    if (argc != 3) 
    {
        miss_usage();
        return 1; //wrong number of inputs
    }

    printf("1\n");

    //analizing option
    if (strcmp(argv[1], "-p") == 0)
    {
        return pre_processor(argv[2]);
    }
    else if (strcmp(argv[1], "-o") == 0)
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
    if(fp_out == NULL)
    {
        printf("Error when trying to criate the file %s\n", f_out_name);
        fclose(fp_in);
        return 4;
    }

    printf("1\n");
    int ret = pre_processor_single_pass(fp_out, fp_in);
    printf("2\n");
    
    //all done bye bye
    fclose(fp_out);
    fclose(fp_in);
    return ret;
}

int pre_processor_single_pass(FILE *fp_out, FILE *fp_in)
{
    char palavra[500];

    while(fscanf(fp_in, "%s", palavra) != EOF)
    {
        printf("%s\n", palavra);
    }

    return 0;
}

int assembler(char f_in_name[])
{
    //TO DO
    return 0;
}
