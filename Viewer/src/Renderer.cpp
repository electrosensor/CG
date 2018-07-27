#include "Renderer.h"
#include "InitShader.h"
#include <imgui/imgui.h>
#include "Util.h"
#include "Defs.h"

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)

Renderer::Renderer() : width(DEFAULT_WIDTH), height(DEFAULT_HEIGHT)
{
	initOpenGLRendering();
	createBuffers(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

Renderer::Renderer(int w, int h) : width(w), height(h)
{
	initOpenGLRendering();
	createBuffers(w,h);
}

Renderer::~Renderer()
{
	delete[] colorBuffer;
}

void Renderer::DrawTriangles(const vector<glm::vec3>* vertices, const vector<glm::vec3>* normals)
{
	glm::vec3 color(WHITE_COLOR);
	vector<glm::vec3>::const_iterator it = vertices->begin();
	while (it != vertices->end())
	{
		glm::vec3 p1 = *(it++);
		glm::vec3 p2 = *(it++);
		glm::vec3 p3 = *(it++);

		p1 = Util::toNormalForm(m_cameraProjection * m_cameraTransform * Util::toHomogenicForm(p1));
		p2 = Util::toNormalForm(m_cameraProjection * m_cameraTransform * Util::toHomogenicForm(p2));
		p3 = Util::toNormalForm(m_cameraProjection * m_cameraTransform * Util::toHomogenicForm(p3));

		DrawLine(glm::vec2(p1.x, p1.y), glm::vec2(p2.x, p2.y), color);
		DrawLine(glm::vec2(p2.x, p2.y), glm::vec2(p3.x, p3.y), color);
		DrawLine(glm::vec2(p3.x, p3.y), glm::vec2(p1.x, p1.y), color);
	}
}

void Renderer::SetCameraTransform(const glm::mat4x4 & cTransform)
{
	m_cameraTransform = cTransform;
}

void Renderer::SetProjection(const glm::mat4x4 & projection)
{
	m_cameraProjection = projection;
}

void Renderer::SetObjectMatrices(const glm::mat4x4 & oTransform, const glm::mat4x4 & nTransform)
{
	m_objectTransform = oTransform;
	m_normalTransform = nTransform;
}

void Renderer::putPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= width) return;
	if (j < 0) return; if (j >= height) return;
	colorBuffer[INDEX(width, i, j, 0)] = color.x;
	colorBuffer[INDEX(width, i, j, 1)] = color.y;
	colorBuffer[INDEX(width, i, j, 2)] = color.z;
}

void Renderer::DrawLine(const glm::vec2 & p1, const glm::vec2 & p2, const glm::vec3& color)
{
	float x1 = p1.x;
	float x2 = p2.x;
	float y1 = p1.y;
	float y2 = p2.y;
	// taken from internet:
		// Bresenham's line algorithm
		const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
		if (steep)
		{
			std::swap(x1, y1);
			std::swap(x2, y2);
		}

		if (x1 > x2)
		{
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		const float dx = x2 - x1;
		const float dy = fabs(y2 - y1);

		float error = dx / 2.0f;
		const int ystep = (y1 < y2) ? 1 : -1;
		int y = (int)y1;

		const int maxX = (int)x2;

		for (int x = (int)x1; x<maxX; x++)
		{
			if (steep)
			{
				putPixel(y, x, color);
			}
			else
			{
				putPixel(x, y, color);
			}

			error -= dy;
			if (error < 0)
			{
				y += ystep;
				error += dx;
			}
		}
}

void Renderer::createBuffers(int w, int h)
{
	createOpenGLBuffer(); //Do not remove this line.
	colorBuffer = new float[3*w*h];
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			putPixel(i, j, glm::vec3(0.0f, 0.0f, 0.0f));
		}
	}
}

void Renderer::SetDemoBuffer()
{
	int r = 5;
	// Wide red vertical line
	glm::vec4 red = glm::vec4(1, 0, 0, 1);
	for (int i = 0; i<height; i++)
	{
		for (int r0 = 0; r0 < r; r0++)
		{
			putPixel((width / 2) + r0, i, red);
			putPixel((width / 2) - r0, i, red);
		}
	}
	// Wide magenta horizontal line
	glm::vec4 magenta = glm::vec4(1, 0, 1, 1);
	for (int i = 0; i<width; i++)
	{
		for (int r0 = 0; r0 < r; r0++)
		{
			putPixel(i, (height / 2) + r0, magenta);
			putPixel(i, (height / 2) - r0, magenta);
		}

	}
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::initOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &glScreenTex);
	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &glScreenVtc);
	GLuint buffer;
	// Makes this VAO the current one.
	glBindVertexArray(glScreenVtc);
	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);
	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};
	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};
	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);
	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);
	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	// Make this program the current one.
	glUseProgram( program );
	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );
	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::createOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);
	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);
	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, width, height);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);
	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);
	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, colorBuffer);
	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);
	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);
	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			putPixel(i, j, color);
		}
	}
}

void Renderer::Viewport(int w, int h)
{
	if (w == width && h == height)
	{
		return;
	}
	width = w;
	height = h;
	delete[] colorBuffer;
	colorBuffer = new float[3 * h*w];
	createOpenGLBuffer();
}
