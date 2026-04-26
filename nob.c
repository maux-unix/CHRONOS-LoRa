/*
 * Copyright (c) 2026 Maulana M. Ali
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>

#define NOB_IMPLEMENTATION
#define NOB_WARN_DEPRECATED
#define NOB_EXPERIMENTAL_DELETE_OLD
#define NOBDEF static inline
#include "thirdparty/nob/nob.h"

/* CLI Tools' Aliases */
#define SHELL(cmd, ...)   nob_cmd_append(cmd, "sh", "-c", __VA_ARGS__)
#define CLANG(cmd, ...)   nob_cmd_append(cmd, "clang-20", "-xc", "-std=c23")
#define CLANGPP(cmd)      nob_cmd_append(cmd, "clang++-20", "-xc++", "-std=c++23")
#define COMPILE(cmd, ...) nob_cc_inputs(cmd, "-c", "-fPIC", __VA_ARGS__)
#define INPUTS(cmd, ...)  nob_cc_inputs(cmd, __VA_ARGS__)
#define OUTPUT(cmd, ...)  nob_cc_output(cmd, __VA_ARGS__)
#define APPEND(cmd, ...)  nob_cmd_append(cmd, __VA_ARGS__)
#define STATIC_LIB(cmd)   nob_cmd_append(cmd, "llvm-ar-20", "rcs")
#define SHARED_LIB(cmd)   nob_cmd_append(cmd, "clang-20", "-shared")
#define CLANG_FORMAT(cmd, ...) \
    nob_cmd_append(cmd, "clang-format-20", __VA_ARGS__)

/* Folders' PATH */
#define BUILD_FOLDER       "build/"
#define SRC_FOLDER         "src/"
#define DOCS_FOLDER        "docs/"
#define FLAGS_PATH         "flags/"
#define PKG_CONFIGS_FOLDER BUILD_FOLDER "pkg-configs/"
#define BIN_FOLDER         BUILD_FOLDER "bin/"
#define LIB_FOLDER         BUILD_FOLDER "lib/"

/* Flags */
#define BUILD_FLAGS(cmd) \
    nob_cmd_append(cmd, "@" FLAGS_PATH "dev_compile_flags.txt")
#define COMMANDS_FLAGS(cmd, obj) \
    nob_cmd_append(cmd, "-MJ", BUILD_FOLDER obj "_commands.json")

/* Includes */
#define INCLUDE_THIRDPARTY(cmd) nob_cmd_append(cmd, "-Ithirdparty/")
#define INCLUDE_GRAPHICSMAGICK(cmd)                                           \
    nob_cmd_append(cmd, "@" PKG_CONFIGS_FOLDER "graphicsmagick_includes.txt", \
        "@" PKG_CONFIGS_FOLDER "graphicsmagickwand_includes.txt")
#define INCLUDE_LIBCORRECT(cmd) \
    nob_cmd_append(cmd, "-Ithirdparty/libcorrect/include/")

/* Linking */
#define LINK_CHRONOS(cmd) \
    nob_cmd_append(cmd, "-L" LIB_FOLDER, "-l:libchronos.a")
#define LINK_GRAPHICSMAGICK(cmd)                                          \
    nob_cmd_append(cmd, "@" PKG_CONFIGS_FOLDER "graphicsmagick_libs.txt", \
        "@" PKG_CONFIGS_FOLDER "graphicsmagickwand_libs.txt")
#define LINK_LIBCORRECT(cmd) \
    nob_cmd_append(cmd, "-Lthirdparty/libcorrect/lib/", "-lcorrect")

