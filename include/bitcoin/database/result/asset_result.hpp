//author chenxf  2016.11.03
#ifndef LIBBITCOIN_DATABASE_ASSET_RESULT_HPP
#define LIBBITCOIN_DATABASE_ASSET_RESULT_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/database/define.hpp>
#include <bitcoin/database/memory/memory.hpp>

namespace libbitcoin {

	///temporany for test....
	namespace chain
	{
		class asset {};
	}

	namespace database {

		/// Deferred read asset result.
		class BCD_API asset_result
		{
		public:
			asset_result(const memory_ptr slab);

			/// True if this asset result is valid (found).
			operator bool() const;

			/// The height of the block which includes the asset.
			size_t height() const;

			/// The position of the asset within its block.
			size_t index() const;

			/// The asset.
			chain::asset asset() const;

		private:
			const memory_ptr slab_;
		};

	} // namespace database
} // namespace libbitcoin

#endif
