#include "client/client.hpp"

int main(int argc, char *argv[])
{
    cli::client client("127.0.0.1", 27778);
    printf("%d\n", client.connect());
}