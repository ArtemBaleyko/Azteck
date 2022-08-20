#pragma once

namespace Azteck
{
	enum class ShaderDataType
	{
		None = 0,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case Azteck::ShaderDataType::None:		return 0;
			case Azteck::ShaderDataType::Int:		return 4;
			case Azteck::ShaderDataType::Int2:		return 4 * 2;
			case Azteck::ShaderDataType::Int3:		return 4 * 3;
			case Azteck::ShaderDataType::Int4:		return 4 * 4;
			case Azteck::ShaderDataType::Float:		return 4;
			case Azteck::ShaderDataType::Float2:	return 4 * 2;
			case Azteck::ShaderDataType::Float3:	return 4 * 3;
			case Azteck::ShaderDataType::Float4:	return 4 * 4;
			case Azteck::ShaderDataType::Mat3:		return 4 * 3 * 3;
			case Azteck::ShaderDataType::Mat4:		return 4 * 4 * 4;
			case Azteck::ShaderDataType::Bool:		return 1;
			default:
			{
				AZ_CORE_ASSERT(false, "Unknown ShaderDataType");
				return 0;
			}
		}
	}

	struct BufferElement
	{
		BufferElement()
			: name("")
			, type(ShaderDataType::None)
			, size(0)
			, offset(0)
			, normalized(false)
		{}

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: name(name)
			, type(type)
			, size(ShaderDataTypeSize(type))
			, offset(0)
			, normalized(normalized)
		{}

		uint32_t getComponentCount() const
		{
			switch (type)
			{
				case Azteck::ShaderDataType::None:		return 0;
				case Azteck::ShaderDataType::Int:		return 1;
				case Azteck::ShaderDataType::Int2:		return 2;
				case Azteck::ShaderDataType::Int3:		return 3;
				case Azteck::ShaderDataType::Int4:		return 4;
				case Azteck::ShaderDataType::Float:		return 1;
				case Azteck::ShaderDataType::Float2:	return 2;
				case Azteck::ShaderDataType::Float3:	return 3;
				case Azteck::ShaderDataType::Float4:	return 4;
				case Azteck::ShaderDataType::Mat3:		return 9;
				case Azteck::ShaderDataType::Mat4:		return 16;
				case Azteck::ShaderDataType::Bool:		return 1;
				default:
				{
					AZ_CORE_ASSERT(false, "Unknown ShaderDataType");
					return 0;
				}
			}
		}

		std::string name;
		ShaderDataType type;
		uint32_t size;
		uint32_t offset;
		bool normalized;
	};


	class BufferLayout
	{
	public:
		BufferLayout();
		BufferLayout(const std::initializer_list<BufferElement>& elements);

		inline const std::vector<BufferElement>& getElements() const { return _elements; }
		inline const uint32_t getStride() const { return _stride; }

		std::vector<BufferElement>::iterator begin() { return _elements.begin(); }
		std::vector<BufferElement>::iterator end() { return _elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return _elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return _elements.end(); }
	private:
		void calculateOffsetsAndStride();

	private:
		std::vector<BufferElement> _elements;
		uint32_t _stride = 0;
	};


	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {};

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual const BufferLayout& getLayout() const = 0;
		virtual void setLayout(const BufferLayout& layout) = 0;

		virtual void setData(const void* data, uint32_t size) = 0;

		static Ref<VertexBuffer> create(uint32_t size);
		static Ref<VertexBuffer> create(float* vertices, uint32_t size);
	};


	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {};

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual uint32_t getCount() const = 0;

		static Ref<IndexBuffer> create(uint32_t* indices, uint32_t count);
	};
}