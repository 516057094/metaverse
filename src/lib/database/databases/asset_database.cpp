
#include <bitcoin/database/databases/asset_database.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/database/memory/memory.hpp>
#include <bitcoin/database/result/asset_result.hpp>

namespace libbitcoin {
	namespace database {

		using namespace boost::filesystem;

		BC_CONSTEXPR size_t number_buckets = 100000000;
		BC_CONSTEXPR size_t header_size = slab_hash_table_header_size(number_buckets);
		BC_CONSTEXPR size_t initial_map_file_size = header_size + minimum_slabs_size;

		asset_database::asset_database(const path& map_filename,
			std::shared_ptr<shared_mutex> mutex)
			: lookup_file_(map_filename, mutex),
			lookup_header_(lookup_file_, number_buckets),
			lookup_manager_(lookup_file_, header_size),
			lookup_map_(lookup_header_, lookup_manager_)
		{
		}

		// Close does not call stop because there is no way to detect thread join.
		asset_database::~asset_database()
		{
			close();
		}

		// Create.
		// ----------------------------------------------------------------------------

		// Initialize files and start.
		bool asset_database::create()
		{
			// Resize and create require a started file.
			if (!lookup_file_.start())
				return false;

			// This will throw if insufficient disk space.
			lookup_file_.resize(initial_map_file_size);

			if (!lookup_header_.create() ||
				!lookup_manager_.create())
				return false;

			// Should not call start after create, already started.
			return
				lookup_header_.start() &&
				lookup_manager_.start();
		}

		// Startup and shutdown.
		// ----------------------------------------------------------------------------

		// Start files and primitives.
		bool asset_database::start()
		{
			return
				lookup_file_.start() &&
				lookup_header_.start() &&
				lookup_manager_.start();
		}

		// Stop files.
		bool asset_database::stop()
		{
			return lookup_file_.stop();
		}

		// Close files.
		bool asset_database::close()
		{
			return lookup_file_.close();
		}

		// ----------------------------------------------------------------------------

		asset_result asset_database::get(const hash_digest& hash) const
		{
			const auto memory = lookup_map_.find(hash);
			return asset_result(memory);
		}

		void asset_database::store(size_t height, size_t index,
			const chain::asset& tx)
		{
			// Write block data.
			const auto key = tx.hash();
			const auto tx_size = tx.serialized_size();

			BITCOIN_ASSERT(height <= max_uint32);
			const auto hight32 = static_cast<size_t>(height);

			BITCOIN_ASSERT(index <= max_uint32);
			const auto index32 = static_cast<size_t>(index);

			BITCOIN_ASSERT(tx_size <= max_size_t - 4 - 4);
			const auto value_size = 4 + 4 + static_cast<size_t>(tx_size);

			auto write = [&hight32, &index32, &tx](memory_ptr data)
			{
				auto serial = make_serializer(REMAP_ADDRESS(data));
				serial.write_4_bytes_little_endian(hight32);
				serial.write_4_bytes_little_endian(index32);
				serial.write_data(tx.to_data());
			};
			lookup_map_.store(key, write, value_size);
		}

		void asset_database::remove(const hash_digest& hash)
		{
			DEBUG_ONLY(bool success = ) lookup_map_.unlink(hash);
			BITCOIN_ASSERT(success);
		}

		void asset_database::sync()
		{
			lookup_manager_.sync();
		}

	} // namespace database
} // namespace libbitcoin
