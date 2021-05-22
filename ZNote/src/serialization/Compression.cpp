#include "Compression.h"


namespace app {

	typedef void(*CompressionFunc)(uint8_t** dataPtr, uint32_t size, uint32_t* newSize);

	constexpr CompressionFunc GetCompressionFunction(Compression::CompressionAlgorithm func) {
		return NULL;
	}

	constexpr CompressionFunc GetDecompressionFunction(Compression::CompressionAlgorithm func) {
		return NULL;
	}


	bool Compression::Compress(uint8_t** dataPtr, uint32_t size, uint32_t* newSize) const {
		if (m_algorithm == CompressionAlgorithm::NONE)
			return true;

		CompressionFunc func = GetCompressionFunction(m_algorithm);
		if (!func)
			return false;

		func(dataPtr, size, newSize);
		return true;
	}

	bool Compression::Uncompress(uint8_t** dataPtr, uint32_t size, uint32_t* newSize) const {
		if (m_algorithm == CompressionAlgorithm::NONE)
			return true;

		CompressionFunc func = GetDecompressionFunction(m_algorithm);
		if (!func)
			return false;

		func(dataPtr, size, newSize);
		return true;
	}
}