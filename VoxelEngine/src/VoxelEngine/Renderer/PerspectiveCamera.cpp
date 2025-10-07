#include "pch.h"
#include "PerspectiveCamera.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "VoxelEngine/Core/Application.h"
namespace VoxelEngine {
	PerspectiveCamera::PerspectiveCamera(float fov, float zNear, float zFar)
		: m_Fov(fov), m_ZNear(zNear), m_ZFar(zFar)
	{
		VE_PROFILE_FUNCTION;
		RecalculateProjectionMatrix();
		RecalculateViewMatrix();
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void PerspectiveCamera::AddToYawAndPitch(float yawOffset, float pitchOffset)
	{
		VE_PROFILE_FUNCTION;
		m_Yaw += yawOffset * yawAndPitchSensitivity;
		m_Pitch += pitchOffset * yawAndPitchSensitivity;
		if (m_Pitch > 89.0f) {
			m_Pitch = 89.0f;
		}
		else if (m_Pitch < -89.0f) {
			m_Pitch = -89.0f;
		}
		m_Front.x = std::cos(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
		m_Front.y = std::sin(glm::radians(m_Pitch));
		m_Front.z = std::sin(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(m_Front);

		m_WorldFront = m_Front;
		m_WorldFront.y = 0;
		m_WorldFront = glm::normalize(m_WorldFront);
		m_WorldRight = glm::cross(m_WorldFront, m_WorldUp);
		RecalculateViewMatrix();
	}
	void PerspectiveCamera::RecalculateProjectionMatrix()
	{
		VE_PROFILE_FUNCTION;
		m_ProjectionMatrix = glm::perspective(m_Fov, Application::Get().GetWindow().GetAspectRatio(), m_ZNear, m_ZFar);
	}
	void PerspectiveCamera::RecalculateViewMatrix() {
		VE_PROFILE_FUNCTION;
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_WorldUp);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
