//========================================================================
// Offscreen rendering example
// Copyright (c) Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if USE_NATIVE_OSMESA
 #define GLFW_EXPOSE_NATIVE_OSMESA
 #include <GLFW/glfw3native.h>
#endif

#include "linmath.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "myPng.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

float f_pos1[] = {
        -1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
};

float f_pos2[] = {
//        -0.5f, 0.5f,
//        -0.5f, -0.5f,
//		0.5f, -0.5f,
//		0.5f, 0.5f,

        -0.8f, 0.8f,
        -0.8f, -0.8f,
        0.8f, -0.8f,
        0.8f, 0.8f,
};

float f_tex1[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
};

float f_tex2[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
};

static const char* vertex_shader_text =
	"attribute vec4 a_Position1;     \n"
	"attribute vec4 a_Position2;     \n"
	"attribute vec2 a_TextureCoordinates;     \n"
	"attribute vec2 a_TextureCoordinates2;     \n"
	"uniform  mat4 u_Matrix;     \n"
	"uniform  float u_type;     \n"
	"\n"
	"varying vec2 v_TextureCoordinates;     \n"
	"varying float v_type;     \n"
	"          \n"
	"\n"
	"void main()                      \n"
	"{                                \n"
	"	 if(u_type == 2.0)\n"
	"	 {\n"
	"	 	v_TextureCoordinates = a_TextureCoordinates2;\n"
	"	 	gl_Position = u_Matrix * a_Position2;\n"
	"	 }\n"
	"	 else\n"
	"	 {\n"
	"	  	v_TextureCoordinates = a_TextureCoordinates;\n"
	"	  	gl_Position = a_Position1;\n"
	"	 }\n"
	"	 \n"
	"	 v_type = u_type;\n"
	"}";

static const char* fragment_shader_text =
//        "precision mediump float;   \n"
        "                                          \n"
        "uniform sampler2D  u_TextureUnit;                                 \n"
        "uniform sampler2D  u_TextureUnit2;                                 \n"
        "varying vec2 v_TextureCoordinates;                                 \n"
		"varying float v_type;     \n"
		"    \n"
        "void main()                           \n"
        "{                                 \n"
        "    if(v_type == 1.0){"
		"		 gl_FragColor = texture2D(u_TextureUnit, v_TextureCoordinates);"
        "	 }else{"
		"		 gl_FragColor = texture2D(u_TextureUnit2, v_TextureCoordinates);    "
        "	 }                                           \n"
//        "    gl_FragColor = texture2D(u_TextureUnit, v_TextureCoordinates);                                           \n"
        "}";


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main()
{
    GLFWwindow* window;
    GLuint vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vpos2_location, vtex_location, vtex2_location;
    GLuint texture[2];
    GLint tex1_location, tex2_location;
    GLint type_location;
    float ratio;
    int width, height;
    mat4x4 m, p, mvp;
    char* buffer;
    int i;

    glfwSetErrorCallback(error_callback);

    glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // NOTE: OpenGL error checks have been omitted for brevity

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    GLint compile_status = -20;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (GL_TRUE != compile_status) {
        GLsizei length = 0;
        GLchar infoLog[1024];
        glGetShaderInfoLog(vertex_shader,
                           1024,
                           &length,
                           infoLog);
        printf("OpenGLHelper : compile vertexShader failed : error msg = %s", infoLog);

        return 0;
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (GL_TRUE != compile_status) {
        GLsizei length = 0;
        GLchar infoLog[1024];
        glGetShaderInfoLog(vertex_shader,
                           1024,
                           &length,
                           infoLog);
        printf("OpenGLHelper : compile fragment_shader failed : error msg = %s", infoLog);

        return 0;
    }


    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "u_Matrix");
    vpos_location = glGetAttribLocation(program, "a_Position1");
    vpos2_location = glGetAttribLocation(program, "a_Position2");
    vtex_location = glGetAttribLocation(program, "a_TextureCoordinates");
    vtex2_location = glGetAttribLocation(program, "a_TextureCoordinates2");
    tex1_location = glGetUniformLocation(program, "u_TextureUnit");
    tex2_location = glGetUniformLocation(program, "u_TextureUnit2");
    type_location = glGetUniformLocation(program, "u_type");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          0, f_pos1);

    glEnableVertexAttribArray(vpos2_location);
    glVertexAttribPointer(vpos2_location, 2, GL_FLOAT, GL_FALSE,
                          0, f_pos2);

    glEnableVertexAttribArray(vtex_location);
    glVertexAttribPointer(vtex_location, 2, GL_FLOAT, GL_FALSE,
                          0, f_tex1);

    glEnableVertexAttribArray(vtex2_location);
    glVertexAttribPointer(vtex2_location, 2, GL_FLOAT, GL_FALSE,
                          0, f_tex2);

    PNGHandle* handles[2];
    PNGHandle* handle = mallocPngHandle();
    PNGHandle* handle1 = mallocPngHandle();
    decodePng("mu.png", handle);
//    decodePng("hiei2.png", handle1);
    decodePng("11.png", handle1);

    handles[0] = handle;
    handles[1] = handle1;

    for(i = 0; i < sizeof(texture)/sizeof(GLuint); i++){
    	PNGHandle* handle = handles[i];
    	glGenTextures(1, texture + i);
    	// Bind to the texture in OpenGL
    	glBindTexture(GL_TEXTURE_2D, texture[i]);
    	// Set filtering
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    	// Load the bitmap into the bound texture.
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, handle->width, handle->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, handle->data);
    }


    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4x4_identity(m);
    mat4x4_rotate_Z(m, m, 1*M_PI/90);
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
    glUniform1i(tex1_location, 0);
    glUniform1f(type_location, 1.0f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
    glUniform1i(tex2_location, 1);
    glUniform1f(type_location, 2.0f);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    i = 0;
//    while (!glfwWindowShouldClose(window))
//    {
//        glfwGetFramebufferSize(window, &width, &height);
//        ratio = width / (float) height;
//
//        glViewport(0, 0, width, height);
//        glClear(GL_COLOR_BUFFER_BIT);
//        glEnable(GL_MULTISAMPLE);
//
//        mat4x4_identity(m);
//        mat4x4_rotate_Z(m, m, i++*M_PI/1080);
//        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
//        mat4x4_mul(mvp, p, m);
//
//        glUseProgram(program);
//        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
//        glActiveTexture(GL_TEXTURE0);
//    	glBindTexture(GL_TEXTURE_2D, texture[0]);
//        glUniform1i(tex1_location, 0);
//        glUniform1f(type_location, 1.0f);
//        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//
//    	glActiveTexture(GL_TEXTURE1);
//    	glBindTexture(GL_TEXTURE_2D, texture[1]);
//        glUniform1i(tex2_location, 1);
//        glUniform1f(type_location, 2.0f);
//
//        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//        glDisable(GL_MULTISAMPLE);
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }


#if USE_NATIVE_OSMESA
    glfwGetOSMesaColorBuffer(window, &width, &height, NULL, (void**) &buffer);
#else
    buffer = calloc(4, width * height);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
#endif

    // Write image Y-flipped because OpenGL
    stbi_write_png("offscreen.png",
                   width, height, 4,
                   buffer + (width * 4 * (height - 1)),
                   -width * 4);

#if USE_NATIVE_OSMESA
    // Here is where there's nothing
#else
    free(buffer);
#endif

    freePngHandle(handle);
    freePngHandle(handle1);
    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

