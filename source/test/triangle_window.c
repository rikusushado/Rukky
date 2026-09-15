#include <rukky/rukky.h>

#include <stdio.h>

static const char *vertex_src =
"#version 330 core\n"
"layout(location = 0) in vec2 aPos;\n"
"void main() {\n"
"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
"}\n";

static const char *fragment_src =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(1.0, 0.3, 0.2, 1.0);\n"
"}\n";

void DrawTriangle(uptr* parameter, char action) {
    switch (action) {
    case 'i':
				parameter[0] = 0;
        parameter[1] = (uptr)calloc(6, sizeof(float));
				++parameter[0];

        float verts[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.0f,  0.5f
        };

        memcpy((float*)parameter[1], verts, sizeof(verts));
				
        glGenVertexArrays(1, (GLuint*)&parameter[2]);
        glBindVertexArray((GLuint)parameter[2]);
				
        glGenBuffers(1, (GLuint*)&parameter[3]);
        glBindBuffer(GL_ARRAY_BUFFER, (GLuint)parameter[3]);
        glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), (float*)parameter[1], GL_STATIC_DRAW);

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertex_src, NULL);
        glCompileShader(vs);
        
        GLint ok;
				glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
				printf("VS compile: %d\n", ok);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragment_src, NULL);
        glCompileShader(fs);
        
        glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
				printf("FS compile: %d\n", ok);

				
        parameter[4] = (uptr)glCreateProgram();
        glAttachShader((GLuint)parameter[4], vs);
        glAttachShader((GLuint)parameter[4], fs);
        glLinkProgram((GLuint)parameter[4]);
        
        glGetProgramiv((GLuint)parameter[4], GL_LINK_STATUS, &ok);
				printf("Program link: %d\n", ok);


        glDeleteShader(vs);
        glDeleteShader(fs);

        glUseProgram((GLuint)parameter[4]);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        break;

    case 'l':
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray((GLuint)parameter[2]);
        glUseProgram(4);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        break;

    case 'e':
        glDeleteBuffers(1, (GLuint*)&parameter[3]);
        glDeleteVertexArrays(1, (GLuint*)&parameter[1]);
        glDeleteProgram((GLuint)parameter[4]);
        
				for (uptr i = 1; i <= parameter[0]; ++i) {
						free((void*)parameter[i]);
				}
        break;
    }
}


int main(int argc, char** argv)
{
	uptr* window = NULL;
	bool success = RukkyNewWindow(
	    &window,
	    0, 0,
	    144, 144,
	    0,
	    "Rukky | Blank Window Test"
	);

	if (!success) return -1;

  uptr parameter[5];

	RukkyApplicationLoop(
		&window,
		1000,
		(uptr)&DrawTriangle,
		parameter,
    -1
	);

	return 0;
}
