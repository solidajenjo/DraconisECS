#include "render.h"
#include <iostream>
#include "GL/glew.h"

namespace module
{

static void GLAPIENTRY openglDebugCallback( GLenum source,
											GLenum type,
											GLuint id,
											GLenum severity,
											GLsizei length,
											const GLchar* message,
											const void* userParam )
{
	(void)length;  // Unused parameter
	(void)userParam;  // Unused parameter

	// Ignore non-significant error/warning codes
	if( severity == GL_DEBUG_SEVERITY_NOTIFICATION )
		return;

	std::cerr << "OpenGL Debug - ";

	// Print source
	std::cerr << "Source: ";
	switch( source )
	{
		case GL_DEBUG_SOURCE_API:
			std::cerr << "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			std::cerr << "Window System";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			std::cerr << "Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			std::cerr << "Third Party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			std::cerr << "Application";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			std::cerr << "Other";
			break;
		default:
			std::cerr << "Unknown";
			break;
	}

	// Print type
	std::cerr << ", Type: ";
	switch( type )
	{
		case GL_DEBUG_TYPE_ERROR:
			std::cerr << "Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cerr << "Deprecated Behavior";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cerr << "Undefined Behavior";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cerr << "Portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cerr << "Performance";
			break;
		case GL_DEBUG_TYPE_MARKER:
			std::cerr << "Marker";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			std::cerr << "Push Group";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			std::cerr << "Pop Group";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cerr << "Other";
			break;
		default:
			std::cerr << "Unknown";
			break;
	}

	std::cerr << ", ID: " << id;

	switch( severity )
	{
		case GL_DEBUG_SEVERITY_HIGH:
			std::cerr << ", Severity: HIGH";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cerr << ", Severity: MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			std::cerr << ", Severity: LOW";
			break;
		default:
			std::cerr << ", Severity: UNKNOWN";
			break;
	}

	std::cerr << "\nMessage: " << message << std::endl;
}

Render::Render()
	: m_DefaultFramebuffer( nullptr ), m_ActiveFramebuffer( nullptr ), m_ShaderProgram( 0 ), m_VAO( 0 ), m_VBO( 0 )
{
}

Render::~Render()
{
	cleanupBuffers();
}

bool Render::init()
{
	if( glewInit() != GLEW_OK )
	{
		std::cout << "Error: GLEW failed to initialize" << std::endl;
		return false;
	}

	// Enable debug output
	glEnable( GL_DEBUG_OUTPUT );
	glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
	glDebugMessageCallback( openglDebugCallback, nullptr );

	std::cout << "OpenGL Version: " << glGetString( GL_VERSION ) << std::endl;
	std::cout << "GLSL Version: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;

	// Initialize shaders
	if( !initShaders() )
	{
		std::cout << "Error: Failed to initialize shaders" << std::endl;
		return false;
	}
	std::cout << "Shaders initialized successfully" << std::endl;

	// Initialize buffers
	if( !initBuffers() )
	{
		std::cout << "Error: Failed to initialize buffers" << std::endl;
		return false;
	}
	std::cout << "Buffers initialized successfully" << std::endl;

	// Create default framebuffer (1280x720 as default size)
	m_DefaultFramebuffer = std::make_unique<Framebuffer>( 1280, 720 );
	if( !m_DefaultFramebuffer->isComplete() )
	{
		std::cout << "Error: Default framebuffer is not complete" << std::endl;
		return false;
	}
	m_ActiveFramebuffer = m_DefaultFramebuffer.get();
	std::cout << "Framebuffer initialized successfully" << std::endl;

	// Enable depth testing
	glEnable( GL_DEPTH_TEST );

	// Check for OpenGL errors
	GLenum err;
	while( ( err = glGetError() ) != GL_NO_ERROR )
	{
		std::cout << "OpenGL error during initialization: 0x" << std::hex << err << std::dec << std::endl;
	}

	return true;
}

bool Render::initShaders()
{
	// Create and compile vertex shader
	GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertexShader, 1, &vertexShaderSource, nullptr );
	glCompileShader( vertexShader );

