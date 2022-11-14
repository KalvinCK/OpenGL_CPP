#pragma once

#include <map>
#include <vector>
#include <iostream>
using namespace std;

#include "ShaderProgram.h"
#include "TextureProgram.h"
#include "Model.h"
#include "Mesh.h"


extern Camera camera;

class Game
{
private:
public:
	
	Game(const string & model3D, bool flipUvs = false)
	{
	
		
	}
	void RenderFrame()
	{
		

	}
	void RenderMeshs()
	{
	}
	void Delete()
	{

	}
};

//class AnimationMesh
//{
//private:
//	Model model_mesh;
//	Animation danceanimation;
//	Animator animator;
//
//	ShaderProgram Shader;
//	map<string, TextureProgram> texturesMap;
//	
//public:
//	mat4 model = mat4(1.0f);
//	CubeTextures cube_texturesMap;
//	AnimationMesh(const string& model3D, bool flipUvs = false)
//	{
//		
//
//		model_mesh.Load(model3D);
//		danceanimation.init(model3D, &model_mesh);
//		animator.Init(&danceanimation);
//
//		
//		Shader.createShader("Shaders/shader.vert", "Shaders/shader.frag");
//
//		for (auto item : model_mesh.meshes)
//		{
//			LoadTextures(item.DiffusePath, false);
//
//			item.PrintTexturesMap();
//		}
//		cout << "Load texture map for meshes complete..." << endl;
//	}
//	void RenderFrame()
//	{
//		Shader.Use();
//
//		Shader.SetMatrix4("projection", camera.ProjectionMatrix);
//		Shader.SetMatrix4("view", camera.ViewMatrix);
//
//		auto transforms = animator.GetFinalBoneMatrices();
//		for (int i = 0; i < transforms.size(); ++i)
//			Shader.SetMatrix4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
//
//		model = glm::mat4(1.0f);
//		model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
//		//model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
//		Shader.SetMatrix4("model", model);
//
//		for (int i = 0; i < model_mesh.meshes.size(); i++)
//		{
//			texturesMap[model_mesh.meshes[i].DiffusePath].Use(GL_TEXTURE0);
//			Shader.setTexture("DiffuseMap", 0);
//			model_mesh.meshes[i].RenderFrame();
//		}
//		
//
//	}
//	void UpdateFrame()
//	{
//		animator.UpdateAnimation(window.DeltaTime);
//	}
//	void Delete()
//	{
//		model_mesh.Delete();
//
//		for (auto index : texturesMap) 
//			texturesMap[index.first].Delete();
//
//		Shader.Delete();
//	}
//	void RenderMeshs()
//	{
//		model_mesh.RenderFrame();
//	}
//};