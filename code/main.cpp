#include "client/client.hpp"

int main(int argc, char *argv[])
{
    cli::client client("127.0.0.1", 27777);
    printf("%d\n", client.connect());

    client.send_status(0xFFFE);
    printf("%d\n", client.login("vaskozlov", "212017"));

}
