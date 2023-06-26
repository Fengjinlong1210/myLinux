#include <stdio.h>
#include <string.h>
#define FILENAME "bite"

int main()
{
    FILE* fwt = fopen(FILENAME, "w");
    if(fwt == NULL)
    {
        perror("fopen");
        return 1;
    }
    char buffer[] = {"linux so easy!\n"};
    fwrite(buffer, sizeof(char), sizeof(buffer), fwt);
    fclose(fwt);

    FILE* fwd = fopen(FILENAME, "r");
    if(fwd == NULL)
    {
        perror("fopen2");
        return 1;
    }
    char buff[1024] = {0};
    fread(buff, sizeof(char), sizeof(buff), fwd);
    printf("I read : %s", buff);
    fclose(fwd);
    return 0;
}
