
#include "Shader.h"
#include <fstream>

namespace RHI {

	Shader::Shader()
		: m_pContext(nullptr)
		, m_Type(ShaderType::None)
		, m_IsCompiled(false)
		, m_Language(ShaderLanguage::None)
	{
	}

	Shader::~Shader()
	{
	}

	b8 Shader::LoadShaderSource()
	{
		std::ifstream stream(m_FilePath.c_str());
		if (!stream.is_open())
		{
			//g_Logger.LogFormat(LogRenderCore(), LogLevel::Error, "Failed to open shader file: %s!", m_FilePath);
			return false;
		}

		stream.seekg(0, std::ios::end);
		sizeT size = stream.tellg();
		stream.seekg(0, std::ios::beg);


		m_Source.resize(size);
		stream.read((char*)m_Source.data(), size);
		stream.close();

		return true;
	}
}
