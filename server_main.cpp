#include "include/server/ApiServer.h"
#include <cstdlib>

// Run from the project root so data/ and frontend/ resolve:  ./server
int main() {
    const char* port = std::getenv("PORT");          // set by hosting platforms
    return runApiServer(port ? std::atoi(port) : 8080, "frontend");
}
