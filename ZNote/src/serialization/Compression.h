#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stdint.h>
#include <vector>


namespace app {

	

	class Compression {
		
	public:
		enum class CompressionAlgorithm : uint8_t { NONE = 0 };

		

		Compression(CompressionAlgorithm algo) : m_algorithm(algo) {}
		Compression() : Compression(CompressionAlgorithm::NONE) {}

		bool Compress(uint8_t** dataPtr, uint32_t size, uint32_t* newSize) const;
		bool Uncompress(uint8_t** dataPtr, uint32_t size, uint32_t* newSize) const;


	private:
		CompressionAlgorithm m_algorithm;
	};

}


#endif //COMPRESSION_H