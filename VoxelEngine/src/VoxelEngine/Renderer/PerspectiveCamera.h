#pragma once
#include <glm/glm.hpp>

namespace VoxelEngine {
	class PerspectiveCamera {
	public:
		PerspectiveCamera(float fov, float zNear, float zFar);

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		//float GetRotation() const { return m_Rotation; }
		//void SetRotation(const float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
		void AddToYawAndPitch(float yawOffset, float pitchOffset);
		void RecalculateProjectionMatrix();

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.0f , 0.0f, 0.0f };
		glm::vec3 m_Front = { 0.0f, 0.0f, -1.0f };
		glm::vec3 m_WorldUp = { 0.0f, 1.0f, 0.0f };

		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;
		float m_Fov;
		float m_ZNear;
		float m_ZFar;
	};
}
