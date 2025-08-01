/* 
 * QR Code generator library (C++)
 * 
 * Copyright (c) Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/qr-code-generator-library
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */

#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <utility>
#include "qrcodegen.hpp"

using std::int8_t;
using std::uint8_t;
using std::size_t;
using std::vector;


namespace qrcodegen {

/*---- Class QrSegment ----*/

const QrSegment::Mode QrSegment::Mode::NUMERIC     (0x1, 10, 12, 14);
const QrSegment::Mode QrSegment::Mode::ALPHANUMERIC(0x2,  9, 11, 13);
const QrSegment::Mode QrSegment::Mode::BYTE        (0x4,  8, 16, 16);
const QrSegment::Mode QrSegment::Mode::KANJI       (0x8,  8, 10, 12);
const QrSegment::Mode QrSegment::Mode::ECI         (0x7,  0,  0,  0);


QrSegment::Mode::Mode(int mode, int cc0, int cc1, int cc2) :
	modeBits(mode),
	numBitsCharCount{cc0, cc1, cc2} {}


int QrSegment::Mode::getModeBits() const {
	return modeBits;
}


int QrSegment::Mode::getNumCharCountBits(int ver) const {
	assert(1 <= ver && ver <= 40);
	return numBitsCharCount[(ver + 7) / 17];
}


QrSegment QrSegment::makeBytes(const vector<uint8_t> &data) {
	vector<bool> bb;
	for (uint8_t b : data) {
		for (int i = 7; i >= 0; i--)
			bb.push_back(((b >> i) & 1) != 0);
	}
	return QrSegment(Mode::BYTE, static_cast<int>(data.size()), std::move(bb));
}


QrSegment QrSegment::makeNumeric(const char *digits) {
	if (!isNumeric(digits))
		throw std::domain_error("String contains non-numeric characters");
	
	vector<bool> bb;
	int accumData = 0;
	int accumCount = 0;
	for (; *digits != '\0'; digits++) {
		char c = *digits;
		accumData = accumData * 10 + (c - '0');
		accumCount++;
		if (accumCount == 3) {
			for (int i = 9; i >= 0; i--)
				bb.push_back(((accumData >> i) & 1) != 0);
			accumData = 0;
			accumCount = 0;
		}
	}
	if (accumCount > 0) {  // 1 or 2 digits remaining
		for (int i = accumCount * 3 + 0; i >= 0; i--)
			bb.push_back(((accumData >> i) & 1) != 0);
	}
	return QrSegment(Mode::NUMERIC, static_cast<int>(std::strlen(digits)), std::move(bb));
}


QrSegment QrSegment::makeAlphanumeric(const char *text) {
	if (!isAlphanumeric(text))
		throw std::domain_error("String contains unencodable characters in alphanumeric mode");
	
	vector<bool> bb;
	int accumData = 0;
	int accumCount = 0;
	for (; *text != '\0'; text++) {
		const char *temp = std::strchr(ALPHANUMERIC_CHARSET, *text);
		accumData = accumData * 45 + static_cast<int>(temp - ALPHANUMERIC_CHARSET);
		accumCount++;
		if (accumCount == 2) {
			for (int i = 10; i >= 0; i--)
				bb.push_back(((accumData >> i) & 1) != 0);
			accumData = 0;
			accumCount = 0;
		}
	}
	if (accumCount > 0) {  // 1 character remaining
		for (int i = 5; i >= 0; i--)
			bb.push_back(((accumData >> i) & 1) != 0);
	}
	return QrSegment(Mode::ALPHANUMERIC, static_cast<int>(std::strlen(text)), std::move(bb));
}


vector<QrSegment> QrSegment::makeSegments(const char *text) {
	// Select the most efficient segment encoding automatically
	vector<QrSegment> result;
	if (*text == '\0');  // Leave result empty
	else if (isNumeric(text))
		result.push_back(makeNumeric(text));
	else if (isAlphanumeric(text))
		result.push_back(makeAlphanumeric(text));
	else {
		vector<uint8_t> bytes;
		for (; *text != '\0'; text++)
			bytes.push_back(static_cast<uint8_t>(*text));
		result.push_back(makeBytes(bytes));
	}
	return result;
}


QrSegment QrSegment::makeEci(long assignVal) {
	vector<bool> bb;
	if (0 <= assignVal && assignVal < (1 << 7)) {
		for (int i = 7; i >= 0; i--)
			bb.push_back(((assignVal >> i) & 1) != 0);
	} else if ((1 << 7) <= assignVal && assignVal < (1 << 14)) {
		bb.push_back(true);
		bb.push_back(false);
		for (int i = 13; i >= 0; i--)
			bb.push_back(((assignVal >> i) & 1) != 0);
	} else if ((1 << 14) <= assignVal && assignVal < 1000000L) {
		bb.push_back(true);
		bb.push_back(true);
		for (int i = 19; i >= 0; i--)
			bb.push_back(((assignVal >> i) & 1) != 0);
	} else
		throw std::domain_error("ECI assignment value out of range");
	return QrSegment(Mode::ECI, 0, std::move(bb));
}


QrSegment QrSegment::makeSegment(const Mode &md, int numCh, const vector<bool> &data) {
	return QrSegment(md, numCh, data);
}


QrSegment::QrSegment(const Mode &md, int numCh, const vector<bool> &dt) :
		mode(md),
		numChars(numCh),
		data(dt) {
	if (numCh < 0)
		throw std::domain_error("Invalid value");
}


QrSegment::QrSegment(const Mode &md, int numCh, vector<bool> &&dt) :
		mode(md),
		numChars(numCh),
		data(std::move(dt)) {
	if (numCh < 0)
		throw std::domain_error("Invalid value");
}


int QrSegment::getTotalBits(const vector<QrSegment> &segs, int version) {
	long result = 0;
	for (const QrSegment &seg : segs) {
		int ccbits = seg.mode.getNumCharCountBits(version);
		if (seg.numChars >= (1L << ccbits))
			return -1;  // The segment's length doesn't fit the field's bit width
		result += 4L + ccbits + seg.data.size();
		if (result > INT_MAX)
			return -1;  // The sum is too long to fit in an int type
	}
	return static_cast<int>(result);
}


bool QrSegment::isNumeric(const char *text) {
	for (; *text != '\0'; text++) {
		if (*text < '0' || *text > '9')
			return false;
	}
	return true;
}


bool QrSegment::isAlphanumeric(const char *text) {
	for (; *text != '\0'; text++) {
		if (std::strchr(ALPHANUMERIC_CHARSET, *text) == nullptr)
			return false;
	}
	return true;
}


const QrSegment::Mode &QrSegment::getMode() const {
	return mode;
}


int QrSegment::getNumChars() const {
	return numChars;
}


const vector<bool> &QrSegment::getData() const {
	return data;
}


const char *QrSegment::ALPHANUMERIC_CHARSET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";



/*---- Class QrCode ----*/

int QrCode::getFormatBits(Ecc ecl) {
	switch (ecl) {
		case Ecc::LOW_ECC     :  return 1;
		case Ecc::MEDIUM_ECC  :  return 0;
		case Ecc::QUARTILE_ECC:  return 3;
		case Ecc::HIGH_ECC    :  return 2;
		default:  throw std::logic_error("Unreachable");
	}
}


QrCode QrCode::encodeText(const char *text, Ecc ecl) {
	vector<QrSegment> segs = QrSegment::makeSegments(text);
	return encodeSegments(segs, ecl);
}


QrCode QrCode::encodeBinary(const vector<uint8_t> &data, Ecc ecl) {
	vector<QrSegment> segs{QrSegment::makeBytes(data)};
	return encodeSegments(segs, ecl);
}


QrCode QrCode::encodeSegments(const vector<QrSegment> &segs, Ecc ecl,
		int minVersion, int maxVersion, int mask, bool boostEcl) {
	if (!(MIN_VERSION <= minVersion && minVersion <= maxVersion && maxVersion <= MAX_VERSION) || mask < -1 || mask > 7)
		throw std::invalid_argument("Invalid value");
	
	// Find the minimal version number to use
	int version, dataUsedBits;
	for (version = minVersion; ; version++) {
		int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;  // Number of data bits available
		dataUsedBits = QrSegment::getTotalBits(segs, version);
		if (dataUsedBits != -1 && dataUsedBits <= dataCapacityBits)
			break;  // This version number is found to be suitable
		if (version >= maxVersion) {  // All versions in the range could not fit the given data
			std::ostringstream sb;
			if (dataUsedBits == -1)
				sb << "Segment too long";
			else {
				sb << "Data length = " << dataUsedBits << " bits, ";
				sb << "Max capacity = " << dataCapacityBits << " bits";
			}
			throw std::invalid_argument(sb.str());
		}
	}
	assert(dataUsedBits != -1);
	
	// Increase the error correction level while the data still fits in the same version number
	for (Ecc newEcl : {Ecc::MEDIUM_ECC, Ecc::QUARTILE_ECC, Ecc::HIGH_ECC}) {  // From low to high
		if (boostEcl && dataUsedBits <= getNumDataCodewords(version, newEcl) * 8)
			ecl = newEcl;
	}
	
	// Concatenate all segments to create the data bit string
	vector<bool> bb;
	for (const QrSegment &seg : segs) {
		for (int i = 3; i >= 0; i--)
			bb.push_back(((seg.getMode().getModeBits() >> i) & 1) != 0);
		int ccbits = seg.getMode().getNumCharCountBits(version);
		for (int i = ccbits - 1; i >= 0; i--)
			bb.push_back(((seg.getNumChars() >> i) & 1) != 0);
		for (bool b : seg.getData())
			bb.push_back(b);
	}
	assert(bb.size() == static_cast<unsigned int>(dataUsedBits));
	
	// Add terminator and pad up to a byte if applicable
	int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;
	assert(bb.size() <= static_cast<unsigned int>(dataCapacityBits));
	for (int i = 0; i < 4 && bb.size() < static_cast<unsigned int>(dataCapacityBits); i++)
		bb.push_back(false);
	while (bb.size() % 8 != 0)
		bb.push_back(false);
	assert(bb.size() % 8 == 0);
	
	// Pad with alternating bytes until data capacity is reached
	for (uint8_t padByte = 0xEC; bb.size() < static_cast<unsigned int>(dataCapacityBits); padByte ^= 0xEC ^ 0x11) {
		for (int i = 7; i >= 0; i--)
			bb.push_back(((padByte >> i) & 1) != 0);
	}
	
	// Pack bits into bytes in big endian
	vector<uint8_t> dataCodewords;
	for (size_t i = 0; i < bb.size(); i += 8) {
		uint8_t byte = 0;
		for (int j = 0; j < 8; j++)
			byte |= (bb.at(i + j) ? 1 : 0) << (7 - j);
		dataCodewords.push_back(byte);
	}
	
	// Create the QR Code object
	return QrCode(version, ecl, dataCodewords, mask);
}


QrCode::QrCode(int ver, Ecc ecl, const vector<uint8_t> &dataCodewords, int msk) :
		// Initialize fields and check arguments
		version(ver),
		errorCorrectionLevel(ecl) {
	if (ver < MIN_VERSION || ver > MAX_VERSION)
		throw std::domain_error("Version value out of range");
	if (msk < -1 || msk > 7)
		throw std::domain_error("Mask value out of range");
	size = ver * 4 + 17;
	size_t sz = static_cast<size_t>(size);
	modules    = vector<vector<bool> >(sz, vector<bool>(sz));  // Initially all light
	isFunction = vector<vector<bool> >(sz, vector<bool>(sz));
	
	// Compute ECC, draw modules
	drawFunctionPatterns();
	const vector<uint8_t> allCodewords = addEccAndInterleave(dataCodewords);
	drawCodewords(allCodewords);
	
	// Do masking
	if (msk == -1) {  // Automatically choose best mask
		long minPenalty = LONG_MAX;
		for (int i = 0; i < 8; i++) {
			applyMask(i);
			drawFormatBits(i);
			long penalty = getPenaltyScore();
			if (penalty < minPenalty) {
				msk = i;
				minPenalty = penalty;
			}
			applyMask(i);  // Undoes the mask due to XOR
		}
	}
	assert(0 <= msk && msk <= 7);
	mask = msk;
	applyMask(msk);  // Apply the final choice of mask
	drawFormatBits(msk);  // Overwrite old format bits
	
	isFunction.clear();
	isFunction.shrink_to_fit();
}


int QrCode::getVersion() const {
	return version;
}


int QrCode::getSize() const {
	return size;
}


QrCode::Ecc QrCode::getErrorCorrectionLevel() const {
	return errorCorrectionLevel;
}


int QrCode::getMask() const {
	return mask;
}


bool QrCode::getModule(int x, int y) const {
	return 0 <= x && x < size && 0 <= y && y < size && module(x, y);
}


void QrCode::drawFunctionPatterns() {
	// Draw horizontal and vertical timing patterns
	for (int i = 0; i < size; i++) {
		setFunctionModule(6, i, i % 2 == 0);
		setFunctionModule(i, 6, i % 2 == 0);
	}
	
	// Draw 3 finder patterns (all corners except bottom right; overwrites some timing modules)
	drawFinderPattern(3, 3);
	drawFinderPattern(size - 4, 3);
	drawFinderPattern(3, size - 4);
	
	// Draw numerous alignment patterns
	const vector<int> alignPatPos = getAlignmentPatternPositions();
	size_t numAlign = alignPatPos.size();
	for (size_t i = 0; i < numAlign; i++) {
		for (size_t j = 0; j < numAlign; j++) {
			// Don't draw on the three finder corners
			if (!((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0)))
				drawAlignmentPattern(alignPatPos.at(i), alignPatPos.at(j));
		}
	}
	
	// Draw configuration data
	drawFormatBits(0);  // Dummy mask value; overwritten later in the constructor
	drawVersion();
}


void QrCode::drawFormatBits(int msk) {
	// Calculate error correction code and pack bits
	int data = getFormatBits(errorCorrectionLevel) << 3 | msk;  // errCorrLvl is uint2, msk is uint3
	int rem = data;
	for (int i = 0; i < 10; i++)
		rem = (rem << 1) ^ ((rem >> 9) * 0x537);
	int bits = (data << 10 | rem) ^ 0x5412;  // uint15
	assert(bits >> 15 == 0);
	
	// Draw first copy
	for (int i = 0; i <= 5; i++)
		setFunctionModule(8, i, getBit(bits, i));
	setFunctionModule(8, 7, getBit(bits, 6));
	setFunctionModule(8, 8, getBit(bits, 7));
	setFunctionModule(7, 8, getBit(bits, 8));
	for (int i = 9; i < 15; i++)
		setFunctionModule(14 - i, 8, getBit(bits, i));
	
	// Draw second copy
	for (int i = 0; i < 8; i++)
		setFunctionModule(size - 1 - i, 8, getBit(bits, i));
	for (int i = 8; i < 15; i++)
		setFunctionModule(8, size - 15 + i, getBit(bits, i));
	setFunctionModule(8, size - 8, true);  // Always dark
}


void QrCode::drawVersion() {
	if (version < 7)
		return;
	
	// Calculate error correction code and pack bits
	int rem = version;  // version is uint6, in the range [7, 40]
	for (int i = 0; i < 12; i++)
		rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
	long bits = static_cast<long>(version) << 12 | rem;  // uint18
	assert(bits >> 18 == 0);
	
	// Draw two copies
	for (int i = 0; i < 18; i++) {
		bool bit = getBit(bits, i);
		int a = size - 11 + i % 3;
		int b = i / 3;
		setFunctionModule(a, b, bit);
		setFunctionModule(b, a, bit);
	}
}


void QrCode::drawFinderPattern(int x, int y) {
	for (int dy = -4; dy <= 4; dy++) {
		for (int dx = -4; dx <= 4; dx++) {
			int dist = std::max(std::abs(dx), std::abs(dy));  // Chebyshev/infinity norm
			int xx = x + dx, yy = y + dy;
			if (0 <= xx && xx < size && 0 <= yy && yy < size)
				setFunctionModule(xx, yy, dist != 2 && dist != 4);
		}
	}
}


void QrCode::drawAlignmentPattern(int x, int y) {
	for (int dy = -2; dy <= 2; dy++) {
		for (int dx = -2; dx <= 2; dx++)
			setFunctionModule(x + dx, y + dy, std::max(std::abs(dx), std::abs(dy)) != 1);
	}
}


void QrCode::setFunctionModule(int x, int y, bool isDark) {
	size_t ux = static_cast<size_t>(x);
	size_t uy = static_cast<size_t>(y);
	modules   .at(uy).at(ux) = isDark;
	isFunction.at(uy).at(ux) = true;
}


bool QrCode::module(int x, int y) const {
	return modules.at(static_cast<size_t>(y)).at(static_cast<size_t>(x));
}


vector<uint8_t> QrCode::addEccAndInterleave(const vector<uint8_t> &data) const {
	if (data.size() != static_cast<unsigned int>(getNumDataCodewords(version, errorCorrectionLevel)))
		throw std::invalid_argument("Invalid argument");
	
	// Calculate parameter numbers
	int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[static_cast<int>(errorCorrectionLevel)][version];
	int blockEccLen = ECC_CODEWORDS_PER_BLOCK  [static_cast<int>(errorCorrectionLevel)][version];
	int rawCodewords = getNumRawDataModules(version) / 8;
	int numShortBlocks = numBlocks - rawCodewords % numBlocks;
	int shortBlockLen = rawCodewords / numBlocks;
	
	// Split data into blocks and append ECC to each block
	vector<vector<uint8_t> > blocks;
	const vector<uint8_t> rsDiv = reedSolomonComputeDivisor(blockEccLen);
	for (int i = 0, k = 0; i < numBlocks; i++) {
		vector<uint8_t> dat(data.cbegin() + k, data.cbegin() + (k + shortBlockLen - blockEccLen + (i < numShortBlocks ? 0 : 1)));
		k += static_cast<int>(dat.size());
		const vector<uint8_t> ecc = reedSolomonComputeRemainder(dat, rsDiv);
		if (i < numShortBlocks)
			dat.push_back(0);
		dat.insert(dat.end(), ecc.cbegin(), ecc.cend());
		blocks.push_back(std::move(dat));
	}
	
	// Interleave (not concatenate) the bytes from every block into a single sequence
	vector<uint8_t> result;
	for (size_t i = 0; i < blocks.at(0).size(); i++) {
		for (size_t j = 0; j < blocks.size(); j++) {
			// Skip the padding byte in short blocks
			if (i != static_cast<unsigned int>(shortBlockLen - blockEccLen) || j >= static_cast<unsigned int>(numShortBlocks))
				result.push_back(blocks.at(j).at(i));
		}
	}
	assert(result.size() == static_cast<unsigned int>(rawCodewords));
	return result;
}


void QrCode::drawCodewords(const vector<uint8_t> &data) {
	if (data.size() != static_cast<unsigned int>(getNumRawDataModules(version) / 8))
		throw std::invalid_argument("Invalid argument");
	
	size_t i = 0;  // Bit index into the data
	// Do the funny zigzag scan
	for (int right = size - 1; right >= 1; right -= 2) {  // Index of right column in each column pair
		if (right == 6)
			right = 5;
		for (int vert = 0; vert < size; vert++) {  // Vertical counter
			for (int j = 0; j < 2; j++) {
				size_t x = static_cast<size_t>(right - j);  // Actual x coordinate
				bool upward = ((right + 1) & 2) == 0;
				size_t y = static_cast<size_t>(upward ? size - 1 - vert : vert);  // Actual y coordinate
				if (!isFunction.at(y).at(x) && i < data.size() * 8) {
					modules.at(y).at(x) = getBit(data.at(i >> 3), 7 - (i & 7));
					i++;
				}
				// If this QR Code has any remainder bits (0 to 7), they were assigned as
				// 0/false/light by the constructor and are left unchanged by this method
			}
		}
	}
	assert(i == data.size() * 8);
}


void QrCode::applyMask(int msk) {
	if (msk < 0 || msk > 7)
		throw std::domain_error("Mask value out of range");
	size_t sz = static_cast<size_t>(size);
	for (size_t y = 0; y < sz; y++) {
		for (size_t x = 0; x < sz; x++) {
			if (!isFunction.at(y).at(x)) {
				bool invert;
				switch (msk) {
					case 0:  invert = (x + y) % 2 == 0;                          break;
					case 1:  invert = y % 2 == 0;                                break;
					case 2:  invert = x % 3 == 0;                                break;
					case 3:  invert = (x + y) % 3 == 0;                          break;
					case 4:  invert = (x / 3 + y / 2) % 2 == 0;                  break;
					case 5:  invert = x * y % 2 + x * y % 3 == 0;                break;
					case 6:  invert = (x * y % 2 + x * y % 3) % 2 == 0;          break;
					case 7:  invert = ((x + y) % 2 + x * y % 3) % 2 == 0;        break;
					default:  throw std::logic_error("Unreachable");
				}
				modules.at(y).at(x) = modules.at(y).at(x) ^ invert;
			}
		}
	}
}


long QrCode::getPenaltyScore() const {
	long result = 0;
	
	// Adjacent modules in row having same color, and finder-like patterns
	for (int y = 0; y < size; y++) {
		bool runColor = false;
		int runX = 0;
		std::array<int,7> runHistory = {};
		for (int x = 0; x < size; x++) {
			if (module(x, y) == runColor) {
				runX++;
				if (runX == 5)
					result += PENALTY_N1;
				else if (runX > 5)
					result++;
			} else {
				finderPenaltyAddHistory(runX, runHistory);
				if (!runColor)
					result += finderPenaltyCountPatterns(runHistory) * PENALTY_N3;
				runColor = module(x, y);
				runX = 1;
			}
		}
		result += finderPenaltyTerminateAndCount(runColor, runX, runHistory) * PENALTY_N3;
	}
	// Adjacent modules in column having same color, and finder-like patterns
	for (int x = 0; x < size; x++) {
		bool runColor = false;
		int runY = 0;
		std::array<int,7> runHistory = {};
		for (int y = 0; y < size; y++) {
			if (module(x, y) == runColor) {
				runY++;
				if (runY == 5)
					result += PENALTY_N1;
				else if (runY > 5)
					result++;
			} else {
				finderPenaltyAddHistory(runY, runHistory);
				if (!runColor)
					result += finderPenaltyCountPatterns(runHistory) * PENALTY_N3;
				runColor = module(x, y);
				runY = 1;
			}
		}
		result += finderPenaltyTerminateAndCount(runColor, runY, runHistory) * PENALTY_N3;
	}
	
	// 2*2 blocks of modules having same color
	for (int y = 0; y < size - 1; y++) {
		for (int x = 0; x < size - 1; x++) {
			bool  color = module(x, y);
			if (  color == module(x + 1, y) &&
			      color == module(x, y + 1) &&
			      color == module(x + 1, y + 1))
				result += PENALTY_N2;
		}
	}
	
	// Balance of dark and light modules
	int dark = 0;
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			if (module(x, y))
				dark++;
		}
	}
	int total = size * size;  // Note that size is odd, so dark/total != 1/2
	// Compute the smallest integer k >= 0 such that (45-5k)% <= dark/total <= (55+5k)%
	int k = static_cast<int>((std::abs(dark * 20L - total * 10L) + total - 1) / total) - 1;
	assert(0 <= k && k <= 9);
	result += k * PENALTY_N4;
	assert(0 <= result && result <= 2568888L);  // Non-tight upper bound based on default values of PENALTY_N1, ..., N4
	return result;
}


