#version 460
/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

layout(location=0) out vec2 uv;

void main()
{
    if(gl_VertexIndex == 0) {
        uv = vec2(1.0, 1.0);
        gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
    }
    else if(gl_VertexIndex == 1) {
        uv = vec2(1.0, -1.0);
        gl_Position = vec4(1.0, -3.0, 0.0, 1.0);
    }
    else /* if(gl_VertexIndex == 2) */ {
        uv = vec2(-1.0, 1.0);
        gl_Position = vec4(-3.0, 1.0, 0.0, 1.0);
    }
}
