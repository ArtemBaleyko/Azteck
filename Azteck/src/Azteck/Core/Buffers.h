#pragma once

#include <stdint.h>
#include <cstring>

namespace Azteck {

	// Non-owning raw buffer class
	class RawBuffer
	{
	public:
		RawBuffer() = default;

		RawBuffer(uint64_t size)
		{
			allocate(size);
		}

		RawBuffer(const RawBuffer&) = default;

		static RawBuffer Copy(RawBuffer other)
		{
			RawBuffer result(other._size);
			memcpy(result._data, other._data, other._size);
			return result;
		}

		void allocate(uint64_t size)
		{
			release();

			_data = new uint8_t[size];
			_size = size;
		}

		void release()
		{
			delete[] _data;
			_data = nullptr;
			_size = 0;
		}

		uint8_t* data() { return _data; }
		uint64_t size() { return _size; }

		template<typename T>
		T* as()
		{
			return (T*)_data;
		}

		operator bool() const
		{
			return (bool)_data;
		}

	private:
		uint8_t* _data = nullptr;
		uint64_t _size = 0;
	};

	class ScopedBuffer
	{
	public:
		ScopedBuffer(RawBuffer buffer)
			: _buffer(buffer)
		{
		}

		ScopedBuffer(uint64_t size)
			: _buffer(size)
		{
		}

		~ScopedBuffer()
		{
			_buffer.release();
		}

		uint8_t* data() { return _buffer.data(); }
		uint64_t size() { return _buffer.size(); }

		template<typename T>
		T* as()
		{
			return _buffer.as<T>();
		}

		operator bool() const { return _buffer; }
	private:
		RawBuffer _buffer;
	};


}