vector<int> QrCode::getAlignmentPatternPositions() const {
	if (version == 1)
		return vector<int>();
	else {
		int numAlign = version / 7 + 2;
		int step = (version == 32) ? 26 : (version*4 + numAlign*2 + 1) / (numAlign*2 - 2) * 2;
		vector<int> result;
		for (int i = 0, pos = size - 7; i < numAlign - 1; i++, pos -= step)
			result.insert(result.begin(), pos);
		result.insert(result.begin(), 6);
		return result;
	}
}


int QrCode::getNumRawDataModules(int ver) {
	if (ver < MIN_VERSION || ver > MAX_VERSION)
		throw std::domain_error("Version number out of range");
	int result = (16 * ver + 128) * ver + 64;
	if (ver >= 2) {
		int numAlign = ver / 7 + 2;
		result -= (25 * numAlign - 10) * numAlign - 55;
		if (ver >= 7)
			result -= 36;
	}
	assert(208 <= result && result <= 29648);
	return result;
}


int QrCode::getNumDataCodewords(int ver, Ecc ecl) {
	return getNumRawDataModules(ver) / 8
		- ECC_CODEWORDS_PER_BLOCK    [static_cast<int>(ecl)][ver]
		* NUM_ERROR_CORRECTION_BLOCKS[static_cast<int>(ecl)][ver];
}


