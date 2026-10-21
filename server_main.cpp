#include "include/server/ApiServer.h"

// Run from the project root so data/ and frontend/ resolve:  ./server
int main() {
    return runApiServer(8080, "frontend");
}
