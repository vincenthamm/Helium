#pragma once

#include "Graphics/Graphics.h"
#include "Engine/Resource.h"

#include "Rendering/RShader.h"

#include "Reflect/Enumeration.h"
#include "Reflect/Structure.h"

namespace Helium
{
	class ShaderVariant;
	typedef Helium::StrongPtr< ShaderVariant > ShaderVariantPtr;
	typedef Helium::StrongPtr< const ShaderVariant > ConstShaderVariantPtr;

	HELIUM_DECLARE_RPTR( RShader );

	/// Shader constant information.
	struct HELIUM_GRAPHICS_API ShaderConstantInfo : Reflect::StructureBase
	{
		REFLECT_DECLARE_BASE_STRUCTURE(Helium::ShaderConstantInfo);
		static void PopulateStructure( Reflect::Structure& comp );
		
		inline bool operator==( const ShaderConstantInfo& _rhs ) const;
		inline bool operator!=( const ShaderConstantInfo& _rhs ) const;

		/// Constant name.
		Name name;
		/// Offset of the constant within the buffer, in bytes.
		uint16_t offset;
		/// Size of the constant, in bytes.
		uint16_t size;
		/// Used size of the constant, in bytes.
		uint16_t usedSize;
	};

	/// Constant buffer information.
	struct HELIUM_GRAPHICS_API ShaderConstantBufferInfo : Reflect::StructureBase
	{
		REFLECT_DECLARE_BASE_STRUCTURE(Helium::ShaderConstantBufferInfo);
		static void PopulateStructure( Reflect::Structure& comp );
		
		inline bool operator==( const ShaderConstantBufferInfo& _rhs ) const;
		inline bool operator!=( const ShaderConstantBufferInfo& _rhs ) const;

		/// Constant buffer name.
		Name name;
		/// Shader constants.
		DynamicArray< ShaderConstantInfo > constants;
		/// Index of the buffer within the shader.
		uint16_t index;
		/// Buffer size, in bytes.
		uint16_t size;
	};

	/// Constant buffer information set.
	struct HELIUM_GRAPHICS_API ShaderConstantBufferInfoSet : Reflect::StructureBase
	{
		REFLECT_DECLARE_BASE_STRUCTURE(Helium::ShaderConstantBufferInfoSet);
		static void PopulateStructure( Reflect::Structure& comp );
		
		inline bool operator==( const ShaderConstantBufferInfoSet& _rhs ) const;
		inline bool operator!=( const ShaderConstantBufferInfoSet& _rhs ) const;

		/// Constant buffers.
		DynamicArray< ShaderConstantBufferInfo > buffers;
	};

	/// Shader sampler input information.
	struct HELIUM_GRAPHICS_API ShaderSamplerInfo : Reflect::StructureBase
	{
		REFLECT_DECLARE_BASE_STRUCTURE(Helium::ShaderSamplerInfo);
		static void PopulateStructure( Reflect::Structure& comp );
		
		inline bool operator==( const ShaderSamplerInfo& _rhs ) const;
		inline bool operator!=( const ShaderSamplerInfo& _rhs ) const;

		/// Sampler name.
		Name name;
		/// Bind point index.
		uint16_t bindIndex;
	};

	/// Shader sampler input information set.
	struct HELIUM_GRAPHICS_API ShaderSamplerInfoSet : Reflect::StructureBase
	{
		REFLECT_DECLARE_BASE_STRUCTURE(Helium::ShaderSamplerInfoSet);
		static void PopulateStructure( Reflect::Structure& comp );
		
		inline bool operator==( const ShaderSamplerInfoSet& _rhs ) const;
		inline bool operator!=( const ShaderSamplerInfoSet& _rhs ) const;

		/// Sampler inputs.
		DynamicArray< ShaderSamplerInfo > inputs;
	};

	/// Shader texture input information.
	struct HELIUM_GRAPHICS_API ShaderTextureInfo : Reflect::StructureBase
	{
		REFLECT_DECLARE_BASE_STRUCTURE(Helium::ShaderTextureInfo);
		static void PopulateStructure( Reflect::Structure& comp );
		
		inline bool operator==( const ShaderTextureInfo& _rhs ) const;
		inline bool operator!=( const ShaderTextureInfo& _rhs ) const;

		/// Texture variable name.
		Name name;
		/// Bind point index.
		uint16_t bindIndex;
	};

	/// Shader texture input information set.
	struct HELIUM_GRAPHICS_API ShaderTextureInfoSet : Reflect::StructureBase
	{
		REFLECT_DECLARE_BASE_STRUCTURE(Helium::ShaderTextureInfoSet);
		static void PopulateStructure( Reflect::Structure& comp );
		
		inline bool operator==( const ShaderTextureInfoSet& _rhs ) const;
		inline bool operator!=( const ShaderTextureInfoSet& _rhs ) const;

		/// Texture inputs.
		DynamicArray< ShaderTextureInfo > inputs;
	};
	
	struct HELIUM_GRAPHICS_API CompiledShaderData : Reflect::Object
	{
		REFLECT_DECLARE_OBJECT(Helium::CompiledShaderData, Reflect::Object);
		static void PopulateStructure( Reflect::Structure& comp );

