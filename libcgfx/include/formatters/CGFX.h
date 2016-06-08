#pragma once

#include "../formats/CGFX.h"

namespace cgfx {

	class CGFX {
	private:
		CGFXData cgdata;
		bool hasLoaded = false;

	public:
		/*! \brief Return underlying CGFX data
		 *  \return CGFX struct containing all the parsed data
		 */
		const CGFXData& data() const { return cgdata; }

		/*! \brief Check if the CGFX has been loaded or not
		 *  \return True if the CGFX has been loaded and parsed, False otherwise
		 */
		bool loaded() { return hasLoaded; }

		/*! \brief Load and parse CGFX from byte buffer
		 *
		 *  \param data Buffer containing the raw CGFX data
		 *  \param size Size of the buffer (# of bytes)
		 *
		 *  \return true if the CGFX was successfully loaded and parsed, false otherwise
		 */
		bool loadFile(const uint8_t* data, const size_t size);
	};

}