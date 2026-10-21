// #include "include/server/ApiServer.h"

// // Run from the project root so data/ and frontend/ resolve:  ./server
// int main() {
//     return runApiServer(8080, "frontend");
// }

#include "include/server/ApiServer.h"
#include <cstdlib>
#include <string>

int main()
{
    const char *envPort = std::getenv("PORT");
    int port = envPort ? std::atoi(envPort) : 8080;

    return runApiServer(port, "frontend");
}
