#pragma once
#include <VoxelEngine.h>
#include <glad/glad.h>
class SimpleExample5 : public VoxelEngine::Layer {
public:
	SimpleExample5()
		: m_Camera(70.0f, 0.1f, 1500.0f),
		m_CameraPosition(0, 0, 0)
	{
		VE_PROFILE_FUNCTION;
		unsigned int compute;
		// compute shader
		compute = glCreateShader(GL_COMPUTE_SHADER);
		std::string source = VoxelEngine::Utils::ReadFile("assets/shaders/SimpleExamples/SimpleExample5Compute.glsl");
		VE_CORE_ASSERT(!source.empty(), "Empty shader source read");
		const char* cShaderCode = source.c_str();
		glShaderSource(compute, 1, &cShaderCode, NULL);
		glCompileShader(compute);
		GLint isCompiled = 0;
		glGetShaderiv(compute, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(compute, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(compute, maxLength, &maxLength, &infoLog[0]);

			// We don't need the compute anymore.
			glDeleteShader(compute);

			VE_CORE_ERROR("{0}", infoLog.data());
			VE_CORE_ASSERT(false, "Shader compilation failure!");
		}

		// shader Program
		m_CompileShaderProgram = glCreateProgram();
		glAttachShader(m_CompileShaderProgram, compute);
		glLinkProgram(m_CompileShaderProgram);
		GLint isLinked = 0;
		glGetProgramiv(m_CompileShaderProgram, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_CompileShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_CompileShaderProgram, maxLength, &maxLength, &infoLog[0]);

			// We don't need the m_CompileShaderProgram anymore.
			glDeleteProgram(m_CompileShaderProgram);

			VE_CORE_ERROR("{0}", infoLog.data());
			VE_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		unsigned int texture;

		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_TEXTURE_WIDTH, m_TEXTURE_HEIGHT, 0, GL_RGBA,
			GL_FLOAT, NULL);

		glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		auto shader = m_ShaderLibrary.Load("assets/shaders/SimpleExamples/SimpleExample5.glsl");

		m_SquareVA.reset(VoxelEngine::VertexArray::Create());
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			 -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		VoxelEngine::Ref<VoxelEngine::VertexBuffer> squareVB(VoxelEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		VoxelEngine::BufferLayout squareLayout = {
			{VoxelEngine::ShaderDataType::Float3, "a_Position"},
			{VoxelEngine::ShaderDataType::Float2, "a_TexCoord"},
		};
		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);
		uint32_t squareIndices[] = {
			0,1,2,
			2,3,0,
		};
		VoxelEngine::Ref<VoxelEngine::IndexBuffer> squareIB(VoxelEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);
	};
	~SimpleExample5() {
		VE_PROFILE_FUNCTION;
	};
	void OnAttach() override {
		VE_PROFILE_FUNCTION;
		VoxelEngine::Application& application = VoxelEngine::Application::Get();
		application.GetWindow().SetMaximized(true);
		application.GetWindow().SetCursorVisibility(false);
	};
	void OnUpdate(VoxelEngine::Timestep ts) override {
		VE_PROFILE_FUNCTION;

		{
			VE_PROFILE_SCOPE("Key pooling");
			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_A)) {
				m_CameraPosition -= glm::normalize(m_Camera.GetRight() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_D)) {
				m_CameraPosition += glm::normalize(m_Camera.GetRight() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
			}
			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_W)) {
				m_CameraPosition += glm::normalize(m_Camera.GetFront() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_S)) {
				m_CameraPosition -= glm::normalize(m_Camera.GetFront() * glm::vec3(1, 0, 1)) * (m_CameraMoveSpeed * ts);
			}

			if (VoxelEngine::Input::IsKeyPressed(VE_KEY_SPACE)) {
				m_CameraPosition += glm::vec3(0.0f, 1.0f, 0.0f) * (m_CameraMoveSpeed * ts);
			}
			else if (VoxelEngine::Input::IsKeyPressed(VE_KEY_LEFT_SHIFT)) {
				m_CameraPosition -= glm::vec3(0.0f, 1.0f, 0.0f) * (m_CameraMoveSpeed * ts);
			}
		}

		VoxelEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		VoxelEngine::RenderCommand::Clear();
		VoxelEngine::Renderer::BeginScene(m_Camera);
		m_Camera.SetPosition(m_CameraPosition);
		glUseProgram(m_CompileShaderProgram);
		GLint location = glGetUniformLocation(m_CompileShaderProgram, "t");
		VoxelEngine::Window& window = VoxelEngine::Application::Get().GetWindow();
		glUniform1f(location, window.GetTime());
		glDispatchCompute((unsigned int)m_TEXTURE_WIDTH, (unsigned int)m_TEXTURE_HEIGHT, 1);

		// make sure writing to image has finished before read
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		auto shader = m_ShaderLibrary.Get("SimpleExample5");
		VoxelEngine::Renderer::Submit(shader, m_SquareVA, glm::translate(glm::mat4(1), glm::vec3(0, 0, -1)));
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		VoxelEngine::Renderer::EndScene();
	};
	void OnEvent(VoxelEngine::Event& event) override {
		VE_PROFILE_FUNCTION;
		VoxelEngine::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<VoxelEngine::KeyPressedEvent>([](VoxelEngine::KeyPressedEvent& e) {
			if (e.GetKeyCode() == VE_KEY_ESCAPE) {
				VoxelEngine::Application::Get().Close();
				return true;
			}
			return false;
			});
		dispatcher.Dispatch<VoxelEngine::MouseMovedEvent>([&](VoxelEngine::MouseMovedEvent& e) {
			m_Camera.AddToYawAndPitch(e.GetXOffset(), e.GetYOffset());
			return true;
			});
		dispatcher.Dispatch<VoxelEngine::WindowResizeEvent>([&](VoxelEngine::WindowResizeEvent& e) {
			m_Camera.RecalculateProjectionMatrix();
			return false;
			});
	};
	virtual void OnImGuiRender() override {
		VE_PROFILE_FUNCTION;
	};
	virtual void OnDetach() override {
		VE_PROFILE_FUNCTION;
	};

	inline VoxelEngine::PerspectiveCamera& GetCamera() { return m_Camera; }
private:
	VoxelEngine::ShaderLibrary m_ShaderLibrary;
	VoxelEngine::Ref<VoxelEngine::VertexArray> m_SquareVA;
	VoxelEngine::PerspectiveCamera m_Camera;
	uint32_t m_CompileShaderProgram;
	//glm::vec3 m_SquarePosition;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;
	unsigned int m_VAO;
	const unsigned int m_TEXTURE_WIDTH = 512, m_TEXTURE_HEIGHT = 512;
};
