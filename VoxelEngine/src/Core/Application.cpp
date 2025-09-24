#include "pch.h"
#include "Application.h"

#include "Core/Log.h"
#include "Core/Events/ApplicationEvent.h"
namespace VoxelEngine {
	Application::Application()
	{
	}

	Application::~Application()
	{
	}
	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		VE_TRACE(e);
		while (true);
	}
}
