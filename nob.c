/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>

/* I use nob.h build system by Alexey Kutepov (See README.md file for info) */
#define NOB_IMPLEMENTATION
#define NOBDEF static inline
#include "thirdparty/nob/nob.h"

#define CLANG(cmd)         nob_cmd_append(cmd, "clang")

#define BUILD_FOLDER       "build/"
#define SRC_FOLDER         "src/"
#define DOCS_FOLDER        "docs/"
#define PKG_CONFIGS_FOLDER BUILD_FOLDER "pkg-configs/"
#define FLAGS_PATH         SRC_FOLDER "flags/"

#define BUILD_FLAGS(cmd)   nob_cmd_append(cmd, "@" FLAGS_PATH "build_flags.txt")
#define THIRDPARTY_INCLUDE_FLAGS(cmd) nob_cmd_append(cmd, "-Ithirdparty/")
#define COMMANDS_FLAGS(cmd, obj) \
    nob_cmd_append(cmd, "-MJ", BUILD_FOLDER obj "_commands.json")

#define LINK_GRAPHICSMAGICK(cmd) \
    nob_cmd_append(cmd, "$(GraphicsMagick-config --cflags --ldflags --libs)")
#define LINK_CHRONOS(cmd) nob_cmd_append(cmd, "-Lbuild/", "-lchronos")

bool
build_binaries(Nob_Cmd cmd)
{
    // Compile `chronos.c` =====================================================
    CLANG(&cmd);
    BUILD_FLAGS(&cmd);
    THIRDPARTY_INCLUDE_FLAGS(&cmd);

    nob_cmd_append(&cmd, "@" PKG_CONFIGS_FOLDER "graphicsmagick_includes.txt");
    nob_cmd_append(&cmd,
        "@" PKG_CONFIGS_FOLDER "graphicsmagickwand_includes.txt");
    nob_cc_inputs(&cmd, "-c", SRC_FOLDER "chronos.c");
    nob_cc_output(&cmd, BUILD_FOLDER "chronos.o");

    COMMANDS_FLAGS(&cmd, "chronos");
    if (!nob_cmd_run(&cmd)) return 1;

    // Archive `libchronos.a` library ==========================================
    nob_cmd_append(&cmd, "llvm-ar", "rcs", BUILD_FOLDER "libchronos.a",
        BUILD_FOLDER "chronos.o");
    if (!nob_cmd_run(&cmd)) return 1;

    // Compile `transmitter.c` =================================================
    CLANG(&cmd);
    BUILD_FLAGS(&cmd);
    THIRDPARTY_INCLUDE_FLAGS(&cmd);

    nob_cmd_append(&cmd, "@" PKG_CONFIGS_FOLDER "graphicsmagick_includes.txt");
    nob_cmd_append(&cmd,
        "@" PKG_CONFIGS_FOLDER "graphicsmagickwand_includes.txt");
    nob_cc_inputs(&cmd, "-c", SRC_FOLDER "transmitter.c");
    nob_cc_output(&cmd, BUILD_FOLDER "transmitter.o");
    COMMANDS_FLAGS(&cmd, "transmitter");
    if (!nob_cmd_run(&cmd)) return 1;

    // Linking into `transmitter` ==============================================
    CLANG(&cmd);
    nob_cmd_append(&cmd, "-fuse-ld=lld", "-fsanitize=address");
    nob_cmd_append(&cmd, "-L/opt/homebrew/Cellar/libtool/2.5.4/lib/");
    nob_cmd_append(&cmd, "@build/pkg-configs/graphicsmagick_libs.txt");
    nob_cmd_append(&cmd, "@build/pkg-configs/graphicsmagickwand_libs.txt");
    nob_cc_inputs(&cmd, BUILD_FOLDER "transmitter.o");
    nob_cc_output(&cmd, BUILD_FOLDER "transmitter");
    LINK_CHRONOS(&cmd);
    if (!nob_cmd_run(&cmd)) return 1;

    return 1;
}

