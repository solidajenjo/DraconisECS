#pragma once
#include <GL/glew.h>

namespace module
{
class Framebuffer
{
public:
	Framebuffer( int width, int height );
	~Framebuffer();

	void bind() const;
	void unbind() const;
	void resize( int width, int height );
	GLuint getTextureID() const
	{
		return m_TextureID;
	}
	GLuint getFramebufferID() const
	{
		return m_FramebufferID;
	}
	bool isComplete() const;

private:
	void create();
	void cleanup();

	GLuint m_FramebufferID;
	GLuint m_TextureID;
	GLuint m_DepthStencilRBO;
	int m_Width;
	int m_Height;
	mutable GLint m_PreviousViewport[4];
	mutable GLint m_PreviousFramebuffer;
};
}  // namespace module