#pragma once
#include "VoxelEngine/Core/Core.h"
#include "VoxelEngine/Events/Event.h"
#include "VoxelEngine/Core/Timestep.h"
namespace VoxelEngine {

	class VE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();
		virtual void OnAttach() {}
		virtual void OnTick() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}