bool
run_clang_format(Nob_Cmd cmd)
{
    char *files[] = { "nob.c", SRC_FOLDER "canonical_huffman.h",
        SRC_FOLDER "chronos.c", SRC_FOLDER "chronos.h", SRC_FOLDER "crc32c.h",
        SRC_FOLDER "receiver.c", SRC_FOLDER "reed_solomon.h",
        SRC_FOLDER "transmitter.c", BUILD_FOLDER "compile_commands.json",
        BUILD_FOLDER "chronos_commands.json",
        BUILD_FOLDER "transmitter_commands.json" };
    for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        nob_cmd_append(&cmd, "clang-format", "-i", files[i]);
        if (!nob_cmd_run(&cmd)) return 1;
    }
    return 1;
}

bool
align_compilation_database(Nob_Cmd cmd)
{
    nob_cmd_append(&cmd, "sh", "-c",
        "sed \"\\$s/,\\$//\" build/*.json > build/compile_commands_temp.json");
    if (!nob_cmd_run(&cmd)) return 1;

    nob_cmd_append(&cmd, "sh", "-c",
        "(echo \"[\"; cat build/compile_commands_temp.json; echo \"]\") > build/compile_commands.json");
    if (!nob_cmd_run(&cmd)) return 1;

    return 1;
}

bool
pkg_config_all(Nob_Cmd cmd)
{
    if (!nob_mkdir_if_not_exists(PKG_CONFIGS_FOLDER)) return 1;

    nob_cmd_append(&cmd, "sh", "-c",
        "GraphicsMagick-config --cppflags > " PKG_CONFIGS_FOLDER
        "graphicsmagick_includes.txt");
    if (!nob_cmd_run(&cmd)) return 1;

    nob_cmd_append(&cmd, "sh", "-c",
        "GraphicsMagick-config --ldflags --libs > " PKG_CONFIGS_FOLDER
        "graphicsmagick_libs.txt");
    if (!nob_cmd_run(&cmd)) return 1;

    nob_cmd_append(&cmd, "sh", "-c",
        "GraphicsMagickWand-config --cppflags > " PKG_CONFIGS_FOLDER
        "graphicsmagickwand_includes.txt");
    if (!nob_cmd_run(&cmd)) return 1;

    nob_cmd_append(&cmd, "sh", "-c",
        "GraphicsMagickWand-config --ldflags --libs > " PKG_CONFIGS_FOLDER
        "graphicsmagickwand_libs.txt");
    if (!nob_cmd_run(&cmd)) return 1;

    return 1;
}

bool
build_docs_normal(Nob_Cmd cmd, const char *source_files[], size_t count)
{
    nob_cmd_append(&cmd, "clang-doc", "--format=html", "--output=docs/",
        "--doxygen", "--project-name=CHRONOS-LoRa");

    for (size_t i = 0; i < count; i++) {
        nob_cmd_append(&cmd, source_files[i]);
    }
    if (!nob_cmd_run(&cmd)) return 1;

    return 1;
}

int
main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = { 0 };
    const char *source_files[] = { SRC_FOLDER "chronos.c",
        SRC_FOLDER "transmitter.c", SRC_FOLDER "receiver.c" };
    size_t count = 3;

    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    nob_cmd_append(&cmd, "sh", "-c",
        "find build -type f -name \"*.json\" -delete");
    if (!nob_cmd_run(&cmd)) return 1;

    if (!pkg_config_all(cmd)) return 1;

    if (!nob_mkdir_if_not_exists(DOCS_FOLDER)) return 1;

    if (!build_binaries(cmd)) return 1;
    if (!align_compilation_database(cmd)) return 1;
    if (!run_clang_format(cmd)) return 1;
    if (!build_docs_normal(cmd, source_files, count)) return 1;

    return 0;
}