vector<uint8_t> QrCode::reedSolomonComputeDivisor(int degree) {
	if (degree < 1 || degree > 255)
		throw std::domain_error("Degree out of range");
	// Polynomial coefficients are stored from highest to lowest power, excluding the leading term which is always 1.
	// For example the polynomial x^3 + 255x^2 + 8x + 93 is stored as the uint8 array {255, 8, 93}.
	vector<uint8_t> result(static_cast<size_t>(degree));
	result.at(degree - 1) = 1;  // Start off with the monomial x^0
	
	// Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
	// and drop the highest monomial term which is always 1x^degree.
	// Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
	uint8_t root = 1;
	for (int i = 0; i < degree; i++) {
		// Multiply the current product by (x - r^i)
		for (int j = 0; j < degree; j++) {
			result.at(j) = reedSolomonMultiply(result.at(j), root);
			if (j + 1 < degree)
				result.at(j) ^= result.at(j + 1);
		}
		root = reedSolomonMultiply(root, 0x02);
	}
	return result;
}


vector<uint8_t> QrCode::reedSolomonComputeRemainder(const vector<uint8_t> &data, const vector<uint8_t> &divisor) {
	vector<uint8_t> result(divisor.size());
	for (uint8_t b : data) {  // Polynomial division
		uint8_t factor = b ^ result.at(0);
		result.erase(result.begin());
		result.push_back(0);
		for (size_t i = 0; i < result.size(); i++)
			result.at(i) ^= reedSolomonMultiply(divisor.at(i), factor);
	}
	return result;
}


