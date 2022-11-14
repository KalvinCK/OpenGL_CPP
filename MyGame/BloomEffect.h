#pragma once

class BloomEffect
{
private:

    GLuint hdrFBO, rboDepth;

    // vamos ter duas texturas 

    // 2 buffers de frames
    GLuint pingpongFBO[2];
    GLuint pingpongColorbuffers[2];


    ShaderProgram shaderBloomFinal;
    ShaderProgram shaderBlur;
    
    ShaderProgram shaderBetaBloomFinal;
    Quad quad;

public:

    GLuint colorBuffer[2];

    BloomEffect();
    // Essa parte deve ser chamada antes de todos as chamadas de desenhos
    void ResizeFrame();
    void ActiveFrameBloom();
    void RenderFrameBloom();

    void RenderBetaBloom(GLuint bloomTexture);

    void Delete();

};

BloomEffect::BloomEffect()
{
    shaderBlur.createShader("Shaders/bloom/bloom.vert", "Shaders/bloom/blur.frag");
    shaderBloomFinal.createShader("Shaders/bloom/bloom.vert", "Shaders/bloom/bloom_final.frag");


    shaderBetaBloomFinal.createShader("Shaders/new bloom/new_bloom.vert", "Shaders/new bloom/newbloomfinal.frag");


    // create buffers
    glGenFramebuffers(1, &hdrFBO);
    glGenRenderbuffers(1, &rboDepth);
    glGenTextures(2, colorBuffer);

    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);

    ResizeFrame();
}
void BloomEffect::ResizeFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GameWindow::Size.x, GameWindow::Size.y, 0, GL_RGBA, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffer[i], 0);

    }

    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GameWindow::Size.x, GameWindow::Size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;


    // 2 pass
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GameWindow::Size.x, GameWindow::Size.y, 0, GL_RGBA, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

        // checando se o frameBuffer esta completo
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

// Essa parte deve ser chamada antes de todos as chamadas de desenhos
void BloomEffect::ActiveFrameBloom()
{
    ResizeFrame();

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomEffect::RenderFrameBloom()
{

    bool horizontal = true, first_iteration = true;

    unsigned int amount = 10;
    shaderBlur.Use();
    shaderBlur.setInt("count_pass", values.countPass);

    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        shaderBlur.setInt("horizontal", horizontal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffer[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
        shaderBlur.setTexture("image", 0);

        horizontal = !horizontal;
        if (first_iteration)
        {
            first_iteration = false;
        }
        quad.RenderQuad();
    }

    // 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
    // --------------------------------------------------------------------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderBloomFinal.Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);
    shaderBloomFinal.setTexture("scene", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
    shaderBloomFinal.setTexture("bloomBlur", 1);

    shaderBloomFinal.setFloat("exposure", values.exposureBloom);
    shaderBloomFinal.setFloat("gamma", values.gammaBloom);


    shaderBloomFinal.setFloat("elapsedTime", Timer::ElapsedTime);
    shaderBloomFinal.setFloat("film_grain", values.film_grain_bloom);

    quad.RenderQuad();

}
void BloomEffect::RenderBetaBloom(GLuint bloomTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderBetaBloomFinal.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);
    shaderBetaBloomFinal.setInt("scene", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomTexture);
    shaderBetaBloomFinal.setInt("bloomBlur", 1);

    shaderBetaBloomFinal.setFloat("exposure", values.new_bloom_exp);
    shaderBetaBloomFinal.setFloat("bloomStrength", values.new_bloom_streng);
    shaderBetaBloomFinal.setFloat("gamma", values.new_bloom_gama);
    shaderBetaBloomFinal.setFloat("film_grain", values.new_bloom_filmGrain);
    shaderBetaBloomFinal.setFloat("elapsedTime", Timer::ElapsedTime);

    shaderBetaBloomFinal.setFloat("nitidezStrength", values.nitidezStrengh_);

    quad.RenderQuad();
}

void BloomEffect::Delete()
{
    glDeleteTextures(1, colorBuffer);
    glDeleteRenderbuffers(1, &rboDepth);
    glDeleteFramebuffers(1, &hdrFBO);


    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);

    glDeleteFramebuffers(1, pingpongFBO);
    glDeleteTextures(1, pingpongColorbuffers);

    quad.Delete();

    shaderBlur.Delete();
    shaderBloomFinal.Delete();
}