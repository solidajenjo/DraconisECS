#include "framebuffer.h"
#include <iostream>

namespace module
{
Framebuffer::Framebuffer( int width, int height )
	: m_Width( width ), m_Height( height ), m_FramebufferID( 0 ), m_TextureID( 0 ), m_DepthStencilRBO( 0 )
{
	create();
}

Framebuffer::~Framebuffer()
{
	cleanup();
}

void Framebuffer::create()
{
	// Generate and bind framebuffer
	glGenFramebuffers( 1, &m_FramebufferID );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FramebufferID );

	// Create color texture attachment
	glGenTextures( 1, &m_TextureID );
	glBindTexture( GL_TEXTURE_2D, m_TextureID );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0 );

	// Create depth and stencil renderbuffer attachment
	glGenRenderbuffers( 1, &m_DepthStencilRBO );
	glBindRenderbuffer( GL_RENDERBUFFER, m_DepthStencilRBO );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRBO );

	if( !isComplete() )
	{
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}

	// Unbind framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void Framebuffer::cleanup()
{
	if( m_FramebufferID != 0 )
	{
		glDeleteFramebuffers( 1, &m_FramebufferID );
		m_FramebufferID = 0;
	}
	if( m_TextureID != 0 )
	{
		glDeleteTextures( 1, &m_TextureID );
		m_TextureID = 0;
	}
	if( m_DepthStencilRBO != 0 )
	{
		glDeleteRenderbuffers( 1, &m_DepthStencilRBO );
		m_DepthStencilRBO = 0;
	}
}

void Framebuffer::bind() const
{
	glBindFramebuffer( GL_FRAMEBUFFER, m_FramebufferID );
	glViewport( 0, 0, m_Width, m_Height );
}

void Framebuffer::unbind() const
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void Framebuffer::resize( int width, int height )
{
	if( width == m_Width && height == m_Height )
		return;

	m_Width	 = width;
	m_Height = height;

	cleanup();
	create();
}

bool Framebuffer::isComplete() const
{
	glBindFramebuffer( GL_FRAMEBUFFER, m_FramebufferID );
	bool complete = glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE;
	return complete;
}
}  // namespace module