		CompiledShaderData();
		virtual ~CompiledShaderData();
		
		inline bool operator==( const CompiledShaderData& _rhs ) const;
		inline bool operator!=( const CompiledShaderData& _rhs ) const;
		
		DynamicArray< uint8_t > compiledCodeBuffer;
		DynamicArray< ShaderConstantBufferInfo > constantBuffers;
		DynamicArray< ShaderSamplerInfo > samplerInputs;
		DynamicArray< ShaderTextureInfo > textureInputs;
	};

	/// Graphics shader resource.
	class HELIUM_GRAPHICS_API Shader : public Resource
	{
		HELIUM_DECLARE_ASSET( Shader, Resource );

		friend class ShaderResourceHandler;

	public:
		/// Shader variant begin-load override callback type.
		typedef size_t ( BEGIN_LOAD_VARIANT_FUNC )( void* pCallbackData, Shader* pShader, RShader::EType shaderType, uint32_t userOptionIndex );
		/// Shader variant try-finish-load override callback type.
		typedef bool ( TRY_FINISH_LOAD_VARIANT_FUNC )( void* pCallbackData, size_t loadId, ShaderVariantPtr& rspVariant );

		/// Shader preprocessor toggle.
		struct HELIUM_GRAPHICS_API Toggle : Reflect::StructureBase
		{
			REFLECT_DECLARE_BASE_STRUCTURE(Shader::Toggle);
			static void PopulateStructure( Reflect::Structure& comp );
			
			inline bool operator==( const Toggle& _rhs ) const;
			inline bool operator!=( const Toggle& _rhs ) const;

			/// Toggle name.
			Name name;
			/// Applicable shader type flags.
			uint32_t shaderTypeFlags;
		};

		/// Shader preprocessor selection.
		struct HELIUM_GRAPHICS_API Select : Reflect::StructureBase
		{
			REFLECT_DECLARE_BASE_STRUCTURE(Shader::Select);
			static void PopulateStructure( Reflect::Structure& comp );
			
			inline bool operator==( const Select& _rhs ) const;
			inline bool operator!=( const Select& _rhs ) const;

			/// Selection name.
			Name name;
			/// Selection choices.
			DynamicArray< Name > choices;

			union
			{
				struct
				{
					/// Applicable shader type flags.
					uint32_t shaderTypeFlags : 31;
					/// Non-zero if this selection is optional.
					uint32_t bOptional : 1;
				};
				uint32_t allFlags;
			};
		};

		/// Selection name/choice pair.
		struct SelectPair : Reflect::StructureBase
		{
			REFLECT_DECLARE_BASE_STRUCTURE(Shader::SelectPair);
			static void PopulateStructure( Reflect::Structure& comp );
			
			SelectPair() {}
			SelectPair( Name name, Name choice ) : name( name ), choice( choice ) {}
			inline bool operator==( const SelectPair& _rhs ) const;
			inline bool operator!=( const SelectPair& _rhs ) const;

			/// Selection name.
			Name name;
			/// Selection choice.
			Name choice;
		};

		/// Preprocessor shader options.
		struct HELIUM_GRAPHICS_API Options : Reflect::StructureBase
		{
		public:
			REFLECT_DECLARE_BASE_STRUCTURE(Shader::Options);
			static void PopulateStructure( Reflect::Structure& comp );
			
			inline bool operator==( const Options& _rhs ) const;
			inline bool operator!=( const Options& _rhs ) const;

			/// @name Data Access
			//@{
			inline DynamicArray< Toggle >& GetToggles();
			inline const DynamicArray< Toggle >& GetToggles() const;

			inline DynamicArray< Select >& GetSelects();
			inline const DynamicArray< Select >& GetSelects() const;
			//@}

			/// @name Variant Identification
			//@{
			size_t GetOptionSetIndex(
				RShader::EType shaderType, const Name* pToggleNames, size_t toggleNameCount,
				const SelectPair* pSelectPairs, size_t selectPairCount ) const;
			size_t GetOptionSetIndex(
				RShader::EType shaderType, const SelectPair* pOptionPairs, size_t optionPairCount ) const;

			void GetOptionSetFromIndex(
				RShader::EType shaderType, size_t index, DynamicArray< Name >& rToggleNames,
				DynamicArray< SelectPair >& rSelectPairs ) const;

			size_t ComputeOptionSetCount( RShader::EType shaderType ) const;
			//@}

		private:
			/// Preprocessor toggles.
			DynamicArray< Toggle > m_toggles;
			/// Preprocessor selections.
			DynamicArray< Select > m_selects;
		};

		/// Persistent shader resource data.
		class HELIUM_GRAPHICS_API PersistentResourceData : public Reflect::Object
		{
		public:
			REFLECT_DECLARE_OBJECT(Shader::PersistentResourceData, Reflect::Object);
			static void PopulateStructure( Reflect::Structure& comp );

			/// @name Data Access
			//@{
			inline Options& GetSystemOptions();
			inline const Options& GetSystemOptions() const;

			inline Options& GetUserOptions();
			inline const Options& GetUserOptions() const;
			//@}

