#include "ConvertAssimpGLM.h"

glm::mat4 ConvertToGLM::Matrix4(const aiMatrix4x4& matrix4_ASSIMP)
{
	glm::mat4 to;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = matrix4_ASSIMP.a1; to[1][0] = matrix4_ASSIMP.a2; to[2][0] = matrix4_ASSIMP.a3; to[3][0] = matrix4_ASSIMP.a4;
	to[0][1] = matrix4_ASSIMP.b1; to[1][1] = matrix4_ASSIMP.b2; to[2][1] = matrix4_ASSIMP.b3; to[3][1] = matrix4_ASSIMP.b4;
	to[0][2] = matrix4_ASSIMP.c1; to[1][2] = matrix4_ASSIMP.c2; to[2][2] = matrix4_ASSIMP.c3; to[3][2] = matrix4_ASSIMP.c4;
	to[0][3] = matrix4_ASSIMP.d1; to[1][3] = matrix4_ASSIMP.d2; to[2][3] = matrix4_ASSIMP.d3; to[3][3] = matrix4_ASSIMP.d4;
	return to;
}

glm::vec3 ConvertToGLM::Vector3(const aiVector3D& vector3_ASSIMP)
{
	return glm::vec3(vector3_ASSIMP.x, vector3_ASSIMP.y, vector3_ASSIMP.z);
}

glm::quat ConvertToGLM::Quaternion(const aiQuaternion& pOrientation_ASSIMP)
{
	return glm::quat(pOrientation_ASSIMP.w, pOrientation_ASSIMP.x, pOrientation_ASSIMP.y, pOrientation_ASSIMP.z);
}