static bool
build_binaries(Nob_Cmd cmd)
{
    /* (1) Compile `chronos.c` */
    if (nob_needs_rebuild1(BUILD_FOLDER "chronos.o", SRC_FOLDER "chronos.c")) {
        CLANG(&cmd);
        BUILD_FLAGS(&cmd);
        APPEND(&cmd, "-march=armv8-a+crc");
        INCLUDE_THIRDPARTY(&cmd);
        INCLUDE_GRAPHICSMAGICK(&cmd);
        COMPILE(&cmd, SRC_FOLDER "chronos.c");
        OUTPUT(&cmd, BUILD_FOLDER "chronos.o");
        COMMANDS_FLAGS(&cmd, "chronos");
        if (!nob_cmd_run(&cmd)) return 1;
    }

    /* (2) Making `libchronos.a` and `libchronos.so` */
    if (nob_needs_rebuild1(LIB_FOLDER "libchronos.a",
            BUILD_FOLDER "chronos.o")) {
        STATIC_LIB(&cmd);
        APPEND(&cmd, LIB_FOLDER "libchronos.a");
        INPUTS(&cmd, BUILD_FOLDER "chronos.o");
        if (!nob_cmd_run(&cmd)) return 1;
    }

    if (nob_needs_rebuild1(LIB_FOLDER "libchronos.so",
            BUILD_FOLDER "chronos.o")) {
        SHARED_LIB(&cmd);
        INPUTS(&cmd, BUILD_FOLDER "chronos.o");
        OUTPUT(&cmd, LIB_FOLDER "libchronos.so");
        LINK_GRAPHICSMAGICK(&cmd);
        if (!nob_cmd_run(&cmd)) return 1;
    }

    /* (3a) Compile `transmitter.c` */
    if (nob_needs_rebuild1(BUILD_FOLDER "transmitter.o",
            SRC_FOLDER "transmitter.c")) {
        CLANG(&cmd);
        BUILD_FLAGS(&cmd);
        APPEND(&cmd, "-march=armv8-a+crc");
        INCLUDE_THIRDPARTY(&cmd);
        INCLUDE_GRAPHICSMAGICK(&cmd);
        COMPILE(&cmd, SRC_FOLDER "transmitter.c");
        OUTPUT(&cmd, BUILD_FOLDER "transmitter.o");
        COMMANDS_FLAGS(&cmd, "transmitter");
        if (!nob_cmd_run(&cmd)) return 1;
    }

    /* (3b) Compile `transmitter-normal.cpp` */
    if (nob_needs_rebuild1(BUILD_FOLDER "transmitter-normal.o",
            SRC_FOLDER "transmitter-normal.cpp")) {
        CLANGPP(&cmd);
        BUILD_FLAGS(&cmd);
        APPEND(&cmd, "-march=armv8-a+crc");
        INCLUDE_GRAPHICSMAGICK(&cmd);
        APPEND(&cmd, "-I/usr/local/include/RadioLib");
        COMPILE(&cmd, SRC_FOLDER "transmitter-normal.cpp");
        OUTPUT(&cmd, BUILD_FOLDER "transmitter-normal.o");
        COMMANDS_FLAGS(&cmd, "transmitter-normal");
        if (!nob_cmd_run(&cmd)) return 1;
    }

    /* (3c) Compile `receiver-normal.cpp` */
    if (nob_needs_rebuild1(BUILD_FOLDER "receiver-normal.o",
            SRC_FOLDER "receiver-normal.cpp")) {
        CLANGPP(&cmd);
        BUILD_FLAGS(&cmd);
        APPEND(&cmd, "-I/usr/local/include/RadioLib");
        COMPILE(&cmd, SRC_FOLDER "receiver-normal.cpp");
        OUTPUT(&cmd, BUILD_FOLDER "receiver-normal.o");
        // APPEND(&cmd, "-lRadioLib", "-llgpio", "-lpthread", "-lchronos");
        COMMANDS_FLAGS(&cmd, "receiver-normal");
        if (!nob_cmd_run(&cmd)) return 1;
    }

    /* (3d) Compile `transmitter-chronos.cpp` */
    CLANGPP(&cmd);
    BUILD_FLAGS(&cmd);
    COMPILE(&cmd, SRC_FOLDER "transmitter-chronos.cpp");
    OUTPUT(&cmd, BUILD_FOLDER "transmitter-chronos.o");
    COMMANDS_FLAGS(&cmd, "transmitter-chronos");
    if (!nob_cmd_run(&cmd)) return 1;

    /* (4b) Linking `transmitter-normal.o` */
    APPEND(&cmd, "clang++-20", "-fuse-ld=lld");
    APPEND(&cmd, "-Wl,-rpath,'$ORIGIN/../lib'");
    OUTPUT(&cmd, BIN_FOLDER "transmitter-normal");
    INPUTS(&cmd, BUILD_FOLDER "transmitter-normal.o");
    APPEND(&cmd, "-Lbuild/lib/", "-L/usr/local/lib/");
    APPEND(&cmd, "-lRadioLib", "-llgpio", "-lpthread", "-lcorrect");
    LINK_CHRONOS(&cmd);
    LINK_GRAPHICSMAGICK(&cmd);
    if (!nob_cmd_run(&cmd)) return 1;

    /* (4d) Linking `transmitter-chronos.o` */
    APPEND(&cmd, "clang++-20", "-fuse-ld=lld");
    OUTPUT(&cmd, BIN_FOLDER "transmitter-chronos");
    INPUTS(&cmd, BUILD_FOLDER "transmitter-chronos.o");
    APPEND(&cmd, "-L/usr/local/lib/", "-lcorrect");
    // APPEND(&cmd, "-lRadioLib", "-llgpio", "-lpthread", "-lchronos");
    // LINK_GRAPHICSMAGICK(&cmd);
    if (!nob_cmd_run(&cmd)) return 1;

    return 1;
}

