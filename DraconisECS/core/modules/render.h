#pragma once
#include <GL/glew.h>
#include <memory>
#include "Module.h"
#include "framebuffer.h"

namespace module
{
class Render : public module::Module
{
public:
	Render();
	~Render();

	bool init();
	bool preUpdate() override;
	bool update() override;
	bool shutdown() override;

	// Framebuffer management
	void setActiveFramebuffer( Framebuffer* framebuffer );
	Framebuffer* getDefaultFramebuffer() const
	{
		return m_DefaultFramebuffer.get();
	}
	void resetToDefaultFramebuffer();

private:
	bool initShaders();
	bool initBuffers();
	void cleanupBuffers();

	std::unique_ptr<Framebuffer> m_DefaultFramebuffer;
	Framebuffer* m_ActiveFramebuffer = nullptr;

	// OpenGL objects
	GLuint m_ShaderProgram;
	GLuint m_VAO;
	GLuint m_VBO;

	// Basic vertex and fragment shader source
	const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        out vec3 vertexColor;
        
        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
            vertexColor = aColor;
        }
    )";

	const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 vertexColor;
        out vec4 FragColor;
        
        void main()
        {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";
};
}  // namespace module