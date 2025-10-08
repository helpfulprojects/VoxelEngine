#pragma once
#include <glm/gtx/quaternion.hpp>
namespace VoxelEngine {
	class PerspectiveCamera {
	public:
		PerspectiveCamera(float fov, float zNear, float zFar);

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		void AddToYawAndPitch(float yawOffset, float pitchOffset);
		void RecalculateProjectionMatrix();

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		const glm::vec3 GetFront() const { return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, 1.0f)); }
		const glm::vec3 GetRight() const { return glm::rotate(GetOrientation(), glm::vec3(-1.0f, 0.0f, 0.0f)); }
		const glm::vec3 GetUp() const { return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f)); }
		glm::quat GetOrientation() const { return glm::quat(glm::vec3(-glm::radians(m_Pitch), -glm::radians(m_Yaw), 0)); };

		const float GetYaw() const { return m_Yaw; }
		const float GetPitch() const { return m_Pitch; }
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.0f , 0.0f, 0.0f };

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;
		float m_Fov;
		float m_ZNear;
		float m_ZFar;

		float yawAndPitchSensitivity = 0.1f;
	};
}