	// Check vertex shader compilation
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &success );
	if( !success )
	{
		glGetShaderInfoLog( vertexShader, 512, nullptr, infoLog );
		std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
		return false;
	}

	// Create and compile fragment shader
	GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragmentShader, 1, &fragmentShaderSource, nullptr );
	glCompileShader( fragmentShader );

	// Check fragment shader compilation
	glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &success );
	if( !success )
	{
		glGetShaderInfoLog( fragmentShader, 512, nullptr, infoLog );
		std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
		return false;
	}

	// Create shader program and link shaders
	m_ShaderProgram = glCreateProgram();
	glAttachShader( m_ShaderProgram, vertexShader );
	glAttachShader( m_ShaderProgram, fragmentShader );
	glLinkProgram( m_ShaderProgram );

	// Check shader program linking
	glGetProgramiv( m_ShaderProgram, GL_LINK_STATUS, &success );
	if( !success )
	{
		glGetProgramInfoLog( m_ShaderProgram, 512, nullptr, infoLog );
		std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
		return false;
	}

	// Clean up shaders
	glDeleteShader( vertexShader );
	glDeleteShader( fragmentShader );

	// Verify shader program is valid
	glValidateProgram( m_ShaderProgram );
	glGetProgramiv( m_ShaderProgram, GL_VALIDATE_STATUS, &success );
	if( !success )
	{
		glGetProgramInfoLog( m_ShaderProgram, 512, nullptr, infoLog );
		std::cout << "Shader program validation failed:\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

bool Render::initBuffers()
{
	// Triangle vertices with colors
	GLfloat vertices[] = {
		// positions        // colors
		-0.5f,
		-0.5f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,  // bottom left - red
		0.5f,
		-0.5f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,  // bottom right - green
		0.0f,
		0.5f,
		0.0f,
		0.0f,
		0.0f,
		1.0f  // top - blue
	};

	// Create and bind VAO
	glGenVertexArrays( 1, &m_VAO );
	glBindVertexArray( m_VAO );

	// Create and bind VBO
	glGenBuffers( 1, &m_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	// Position attribute
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void*)0 );
	glEnableVertexAttribArray( 0 );

	// Color attribute
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void*)( 3 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );

	// Unbind
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	// Check for OpenGL errors
	GLenum err;
	while( ( err = glGetError() ) != GL_NO_ERROR )
	{
		std::cout << "OpenGL error during buffer initialization: 0x" << std::hex << err << std::dec << std::endl;
		return false;
	}

	return true;
}

void Render::cleanupBuffers()
{
	if( m_ShaderProgram != 0 )
	{
		glDeleteProgram( m_ShaderProgram );
		m_ShaderProgram = 0;
	}
	if( m_VAO != 0 )
	{
		glDeleteVertexArrays( 1, &m_VAO );
		m_VAO = 0;
	}
	if( m_VBO != 0 )
	{
		glDeleteBuffers( 1, &m_VBO );
		m_VBO = 0;
	}
}

bool Render::preUpdate()
{
	// We don't need to do anything with the default framebuffer
	// All rendering should go to our active framebuffer
	return true;
}

bool Render::update()
{
	// Make sure we have an active framebuffer
	if( !m_ActiveFramebuffer )
	{
		std::cout << "No active framebuffer set!" << std::endl;
		return false;
	}

	// Bind our framebuffer and set up for rendering
	m_ActiveFramebuffer->bind();

	// Check framebuffer status
	if( !m_ActiveFramebuffer->isComplete() )
	{
		std::cout << "Error: Active framebuffer is not complete" << std::endl;
		return false;
	}

	// Clear our framebuffer
	glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	// Use shader program
	glUseProgram( m_ShaderProgram );

	// Bind VAO and draw
	glBindVertexArray( m_VAO );
	glDrawArrays( GL_TRIANGLES, 0, 3 );
	glBindVertexArray( 0 );

	// Reset shader program
	glUseProgram( 0 );

	// Unbind our framebuffer
	m_ActiveFramebuffer->unbind();

	// Check for OpenGL errors
	GLenum err;
	while( ( err = glGetError() ) != GL_NO_ERROR )
	{
		std::cout << "OpenGL error in update: 0x" << std::hex << err << std::dec << std::endl;
		return false;
	}

	return true;
}

bool Render::shutdown()
{
	cleanupBuffers();
	return true;
}

void Render::setActiveFramebuffer( Framebuffer* framebuffer )
{
	m_ActiveFramebuffer = framebuffer ? framebuffer : m_DefaultFramebuffer.get();
}

void Render::resetToDefaultFramebuffer()
{
	m_ActiveFramebuffer = m_DefaultFramebuffer.get();
}

}  // namespace module
