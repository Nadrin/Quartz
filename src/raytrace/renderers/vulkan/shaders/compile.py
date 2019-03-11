#!/usr/bin/env python3

# Copyright (C) 2018-2019 Michał Siejak
# This file is part of Quartz - a raytracing aspect for Qt3D.
# See LICENSE file for licensing information

# Compiles GLSL shaders to SPIR-V and generates appropriate QRC file.

import os, sys, shutil
import glob
import subprocess

def find_glslang():
    GLSLANG_NAME = "glslangValidator"
    GLSLANG_FALLBACK_PATH = "/opt/glslang/bin"

    # Try standard PATH.
    glslang = shutil.which(GLSLANG_NAME)

    # Try VULKAN_SDK bin directory.
    if glslang is None:
        vulkan_sdk = os.environ.get("VULKAN_SDK")
        if vulkan_sdk is not None:
            vulkan_sdk_bin = os.path.join(vulkan_sdk, "bin")
            glslang = shutil.which(GLSLANG_NAME, path=vulkan_sdk_bin)

    # Try fallback path on POSIX systems.
    if glslang is None and os.name == "posix":
        glslang = shutil.which(GLSLANG_NAME, path=GLSLANG_FALLBACK_PATH)

    return glslang

def write_qrc_file(filename, shader_names):
    with open(filename, 'w') as f:
        print("<!DOCTYPE RCC>\n<RCC version=\"1.0\">\n<qresource>", file=f)
        for shader_name in shader_names:
            print("  <file>spv/{0}.spv</file>".format(shader_name), file=f)
        print("</qresource>\n</RCC>", file=f)

def compile_shaders(base_path, glslang):
    output_path   = os.path.join(base_path, "spv")
    qrc_file_path = os.path.join(base_path, "vulkan_shaders.qrc")

    try:
        os.mkdir(output_path)
    except FileExistsError:
        pass

    shader_names = []
    for glsl_path in glob.glob(os.path.join(base_path, "*.glsl")):
        shader_name = os.path.splitext(os.path.basename(glsl_path))[0]
        spv_path = os.path.join(output_path, "{0}.spv".format(shader_name))
        subprocess.run([glslang, "-V", glsl_path, "-o", spv_path])
        shader_names.append(shader_name)

    write_qrc_file(qrc_file_path, shader_names)

def main():
    glslang = find_glslang()
    if glslang is None:
        print("Error: Unable to find glslangValidator. Make sure it's either in PATH or that VULKAN_SDK is defined and points to the right directory.", file=sys.stderr)
        sys.exit(1)

    print("Using GLSL compiler: {0}".format(glslang))
    base_path = os.path.dirname(os.path.realpath(__file__))
    compile_shaders(base_path, glslang)

if __name__ == "__main__":
    main()
