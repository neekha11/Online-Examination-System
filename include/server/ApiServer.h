#ifndef APISERVER_H
#define APISERVER_H

#include <string>

// Starts the HTTP server: JSON API under /api/... plus the static frontend.
// Blocks until the server stops. Returns a process exit code.
int runApiServer(int port, const std::string& webRoot);

#endif
