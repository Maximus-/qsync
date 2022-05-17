#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "auth.h"
#include "files.h"

#include <msquic.hpp>

#define QSYNC_ALPN "qsync"

#ifndef WIN32
 #define UNREFERENCED_PARAMETER(x) (void)x
#endif

#define MASSERT(x) do { if (!(x)) { printf("Assert((%s) != true)", #x); } } while (0);

enum QsyncPerspective {
    Client = 0,
    Server = 1,
};

typedef struct _Settings {
    enum QsyncPerspective Perspective;
    struct {
        char *ServerAddress;
        uint16_t ServerPort;
    } ClientSettings;
    struct {
    
    } ServerSettings;
} QsyncSettings;

#include "auth.h"
#include "files.h"
#include "server.h"
