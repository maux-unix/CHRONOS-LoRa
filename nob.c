/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* I use nob.h build system by Alexey Kutepov (See README.md file for info) */
#define NOB_IMPLEMENTATION
#define NOBDEF static inline
#include "thirdparty/nob/nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER   "src/"
#define DOCS_FOLDER  "docs/"
#define TEST         "'' '$ s/,$//'"

#define BUILD_FLAGS(cmd)                                              \
    nob_cmd_append(cmd, "-x", "c", "-g3", "-Wall", "-Wextra",         \
        "-Wno-unused-parameter", "-Wno-unused-function",              \
        "-Wno-sign-conversion", "-fstack-protector-strong",           \
        "-fsanitize=undefined", "-fsanitize-trap", "-O2", "-std=c23", \
        "-pedantic")

#define TRANSMITTER_INCLUDE_FLAGS(cmd) nob_cmd_append(cmd, "-Ithirdparty/")

#define COMMANDS_FLAGS(cmd, obj) \
    nob_cmd_append(cmd, "-MJ", BUILD_FOLDER obj "_commands.json")

bool
build_binaries(Nob_Cmd cmd)
{
    nob_cc(&cmd);
    BUILD_FLAGS(&cmd);
    nob_cc_inputs(&cmd, "-c", SRC_FOLDER "chronos.c");
    nob_cc_output(&cmd, BUILD_FOLDER "chronos.o");
    COMMANDS_FLAGS(&cmd, "chronos");
    if (!nob_cmd_run(&cmd)) return 1;

    nob_cc(&cmd);
    BUILD_FLAGS(&cmd);
    TRANSMITTER_INCLUDE_FLAGS(&cmd);
    nob_cc_inputs(&cmd, SRC_FOLDER "transmitter.c");
    nob_cc_output(&cmd, BUILD_FOLDER "transmitter");
    COMMANDS_FLAGS(&cmd, "transmitter");
    if (!nob_cmd_run(&cmd)) return 1;

    nob_cmd_append(&cmd, "llvm-ar", "rcs", BUILD_FOLDER "libchronos.a",
        BUILD_FOLDER "chronos.o");
    if (!nob_cmd_run(&cmd)) return 1;

    // nob_cmd_append(&cmd, "sed", "-i" , "''", "'$",
    // "s/,$//'",BUILD_FOLDER"*.json"); if (!nob_cmd_run(&cmd)) return 1;
    return 1;
}

bool
build_docs(Nob_Cmd cmd)
{
    return 1;
}

int
main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;
    if (!nob_mkdir_if_not_exists(DOCS_FOLDER)) return 1;

    Nob_Cmd cmd = { 0 };
    if (!build_binaries(cmd)) return 1;
    if (!build_docs(cmd)) return 1;

    return 0;
}