		private:
			/// System preprocessor options.
			Options m_systemOptions;
			/// User preprocessor options.
			Options m_userOptions;
		};

		/// @name Construction/Destruction
		//@{
		Shader();
		virtual ~Shader();
		//@}

		/// @name Serialization
		//@{

		virtual void FinalizeLoad();

#if HELIUM_TOOLS
		virtual void PostSave();
#endif
		//@}

		/// @name Resource Serialization
		//@{
		virtual bool LoadPersistentResourceObject(Reflect::ObjectPtr &_object);
		//@}

		/// @name Resource Caching Support
		//@{
		virtual Name GetCacheName() const;
		//@}

		/// @name Data Access
		//@{
		inline bool GetPrecacheAllVariants() const;

		inline const Options& GetSystemOptions() const;
		inline const Options& GetUserOptions() const;
		//@}

		/// @name Variant Identification
		//@{
		size_t BeginLoadVariant( RShader::EType shaderType, uint32_t userOptionIndex );
		bool TryFinishLoadVariant( size_t loadId, ShaderVariantPtr& rspVariant );
		//@}

		/// @name Variant Load Override Support
		//@{
		static void SetVariantLoadOverride(
			BEGIN_LOAD_VARIANT_FUNC* pBeginLoadVariantOverride,
			TRY_FINISH_LOAD_VARIANT_FUNC* pTryFinishLoadVariantOverride, void* pVariantLoadOverrideData );
		inline static BEGIN_LOAD_VARIANT_FUNC* GetBeginLoadVariantOverride();
		inline static TRY_FINISH_LOAD_VARIANT_FUNC* GetTryFinishLoadVariantOverride();
		inline static void* GetVariantLoadOverrideData();
		//@}

	private:
		/// Persistent shader resource data.
		PersistentResourceData m_persistentResourceData;

		/// Cached number of user option variants for each shader type.
		uint32_t m_variantCounts[ RShader::TYPE_MAX ];

		/// True to precache all possible shader variants.
		bool m_bPrecacheAllVariants;

		/// Shader variant begin-load override callback.
		static BEGIN_LOAD_VARIANT_FUNC* sm_pBeginLoadVariantOverride;
		/// Shader variant try-finish-load override callback.
		static TRY_FINISH_LOAD_VARIANT_FUNC* sm_pTryFinishLoadVariantOverride;
		/// Shader variant load override callback data.
		static void* sm_pVariantLoadOverrideData;
	};

	/// Single variation of a shader.
	class HELIUM_GRAPHICS_API ShaderVariant : public Resource
	{
		HELIUM_DECLARE_ASSET( ShaderVariant, Resource );

	public:
		/// @name Construction/Destruction
		//@{
		ShaderVariant();
		virtual ~ShaderVariant();
		//@}
		
		/// Persistent shader resource data.
		class HELIUM_GRAPHICS_API PersistentResourceData : public Reflect::Object
		{
		public:
			PersistentResourceData();

			REFLECT_DECLARE_OBJECT(ShaderVariant::PersistentResourceData, Reflect::Object);
			static void PopulateStructure( Reflect::Structure& comp );

			uint32_t m_resourceCount;
		};

		PersistentResourceData m_persistentResourceData;

		/// @name Asset Interface
		//@{
		virtual void PreDestroy();
		//@}

		/// @name Serialization
		//@{
		virtual bool NeedsPrecacheResourceData() const;
		virtual bool BeginPrecacheResourceData();
		virtual bool TryFinishPrecacheResourceData();
		//@}

		/// @name Resource Serialization
		//@{
		virtual bool LoadPersistentResourceObject(Reflect::ObjectPtr &_object);
		//@}

		/// @name Resource Caching Support
		//@{
		virtual Name GetCacheName() const;
		//@}

		/// @name Data Access
		//@{
		inline RShader* GetRenderResource( size_t index ) const;
		inline const ShaderConstantBufferInfoSet* GetConstantBufferInfoSet( size_t index ) const;
		inline const ShaderSamplerInfoSet* GetSamplerInfoSet( size_t index ) const;
		inline const ShaderTextureInfoSet* GetTextureInfoSet( size_t index ) const;

		inline Shader* GetShader() const;
		//@}

	private:
		/// Async load data for cached shader code.
		struct LoadData 
		{
			/// Target buffer for async loading.
			void* pData;
			/// Async load ID.
			size_t id;
			/// Size of the loaded data.
			size_t size;
		};

		/// Shader render resources.
		DynamicArray< RShaderPtr > m_renderResources;
		/// Shader constant buffers.
		DynamicArray< ShaderConstantBufferInfoSet > m_constantBufferSets;
		/// Sampler inputs.
		DynamicArray< ShaderSamplerInfoSet > m_samplerInputSets;
		/// Texture inputs.
		DynamicArray< ShaderTextureInfoSet > m_textureInputSets;

		/// Async load data for cached shader code.
		DynamicArray< LoadData > m_renderResourceLoads;
		/// Buffer for async loading of cached shader code.
		void* m_pRenderResourceLoadBuffer;
	};
}

#include "Graphics/Shader.inl"
