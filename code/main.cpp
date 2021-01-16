#include "gclient.hpp"

int main(int argc, char *argv[])
{
   
    gclient client("192.168.1.13", 27778, "vaskozlov", "212017", "212017");
    
    char *buffer1 = (char *)alloca(4096);
    char *buffer2 = (char *)alloca(4096);

    while (true)
    {
        fgets(buffer1, 4096, stdin);

        if (strncmp(buffer1, "exit\n", 5) == 0)
        {
            break;
        }
        else if (strncmp(buffer1, "clear\n", 6) == 0)
        {
            system("clear");
        }
        else if (strncmp(buffer1, "send\n", 5) == 0)
        {
            printf("Enter file to send: ");
            fgets(buffer1, 4096, stdin);
            
            char *filename = strrchr(buffer1, '/');
            *strrchr(buffer1, '\n') = '\0';

            if (filename == NULL)
                filename = buffer1;
            else
                filename++;

            client.send_file(buffer1, filename);
        }
        else if (strncmp(buffer1, "download\n", 9) == 0)
        {
            printf("Enter file to download: ");
            fgets(buffer1, 4096, stdin);

            char *filename = strrchr(buffer1, '/');
            *strrchr(buffer1, '\n') = '\0';

            if (filename == NULL)
                filename = buffer1;
            else
                filename++;
            
            client.download(buffer1, filename);
        }

        else if (strncmp(buffer1, "cli cd ",  6) == 0)
        {
            *strrchr(buffer1, '\n') = '\0';
            chdir(buffer1 + 7);
        }
        else if (strncmp(buffer1, "cli ", 4) == 0)
        {
            system(buffer1 + 4);
        }

        else if (strncmp(buffer1, "cd ", 3) == 0)
        {
            char *local_buffer = buffer1 + 3;

            for (; *local_buffer == ' '; local_buffer++);
            *strrchr(buffer1, '\n') = '\0';

            client.change_dir(local_buffer);
        }
        else
        {
            auto data = client.proccess_command(buffer1);
            printf("%s", (char *)data.buffer);

            free(data.buffer);
        }
    }

    return EXIT_SUCCESS;
}
