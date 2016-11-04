// author chenxf	2016.11.03

#ifndef LIBBITCOIN_DATABASE_ASSET_DATABASE_HPP
#define LIBBITCOIN_DATABASE_ASSET_DATABASE_HPP

#include <memory>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/database/define.hpp>
#include <bitcoin/database/memory/memory_map.hpp>
#include <bitcoin/database/result/asset_result.hpp>
#include <bitcoin/database/primitives/slab_hash_table.hpp>
#include <bitcoin/database/primitives/slab_manager.hpp>

namespace libbitcoin {
	namespace database {

		/// This enables lookups of asset by hash.
		class BCD_API asset_database
		{
		public:
			/// Construct the database.
			asset_database(const boost::filesystem::path& map_filename,
				std::shared_ptr<shared_mutex> mutex = nullptr);

			/// Close the database (all threads must first be stopped).
			~asset_database();

			/// Initialize a new asset database.
			bool create();

			/// Call before using the database.
			bool start();

			/// Call to signal a stop of current operations.
			bool stop();

			/// Call to unload the memory map.
			bool close();

			/// Fetch asset from its hash.
			asset_result get(const hash_digest& hash) const;

			/// Store a asset in the database. Returns a unique index
			/// which can be used to reference the asset.
			void store(size_t height, size_t index, const chain::asset& tx);

			/// Delete a asset from database.
			void remove(const hash_digest& hash);

			/// Synchronise storage with disk so things are consistent.
			/// Should be done at the end of every block write.
			void sync();

		private:
			typedef slab_hash_table<hash_digest> slab_map;

			// Hash table used for looking up asset by hash.
			memory_map lookup_file_;
			slab_hash_table_header lookup_header_;
			slab_manager lookup_manager_;
			slab_map lookup_map_;
		};

	} // namespace database
} // namespace libbitcoin

#endif
