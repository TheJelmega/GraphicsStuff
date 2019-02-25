// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// Shader.h: Shader
#pragma once
#include <string>
#include <vector>
#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace RHI {
	class RHIContext;
	
	struct ShaderDesc
	{
		ShaderType type;			/**< Shader type */
		std::string filePath;			/**< Path to shader source file */
		std::string entryPoint;			/**< Shader entry point */
		ShaderLanguage language;	/**< Shader language */
	};

	class Shader
	{
	public:
		Shader();
		virtual ~Shader();

		/**
		 * Create a shader
		 * @param[in] pContext		RHI context
		 * @param[in] desc			Shader description
		 * @return	True if the shader was created successfully, false otherwise
		 */
		virtual b8 Create(RHI::RHIContext* pContext, const ShaderDesc& desc) = 0;

		/**
		 * Destroy the shader
		 * @return	True if the shader was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

		/**
		 * Get the shader type
		 * @return	Shader type
		 */
		ShaderType GetType() const { return m_Type; }
		/**
		 * Get the shader type
		 * @return	Shader type
		 */
		const std::string& GetFilePath() const { return m_FilePath; }
		/**
		 * Get the shader type
		 * @return	Shader type
		 */
		const std::vector<u8>& GetSource() const { return m_Source; }
		/**
		* Get the shader language
		* @return	Shader language
		*/
		ShaderLanguage GetLanguage() const { return m_Language; }
		/**
		* When the shader source is text, check if it is compiled
		* @return	True if the text shader source is compiled, false otherwise
		*/
		b8 IsCompiled() const { return m_IsCompiled; }

	protected:
		/**
		 * Load the shader source
		 * @return True if the shader source was loaded, false otherwise
		 */
		b8 LoadShaderSource();

		RHI::RHIContext* m_pContext;	/**< RHI context */
		ShaderType m_Type;				/**< Shader type */
		std::string m_FilePath;			/**< Path to source */
		std::string m_EntryPoint;		/**< Entry point of the shader */
		std::vector<u8> m_Source;		/**< Source code */

		ShaderLanguage m_Language;		/**< Shader language */
		b8 m_IsCompiled;				/**< If the source was text, has it been compiled? */
	};

}
