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
		if (m_Yaw >= 180) {
			m_Yaw = m_Yaw - 360;
		}
		else if (m_Yaw < -180) {
			m_Yaw = 360 + m_Yaw;
		}
		m_Pitch += pitchOffset * yawAndPitchSensitivity;
		if (m_Pitch > 89.0f) {
			m_Pitch = 89.0f;
		}
		else if (m_Pitch < -89.0f) {
			m_Pitch = -89.0f;
		}
		RecalculateViewMatrix();
	}
	void PerspectiveCamera::RecalculateProjectionMatrix()
	{
		VE_PROFILE_FUNCTION;
		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), Application::Get().GetWindow().GetAspectRatio(), m_ZNear, m_ZFar);
	}
	void PerspectiveCamera::RecalculateViewMatrix() {
		VE_PROFILE_FUNCTION;
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + GetFront(), glm::vec3(0.0f, 1.0f, 0.0f));
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