static bool
run_clang_format(Nob_Cmd cmd)
{
    char *files[] = { "nob.c", SRC_FOLDER "crc32c.h",
        SRC_FOLDER "reed_solomon.h", SRC_FOLDER "canonical_huffman.h",
        SRC_FOLDER "chronos.h", SRC_FOLDER "chronos.c",
        SRC_FOLDER "receiver-normal.cpp", SRC_FOLDER "transmitter-normal.cpp",
        BUILD_FOLDER "compile_commands.json" };

    for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        CLANG_FORMAT(&cmd, "-i", files[i]);
        if (!nob_cmd_run(&cmd)) return 1;
    }

    return 1;
}

static bool
align_compilation_database(Nob_Cmd cmd)
{
    SHELL(&cmd,
        "sed \"\\$s/,\\$//\" build/*.json > build/compile_commands_temp.json");
    if (!nob_cmd_run(&cmd)) return 1;

    SHELL(&cmd,
        "(echo \"[\"; cat build/compile_commands_temp.json; echo \"]\") > build/compile_commands.json");
    if (!nob_cmd_run(&cmd)) return 1;

    return 1;
}

static bool
pkg_config_all(Nob_Cmd cmd)
{
    SHELL(&cmd,
        "GraphicsMagick-config --cppflags > " PKG_CONFIGS_FOLDER
        "graphicsmagick_includes.txt");
    if (!nob_cmd_run(&cmd)) return 1;

    SHELL(&cmd,
        "GraphicsMagick-config --ldflags --libs > " PKG_CONFIGS_FOLDER
        "graphicsmagick_libs.txt");
    if (!nob_cmd_run(&cmd)) return 1;

    SHELL(&cmd,
        "GraphicsMagickWand-config --cppflags > " PKG_CONFIGS_FOLDER
        "graphicsmagickwand_includes.txt");
    if (!nob_cmd_run(&cmd)) return 1;

    SHELL(&cmd,
        "GraphicsMagickWand-config --ldflags --libs > " PKG_CONFIGS_FOLDER
        "graphicsmagickwand_libs.txt");
    if (!nob_cmd_run(&cmd)) return 1;

    return 1;
}

static bool
build_docs_normal(Nob_Cmd cmd)
{
    const char *inputs[] = { BUILD_FOLDER "transmitter-normal_commands.json" };
    if (nob_needs_rebuild(DOCS_FOLDER "index.html", inputs, 1)) {
        nob_cmd_append(&cmd, "clang-doc-20", "--format=html", "--output=docs/",
            "--doxygen", "--project-name=CHRONOS-LoRa", "--executor=all-TUs",
            BUILD_FOLDER "compile_commands.json");
        if (!nob_cmd_run(&cmd)) return 1;
    }

    return 1;
}

int
main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    Nob_Cmd cmd = { 0 };

    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;
    if (!nob_mkdir_if_not_exists(LIB_FOLDER)) return 1;
    if (!nob_mkdir_if_not_exists(BIN_FOLDER)) return 1;
    if (!nob_mkdir_if_not_exists(DOCS_FOLDER)) return 1;
    if (!nob_mkdir_if_not_exists(PKG_CONFIGS_FOLDER)) return 1;

    SHELL(&cmd, "rm -f " BUILD_FOLDER "compile_commands*.json");
    if (!nob_cmd_run(&cmd)) return 1;

    if (!pkg_config_all(cmd)) return 1;
    if (!build_binaries(cmd)) return 1;
    if (!align_compilation_database(cmd)) return 1;
    if (!run_clang_format(cmd)) return 1;
    if (!build_docs_normal(cmd)) return 1;
    return 0;
}
