#include "support/io.h"

static uint8_t is_num(char *str)
{   
    uint8_t result = 0;

    if (strncmp(str, "0b", 2) == 0)
    {
        result |= 0b10;
        str += 2;
    }
    else if (strncmp(str, "0x", 2) == 0)
    {
        result |= 0b100;
        str += 2;
    }

    for (; str[0] != '\0'; str++)
    {
        if (!(str[0] >= '0' && str[0] <= '9'))
        {
            return 0;
        }
    }

    return result;
}

unsigned long str2bn(const char *str)
{
    if (strncmp(str, "0b", 2) != 0)
    {
        return -1;
    }
    str += 2;

    unsigned long power = strlen(str) - 1;
    unsigned char result = 0;

    for (unsigned long i = 0; power != -1; power--, i++)
        result += ((str[i] - '0') << power);
    
    return result;
}

char *read_file(const char *filename)
{
    long size;
    char *buffer;
    unsigned long i = 0UL;
    FILE *file = fopen(filename, "r");

    fseek(file, 0L, SEEK_END);
    size = ftell(file) + 1;

    fseek(file, 0L, SEEK_SET);
    buffer = (char *)malloc(size);
    
    for (int ch = fgetc(file); ch != EOF; ch = fgetc(file))
        buffer[i++] = (char) ch;

    buffer[i] = '\0';
    
    fclose(file);
    return buffer;
}

char **load_config(
    char *filename,
    char **fields, 
    int amount_of_fields
)
{
    size_t len;
    char *cur_line, *support_line, *support_line2;
    char *buffer = read_file(filename);
    char **filled_lines = (char**) malloc(
        sizeof(char*) * amount_of_fields
    );
    int *lines = (int*) malloc(sizeof(int) * amount_of_fields);
    cur_line = buffer;

    for (int i = 0; i < amount_of_fields; i++)
    {
        len = strlen(fields[i]);

        if (cur_line[0] == '#')
        {
            if (cur_line = strchr(cur_line, '\n') == NULL)
                return NULL;

            if (cur_line == NULL)
                break;

            continue;
        }

        for (int j = 0; j < amount_of_fields; j++)
        {
            if (strncmp(cur_line, fields[j], len) == 0)
            {
                support_line2 = strchr(cur_line, '\n');
                support_line = strchr(cur_line, ':')  + 1;

                if (support_line == NULL || support_line2 == NULL)
                    return NULL;

                for (; support_line[0] == ' '; support_line++);

                if (support_line == support_line2)
                {
                    filled_lines[j] = NULL;
                }
                else{
                    filled_lines[j] = (char*) malloc(support_line2 - support_line + 1);
                    *support_line2 = '\0';
                    strcpy(filled_lines[j], support_line);
                }
                
                break;
            }
        }

        cur_line = support_line2 + 1;
        if (cur_line[0] == '\0')
        {
            break;
        }
    }

    free(lines);
    free(buffer);
    return filled_lines;
  
}