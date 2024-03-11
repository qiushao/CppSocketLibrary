//
// Created by qiushao on 3/11/24.
//
#include <csignal>
#include "log.h"

static void onLoad() __attribute__((constructor));
static void onUnload() __attribute__((destructor));

static void onLoad() {
    LOGD("Library is loaded");
    // Don't signal on socket write errors.
    ::signal(SIGPIPE, SIG_IGN);
}

static void onUnload() {
    LOGD("Library is unloaded");
}
