#include "catch_amalgamated.hpp"

#include "Renderwerk/Core/Memory/Memory.hpp"

TEST_CASE("FMemory basic operations", "[memory]")
{
	SECTION("Allocate and Free")
	{
		// Test different sizes and alignments
		SECTION("Standard allocation")
		{
			constexpr size64 size = 128;
			constexpr size64 alignment = 8;

			void* ptr = FMemory::Allocate(size, alignment);
			REQUIRE(ptr != nullptr);
			REQUIRE(FMemory::IsValidPointer(ptr));

			// Check usable size is at least what we requested
			size64 allocSize = FMemory::GetAllocationSize(ptr);
			REQUIRE(allocSize >= size);

			// Write to memory to ensure it's usable
			memset(ptr, 0xAB, size);

			FMemory::Free(ptr, alignment);
		}

		SECTION("Zero size allocation")
		{
			void* ptr = FMemory::Allocate(0, 8);
			// Mimalloc may return a small allocation even for zero size
			if (ptr != nullptr)
			{
				REQUIRE(FMemory::IsValidPointer(ptr));
				FMemory::Free(ptr, 8);
			}
		}

		SECTION("Various alignments")
		{
			// Test common alignment values
			for (size64 alignment : {1, 2, 4, 8, 16, 32, 64, 128})
			{
				void* ptr = FMemory::Allocate(64, alignment);
				REQUIRE(ptr != nullptr);

				// Check alignment
				REQUIRE(((uintptr_t)ptr % alignment) == 0);

				FMemory::Free(ptr, alignment);
			}
		}
	}

	SECTION("Reallocate")
	{
		SECTION("Grow allocation")
		{
			constexpr size64 initialSize = 64;
			constexpr size64 newSize = 128;
			constexpr size64 alignment = 8;

			void* ptr = FMemory::Allocate(initialSize, alignment);
			REQUIRE(ptr != nullptr);

			// Write pattern to memory
			memset(ptr, 0xCD, initialSize);

			// Reallocate to a larger size
			void* newPtr = FMemory::Reallocate(ptr, newSize, alignment);
			REQUIRE(newPtr != nullptr);
			REQUIRE(FMemory::IsValidPointer(newPtr));

			// Verify original data is preserved
			unsigned char* bytePtr = static_cast<unsigned char*>(newPtr);
			for (size64 i = 0; i < initialSize; i++)
			{
				REQUIRE(bytePtr[i] == 0xCD);
			}

			// Check new size
			size64 allocSize = FMemory::GetAllocationSize(newPtr);
			REQUIRE(allocSize >= newSize);

			FMemory::Free(newPtr, alignment);
		}

		SECTION("Shrink allocation")
		{
			constexpr size64 initialSize = 256;
			constexpr size64 newSize = 64;
			constexpr size64 alignment = 8;

			void* ptr = FMemory::Allocate(initialSize, alignment);
			REQUIRE(ptr != nullptr);

			// Write pattern to memory (only to the portion we'll keep)
			memset(ptr, 0xEF, newSize);

			// Reallocate to a smaller size
			void* newPtr = FMemory::Reallocate(ptr, newSize, alignment);
			REQUIRE(newPtr != nullptr);
			REQUIRE(FMemory::IsValidPointer(newPtr));

			// Verify original data is preserved
			unsigned char* bytePtr = static_cast<unsigned char*>(newPtr);
			for (size64 i = 0; i < newSize; i++)
			{
				REQUIRE(bytePtr[i] == 0xEF);
			}

			FMemory::Free(newPtr, alignment);
		}

		SECTION("Reallocate null pointer")
		{
			constexpr size64 size = 64;
			constexpr size64 alignment = 8;

			// Reallocating nullptr should behave like Allocate
			void* ptr = FMemory::Reallocate(nullptr, size, alignment);
			REQUIRE(ptr != nullptr);
			REQUIRE(FMemory::IsValidPointer(ptr));

			FMemory::Free(ptr, alignment);
		}

		SECTION("Reallocate to zero size")
		{
			constexpr size64 initialSize = 64;
			constexpr size64 alignment = 8;

			void* ptr = FMemory::Allocate(initialSize, alignment);
			REQUIRE(ptr != nullptr);

			// Reallocating to zero might free or return a small allocation
			void* newPtr = FMemory::Reallocate(ptr, 0, alignment);
			if (newPtr != nullptr)
			{
				FMemory::Free(newPtr, alignment);
			}
		}
	}

	SECTION("IsValidPointer")
	{
		SECTION("Valid pointer")
		{
			void* ptr = FMemory::Allocate(64, 8);
			REQUIRE(ptr != nullptr);
			REQUIRE(FMemory::IsValidPointer(ptr));
			FMemory::Free(ptr, 8);
		}

		SECTION("Null pointer")
		{
			// Null pointer should not be valid
			REQUIRE_FALSE(FMemory::IsValidPointer(nullptr));
		}

		SECTION("Stack pointer")
		{
			// Stack pointers should not be managed by the allocator
			int stackVar = 42;
			REQUIRE_FALSE(FMemory::IsValidPointer(&stackVar));
		}
	}

	SECTION("GetAllocationSize")
	{
		SECTION("Standard allocation")
		{
			constexpr size64 requestedSize = 64;
			constexpr size64 alignment = 8;

			void* ptr = FMemory::Allocate(requestedSize, alignment);
			REQUIRE(ptr != nullptr);

			size64 allocSize = FMemory::GetAllocationSize(ptr);
			// The actual allocation size should be at least as large as requested
			REQUIRE(allocSize >= requestedSize);

			FMemory::Free(ptr, alignment);
		}

		SECTION("Null pointer")
		{
			// For null pointer, size should be 0
			REQUIRE(FMemory::GetAllocationSize(nullptr) == 0);
		}
	}

	SECTION("Multiple allocations")
	{
		// Allocate multiple blocks and ensure they don't overlap
		constexpr size64 blockSize = 128;
		constexpr size64 alignment = 16;
		constexpr int numBlocks = 10;

		void* blocks[numBlocks];

		// Allocate all blocks
		for (int i = 0; i < numBlocks; i++)
		{
			blocks[i] = FMemory::Allocate(blockSize, alignment);
			REQUIRE(blocks[i] != nullptr);
			REQUIRE(FMemory::IsValidPointer(blocks[i]));

			// Mark each block with a unique pattern
			memset(blocks[i], 0xA0 + i, blockSize);
		}

		// Verify content of each block
		for (int i = 0; i < numBlocks; i++)
		{
			unsigned char* bytePtr = static_cast<unsigned char*>(blocks[i]);
			for (size64 j = 0; j < blockSize; j++)
			{
				REQUIRE(bytePtr[j] == 0xA0 + i);
			}
		}

		// Free all blocks
		for (int i = 0; i < numBlocks; i++)
		{
			FMemory::Free(blocks[i], alignment);
		}
	}
}
