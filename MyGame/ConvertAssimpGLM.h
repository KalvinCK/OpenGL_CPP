#pragma once

#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>

#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>


class ConvertToGLM
{
public:

	static glm::mat4 Matrix4(const aiMatrix4x4& matrix4_ASSIMP);
	static glm::vec3 Vector3(const aiVector3D& vector3_ASSIMP);
	static glm::quat Quaternion(const aiQuaternion& pOrientation_ASSIMP);
};