uint8_t QrCode::reedSolomonMultiply(uint8_t x, uint8_t y) {
	// Russian peasant multiplication
	uint8_t z = 0;
	for (int i = 7; i >= 0; i--) {
		z = static_cast<uint8_t>((z << 1) ^ ((z >> 7) * 0x11D));
		z ^= ((y >> i) & 1) * x;
	}
	return z;
}


int QrCode::finderPenaltyCountPatterns(const std::array<int,7> &runHistory) const {
	int n = runHistory[1];
	assert(n <= size * 3);
	bool core = n > 0 && runHistory[2] == n && runHistory[3] == 3 * n && runHistory[4] == n && runHistory[5] == n;
	return (core && runHistory[0] >= 4 * n && runHistory[6] >= n ? 1 : 0)
	     + (core && runHistory[6] >= 4 * n && runHistory[0] >= n ? 1 : 0);
}


int QrCode::finderPenaltyTerminateAndCount(bool currentRunColor, int currentRunLength, std::array<int,7> &runHistory) const {
	if (currentRunColor) {  // Terminate dark run
		finderPenaltyAddHistory(currentRunLength, runHistory);
		currentRunLength = 0;
	}
	currentRunLength += size;  // Add light border to final run
	finderPenaltyAddHistory(currentRunLength, runHistory);
	return finderPenaltyCountPatterns(runHistory);
}


void QrCode::finderPenaltyAddHistory(int currentRunLength, std::array<int,7> &runHistory) const {
	if (runHistory[0] == 0)
		currentRunLength += size;  // Add light border to initial run
	std::copy_backward(runHistory.cbegin(), runHistory.cend() - 1, runHistory.end());
	runHistory[0] = currentRunLength;
}


bool QrCode::getBit(long x, int i) {
	return ((x >> i) & 1) != 0;
}


/*---- Tables of constants ----*/

const int QrCode::PENALTY_N1 =  3;
const int QrCode::PENALTY_N2 =  3;
const int QrCode::PENALTY_N3 = 40;
const int QrCode::PENALTY_N4 = 10;


const int8_t QrCode::ECC_CODEWORDS_PER_BLOCK[4][41] = {
	// Version: (note that index 0 is for padding, and is set to an illegal value)
	//0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
	{-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Low
	{-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},  // Medium
	{-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Quartile
	{-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // High
};

const int8_t QrCode::NUM_ERROR_CORRECTION_BLOCKS[4][41] = {
	// Version: (note that index 0 is for padding, and is set to an illegal value)
	//0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
	{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Low
	{-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Medium
	{-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Quartile
	{-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // High
};

}
