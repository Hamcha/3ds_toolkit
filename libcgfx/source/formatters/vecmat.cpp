#include <formats/CGFX.h>
#include <utils/exceptions.h>
#include <utils/endian.h>

using namespace cgfx;

Vector3 Vector3::read(const uint8_t* data, const bool diffEndian) {
	Vector3 vec;
	memcpy(&vec.x, data, 4);
	memcpy(&vec.y, data + 4, 4);
	memcpy(&vec.z, data + 8, 4);
	if (diffEndian) {
		endianSwap(vec.x);
		endianSwap(vec.y);
		endianSwap(vec.z);
	}
	return vec;
}

Mat43 Mat43::read(const uint8_t* data, const bool diffEndian) {
	Mat43 mat;
	memcpy(&mat.a, data, 4 * (4 * 3));

	if (diffEndian) {
		for (uint8_t i = 0; i < 4 * 3; i++) {
			endianSwap(mat.a[i]);
		}
	}

	return mat;
}
