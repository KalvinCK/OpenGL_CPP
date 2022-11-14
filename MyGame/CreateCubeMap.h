#pragma once

#include <stb_image.h>
#include <string>
using namespace std;

struct CubeTextures { GLuint Background_CB_Map, Irradiance_CB_Map, Prefilter_CB_Map; };

class CreateCubeMap
{
private:
    Quad quad;
    Cube cube;

    ShaderProgram equirectangularToCubemapShader;

    ShaderProgram backgroundShader, irradianceShader, prefilterShader;

    // frameBuffer, renderBuffer
    GLuint captureFrameBO, captureRenderBO;

    // buffers textures
    GLuint HDR_Texture;

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[6] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    int size = 1024;
    GLenum InternalFormat;
    // essas texturas serao usadas para configurar a reflexao, elas serao publicas porque vamos precisar delas em outras
    // partes para criar uma simulacao de reflexao no objeto.
    // ------------------------------------------------------------------------------------------------------
    // O irradiance vai ser uma textura de cubo com o brilho irradiado sem nenhum detalhe apenas o brilho de forma dispersa.
    // ja o Prefilter vai ser uma textura de cubo parecida com a textura padrao porem bem menos nitida.
    // Caso vc deseje que o seu modelo tenha uma reflexao mais nitida comparado ao ambiente, vc deve usar a a textura de cubo background
    CubeTextures cubeTexturesMap;

public:
    CreateCubeMap(const string& path, GLenum internal_Format = GL_RGB32F);

    void LoadImage(const string& pathImage);
    void CreateBackground();
    void ConfigureIrradianceMap();
    void PreFilter();
    CubeTextures GetTexturesMap();

    void RenderFrame();
    void Delete();

};

CreateCubeMap::CreateCubeMap(const string& path, GLenum internal_Format)
{
    equirectangularToCubemapShader.createShader("Shaders/CubeMap/cubemap.vert", "Shaders/CubeMap/equirectangular_to_cubemap.frag");

    backgroundShader.createShader("Shaders/CubeMap/background.vert", "Shaders/CubeMap/background.frag");
    irradianceShader.createShader("Shaders/CubeMap/cubemap.vert", "Shaders/CubeMap/irradiance_convolution.frag");
    prefilterShader.createShader("Shaders/CubeMap/cubemap.vert", "Shaders/CubeMap/prefilter.frag.");

    this->InternalFormat = internal_Format;

    LoadImage(path);
    CreateBackground();
    ConfigureIrradianceMap();
    PreFilter();


}
// Vou dividir as etapas em funçoes apenas por questoes de simplicidade
// -----------------------------------------------------------------------
void CreateCubeMap::LoadImage(const string& pathImage)
{
    // Aqui carregamos a imagem normalmente pode ser de qualquer formato mais hdr sempre vai ser melhor
    glGenFramebuffers(1, &captureFrameBO);
    glGenRenderbuffers(1, &captureRenderBO);


    glBindFramebuffer(GL_FRAMEBUFFER, captureFrameBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRenderBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRenderBO);

    stbi_set_flip_vertically_on_load(true);

    // Aqui carregamos a imagem normalmente
    int width, height, nrComponents;
    float* data = stbi_loadf(pathImage.c_str(), &width, &height, &nrComponents, 0);
    if (!data)
    {
        cout << "Erro ao carregar HDR Image: " << pathImage << endl;
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, &HDR_Texture);
    glBindTexture(GL_TEXTURE_2D, HDR_Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

    stbi_image_free(data);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

}
// criamo o cube map atraves da textura que carregamos
// ------------------------------------------------------------------------------------------------------
void CreateCubeMap::CreateBackground()
{
    // cube map
    glGenTextures(1, &cubeTexturesMap.Background_CB_Map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexturesMap.Background_CB_Map);


    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, InternalFormat, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    equirectangularToCubemapShader.Use();
    equirectangularToCubemapShader.SetMatrix4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, HDR_Texture);
    equirectangularToCubemapShader.setInt("equirectangularMap", 0);

    //glViewport(0, 0, 1024, 1024);
    glViewport(0, 0, size, size);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFrameBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.SetMatrix4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeTexturesMap.Background_CB_Map, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.RenderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
// Configurando o irradianceMap 
// aqui iremos configurar o cube map, porque vamos precisar para configurarmos os shader para reflexão
// ----------------------------------------------------------------------------------------
void CreateCubeMap::ConfigureIrradianceMap()
{

    glGenTextures(1, &cubeTexturesMap.Irradiance_CB_Map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexturesMap.Irradiance_CB_Map);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, InternalFormat, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFrameBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRenderBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    irradianceShader.Use();
    irradianceShader.SetMatrix4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexturesMap.Background_CB_Map);
    irradianceShader.setTexture("environmentMap", 0);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFrameBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.SetMatrix4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeTexturesMap.Irradiance_CB_Map, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.RenderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
// crie um mapa pre filtro de cubo
// ------------------------------------------------------------------------------------------------------
void CreateCubeMap::PreFilter()
{
    glGenTextures(1, &cubeTexturesMap.Prefilter_CB_Map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexturesMap.Prefilter_CB_Map);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, InternalFormat, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

// execute uma simulação quase monte-carlo na iluminação do ambiente para criar um mapa de pré-filtro (cubo).
// ------------------------------------------------------------------------------------------------------

    prefilterShader.Use();
    prefilterShader.SetMatrix4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexturesMap.Background_CB_Map);
    prefilterShader.setInt("environmentMap", 0);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFrameBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));

        glBindRenderbuffer(GL_RENDERBUFFER, captureRenderBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.SetMatrix4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeTexturesMap.Prefilter_CB_Map, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube.RenderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
inline CubeTextures CreateCubeMap::GetTexturesMap()
{
    return cubeTexturesMap;
}
// Renderize o cube map 
void CreateCubeMap::RenderFrame()
{
    backgroundShader.Use();
    backgroundShader.SetMatrix4("projection", Camera::ProjectionMatrix);
    backgroundShader.SetMatrix4("view", Camera::ViewMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexturesMap.Background_CB_Map);
    backgroundShader.setInt("environmentMap", 0);
    backgroundShader.setFloat("gamma", values.gamma_Background);
    glDepthFunc(GL_LEQUAL);
    cube.RenderCube();
    glDepthFunc(GL_LESS);
}
void CreateCubeMap::Delete()
{
    equirectangularToCubemapShader.Delete();
    backgroundShader.Delete();
    irradianceShader.Delete();
    prefilterShader.Delete();

    glDeleteTextures(1, &HDR_Texture);
    glDeleteTextures(1, &cubeTexturesMap.Background_CB_Map);
    glDeleteTextures(1, &cubeTexturesMap.Irradiance_CB_Map);
    glDeleteTextures(1, &cubeTexturesMap.Prefilter_CB_Map);

    glDeleteFramebuffers(1, &captureFrameBO);
    glDeleteRenderbuffers(1, &captureRenderBO);

    quad.Delete();
    cube.Delete();
}