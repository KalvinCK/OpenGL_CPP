#include <GL/glew.h>

#include "FreeTypeProgram.h"

FreeType::FreeType(std::string fontPath)
{
	const std::string vert_shader = R"(
										#version 460 core

										layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

										out vec2 TexCoords;

										uniform mat4 projection;

										void main()
										{
											gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
											TexCoords = vertex.zw;
										})";
	const std::string frag_shader = R"(
										#version 460 core

										in vec2 TexCoords;
										out vec4 FragColor;

										uniform sampler2D text;
										uniform vec4 textColor;

										void main()
										{    
											vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
											FragColor = textColor * sampled;
										})";
	shaderFonts.CreateShaderString(vert_shader, frag_shader);

	FT_Library ft;

	if (FT_Init_FreeType(&ft))
	{
		cout << "ERROR: FREETYPE, ao iniciar a freetype" << endl;
		exit(EXIT_FAILURE);
	}
	if (!ifstream(fontPath))
	{
		cout << "Nao foi possivel carregar a font: " << fontPath << endl;
		exit(EXIT_FAILURE);
	}

	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
	{
		cout << "ERROR: FREETYPE, fonte nao carreada: " << fontPath << endl;
		exit(EXIT_FAILURE);
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load first 128 characters of ASCII set
	GLuint texture;
	for (unsigned char c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Falha ao carregar o glifo..." << std::endl;
			continue;
		}
		else
		{
			chs.push_back(c);
		}
		// generate texture

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character =
		{
			texture,
			ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};
		characteres.insert(std::pair<char, Character>(c, character));
	}
	glDeleteTextures(1, &texture);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &Vao);
	glBindVertexArray(Vao);

	glGenBuffers(1, &Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

}
void FreeType::RenderFrame(std::string text, vec2 position, float scale, vec4 color)
{
	glBindVertexArray(Vao);

	shaderFonts.Use();
	mat4 projection = glm::ortho(0.0f, static_cast<float>(GameWindow::Size.x), 0.0f, static_cast<float>(GameWindow::Size.y));
	shaderFonts.SetMatrix4("projection", projection);
	shaderFonts.SetVec4("textColor", color);
	glActiveTexture(GL_TEXTURE0);

	std::string::const_iterator c;

	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characteres[*c];

		float xpos = position.x + ch.Bearing.x * scale;
		float ypos = position.y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		shaderFonts.setTexture("text", 0);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, Vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		position.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
}
void FreeType::Delete()
{
	shaderFonts.Delete();
	glDeleteVertexArrays(1, &Vao);
	glDeleteBuffers(1, &Vbo);

	for (unsigned char c = 0; c < chs.size(); c++)
	{
		glDeleteTextures(1, &characteres[c].TextureID);
	}
}