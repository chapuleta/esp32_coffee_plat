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

#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>


namespace qrcodegen {

/* 
 * A segment of character/binary/control data in a QR Code symbol.
 * Instances of this class are immutable.
 * The mid-level way to create a segment is to take the payload data
 * and call a static factory function such as QrSegment::makeNumeric().
 * The low-level way to create a segment is to custom-make the bit vector
 * and call the QrSegment() constructor with appropriate values.
 * This segment class imposes no length restrictions, but QR Codes have restrictions.
 * Even in the most favorable conditions, a QR Code can only hold 7089 characters of data.
 * Any segment longer than this is guaranteed to fail. But smaller segments may also
 * fail depending on the encoding and error correction level.
 */
class QrSegment final {
	
	/*---- Public helper enumeration ----*/
	
	/* 
	 * Describes precisely all strings that are encodable in numeric mode.
	 * To test whether a string is encodable, call QrSegment::isNumeric().
	 * A string is encodable iff each character is in the range 0 to 9.
	 */
	public: class Mode final {
		
		/*---- Constants ----*/
		
		public: static const Mode NUMERIC;
		public: static const Mode ALPHANUMERIC;
		public: static const Mode BYTE;
		public: static const Mode KANJI;
		public: static const Mode ECI;
		
		
		/*---- Instance fields ----*/
		
		private: const int modeBits;
		
		private: const int numBitsCharCount[3];
		
		
		/*---- Constructor ----*/
		
		private: Mode(int mode, int cc0, int cc1, int cc2);
		
		
		/*---- Instance methods ----*/
		
		/* 
		 * (Package-private) Returns the mode indicator bits, which is an unsigned 4-bit value (range 0 to 15).
		 */
		public: int getModeBits() const;
		
		
		/* 
		 * (Package-private) Returns the bit width of the character count field for a segment in
		 * this mode in a QR Code at the given version number. The result is in the range [0, 16].
		 */
		public: int getNumCharCountBits(int ver) const;
		
	};
	
	
	
	/*---- Static factory functions (mid level) ----*/
	
	/* 
	 * Returns a segment representing the given binary data encoded in
	 * byte mode. All input byte vectors are acceptable. Any text string
	 * can be converted to UTF-8 bytes and encoded as a byte mode segment.
	 */
	public: static QrSegment makeBytes(const std::vector<std::uint8_t> &data);
	
	
	/* 
	 * Returns a segment representing the given string of decimal digits encoded in numeric mode.
	 * The string must consist of only the characters 0 to 9, otherwise an exception is thrown.
	 */
	public: static QrSegment makeNumeric(const char *digits);
	
	
	/* 
	 * Returns a segment representing the given text string encoded in alphanumeric mode.
	 * The characters allowed are: 0 to 9, A to Z (uppercase only), space,
	 * dollar, percent, asterisk, plus, hyphen, period, slash, colon.
	 * The string must not contain any other characters, otherwise an exception is thrown.
	 */
	public: static QrSegment makeAlphanumeric(const char *text);
	
	
	/* 
	 * Returns a list of zero or more segments to represent the given text string. The result
	 * may use various segment modes and switch modes to optimize the length of the bit stream.
	 */
	public: static std::vector<QrSegment> makeSegments(const char *text);
	
	
	/* 
	 * Returns a segment representing an Extended Channel Interpretation
	 * (ECI) designator with the given assignment value.
	 */
	public: static QrSegment makeEci(long assignVal);
	
	
	/*---- Static factory functions (low level) ----*/
	
	/* 
	 * Returns a segment with the given values. The character count (numChars) must agree with the
	 * mode and the bit buffer length, but the constraint isn't checked. The given bit buffer is copied
	 * and stored. This function can be used to generate custom segments for any mode (including modes
	 * not supported by the QrSegment factory functions).
	 */
	public: static QrSegment makeSegment(const Mode &mode, int numChars, const std::vector<bool> &data);
	
	
	/*---- Instance fields ----*/
	
	/* The mode indicator of this segment. Not null. */
	private: Mode mode;
	
	/* The length of this segment's unencoded data. Measured in characters for
	 * numeric/alphanumeric/kanji mode, bytes for byte mode, and 0 for ECI mode.
	 * Always zero or positive. Not the same as the data's bit length.
	 * Immutable after construction. */
	private: int numChars;
	
	/* The data bits of this segment. Not null. Immutable after construction. */
	private: std::vector<bool> data;
	
	
	/*---- Constructors ----*/
	
	/* 
	 * Creates a new QR Code segment with the given attributes and data.
	 * The character count (numChars) must agree with the mode and the bit buffer length,
	 * but the constraint isn't checked. The given bit buffer is copied and stored.
	 */
	public: QrSegment(const Mode &md, int numCh, const std::vector<bool> &dt);
	
	
	/* 
	 * Creates a new QR Code segment with the given parameters and data.
	 * The character count (numChars) must agree with the mode and the bit buffer length,
	 * but the constraint isn't checked. The given bit buffer is moved and stored.
	 */
	public: QrSegment(const Mode &md, int numCh, std::vector<bool> &&dt);
	
	
	/*---- Instance methods ----*/
	
	/* 
	 * Returns the mode field of this segment.
	 */
	public: const Mode &getMode() const;
	
	
	/* 
	 * Returns the character count field of this segment.
	 */
	public: int getNumChars() const;
	
	
	/* 
	 * Returns the data bits of this segment.
	 */
	public: const std::vector<bool> &getData() const;
	
	
	// (Package-private) Calculates and returns the number of bits needed to encode the given segments at
	// the given version. The result is infinity if a segment has too many characters to fit its length field.
	public: static int getTotalBits(const std::vector<QrSegment> &segs, int version);
	
	
	/*---- Private constant ----*/
	
	/* The set of all legal characters in alphanumeric mode,
	 * where each character value maps to the index in the string. */
	private: static const char *ALPHANUMERIC_CHARSET;
	
	
	/*---- Private stateless helper functions ----*/
	
	private: static bool isNumeric(const char *text);
	
	private: static bool isAlphanumeric(const char *text);
	
};



/* 
 * A QR Code symbol, which is a type of two-dimension barcode.
 * Invented by Denso Wave and described in the ISO/IEC 18004 standard.
 * Instances of this class represent an immutable square grid of dark and light cells.
 * The class provides static factory functions to create a QR Code from text or binary data.
 * The class covers the QR Code Model 2 specification, supporting all versions (sizes)
 * from 1 to 40, all 4 error correction levels, and 4 character encoding modes.
 * 
 * Ways to create a QR Code object:
 * - High level: Take the payload data and call QrCode::encodeText() or QrCode::encodeBinary().
 * - Mid level: Custom-make the list of segments and call QrCode::encodeSegments().
 * - Low level: Custom-make the array of data codeword bytes (including
 *   segment headers and final padding, excluding error correction codewords),
 *   supply the appropriate version number, and call the QrCode() constructor.
 * (Note that all ways require supplying the desired error correction level.)
 */
class QrCode final {
	
	/*---- Public helper enumeration ----*/
	
	/* 
	 * The error correction level in a QR Code symbol.
	 */
	public: enum class Ecc {
		LOW_ECC      = 0,  // The QR Code can tolerate about  7% erroneous codewords
		MEDIUM_ECC   = 1,  // The QR Code can tolerate about 15% erroneous codewords
		QUARTILE_ECC = 2,  // The QR Code can tolerate about 25% erroneous codewords
		HIGH_ECC     = 3,  // The QR Code can tolerate about 30% erroneous codewords
	};
	
	
	
	/*---- Static factory functions (high level) ----*/
	
	/* 
	 * Returns a QR Code representing the given Unicode text string at the given error correction level.
	 * As a conservative upper bound, this function is guaranteed to succeed for strings that have 738 or fewer
	 * Unicode code points (not UTF-8 code units) if the LOW error correction level is used. The smallest possible
	 * QR Code version is automatically chosen for the output. The ECC level of the result may be higher than the
	 * ecl argument if it can be done without increasing the version.
	 */
	public: static QrCode encodeText(const char *text, Ecc ecl);
	
	
	/* 
	 * Returns a QR Code representing the given binary data at the given error correction level.
	 * This function always encodes using the binary segment mode, not any text mode. The maximum number of
	 * bytes allowed is 2953. The smallest possible QR Code version is automatically chosen for the output.
	 * The ECC level of the result may be higher than the ecl argument if it can be done without increasing the version.
	 */
	public: static QrCode encodeBinary(const std::vector<std::uint8_t> &data, Ecc ecl);
	
	
	/*---- Static factory functions (mid level) ----*/
	
	/* 
	 * Returns a QR Code representing the given segments with the given encoding parameters.
	 * The smallest possible QR Code version within the given range is automatically
	 * chosen for the output. Iff boostEcl is true, then the ECC level of the result
	 * may be higher than the ecl argument if it can be done without increasing the
	 * version. The mask number is either between 0 to 7 (inclusive) to force that
	 * mask, or -1 to automatically choose an appropriate mask (which may be slow).
	 * This function allows the user to create a custom sequence of segments that switches
	 * between modes (such as alphanumeric and byte) to encode text in less space.
	 * This is a mid-level API; the high-level API is encodeText() and encodeBinary().
	 */
	public: static QrCode encodeSegments(const std::vector<QrSegment> &segs, Ecc ecl,
		int minVersion=1, int maxVersion=40, int mask=-1, bool boostEcl=true);
	
	
	/*---- Constants ----*/
	
	public: static constexpr int MIN_VERSION =  1;  // The minimum version number supported in the QR Code Model 2 standard
	public: static constexpr int MAX_VERSION = 40;  // The maximum version number supported in the QR Code Model 2 standard
	
	
	/*---- Instance fields ----*/
	
	// The version number of this QR Code, which is between 1 and 40 (inclusive).
	// This determines the size of this barcode.
	private: int version;
	
	// The width and height of this QR Code, measured in modules, between
	// 21 and 177 (inclusive). This is equal to version * 4 + 17.
	private: int size;
	
	// The error correction level used in this QR Code.
	private: Ecc errorCorrectionLevel;
	
	// The index of the mask pattern used in this QR Code, which is between 0 and 7 (inclusive).
	// Even if a QR Code is created with automatic masking requested (mask = -1),
	// the resulting object still has a mask value between 0 and 7.
	private: int mask;
	
	// The modules of this QR Code (false = light, true = dark).
	// Immutable after constructor finishes. Accessed through getModule().
	private: std::vector<std::vector<bool> > modules;
	
	// Indicates function modules that are not subjected to masking. Discarded when constructor finishes.
	private: std::vector<std::vector<bool> > isFunction;
	
	
	
	/*---- Constructor (low level) ----*/
	
	/* 
	 * Creates a new QR Code with the given version number,
	 * error correction level, data codeword bytes, and mask number.
	 * This is a low-level API that most users should not use directly.
	 * A mid-level API is the encodeSegments() function.
	 */
	public: QrCode(int ver, Ecc ecl, const std::vector<std::uint8_t> &dataCodewords, int msk);
	
	
	
	/*---- Public instance methods ----*/
	
	/* 
	 * Returns this QR Code's version, in the range [1, 40].
	 */
	public: int getVersion() const;
	
	
	/* 
	 * Returns this QR Code's size, in the range [21, 177].
	 */
	public: int getSize() const;
	
	
	/* 
	 * Returns this QR Code's error correction level.
	 */
	public: Ecc getErrorCorrectionLevel() const;
	
	
	/* 
	 * Returns this QR Code's mask, in the range [0, 7].
	 */
	public: int getMask() const;
	
	
	/* 
	 * Returns the color of the module (pixel) at the given coordinates, which is false
	 * for light or true for dark. The top left corner has the coordinates (x=0, y=0).
	 * If the given coordinates are out of bounds, then false (light) is returned.
	 */
	public: bool getModule(int x, int y) const;
	
	
	
	/*---- Private helper methods for constructor: Drawing function modules ----*/
	
	// Reads this object's version field, and draws and marks all function modules.
	private: void drawFunctionPatterns();
	
	
	// Draws two copies of the format bits (with its own error correction code)
	// based on the given mask and this object's error correction level field.
	private: void drawFormatBits(int msk);
	
	
	// Draws two copies of the version bits (with its own error correction code),
	// based on this object's version field, iff 7 <= version <= 40.
	private: void drawVersion();
	
	
	// Draws a 9*9 finder pattern including the border separator,
	// with the center module at (x, y). Modules can be out of bounds.
	private: void drawFinderPattern(int x, int y);
	
	
	// Draws a 5*5 alignment pattern, with the center module
	// at (x, y). All modules must be in bounds.
	private: void drawAlignmentPattern(int x, int y);
	
	
	// Sets the color of a module and marks it as a function module.
	// Only used by the constructor. Coordinates must be in bounds.
	private: void setFunctionModule(int x, int y, bool isDark);
	
	
	// Returns the color of the module at the given coordinates, which must be in range.
	private: bool module(int x, int y) const;
	
	
	/*---- Private helper methods for constructor: Codewords and masking ----*/
	
	// Returns a new byte string representing the given data with the appropriate error correction
	// codewords appended to it, based on this object's version and error correction level.
	private: std::vector<std::uint8_t> addEccAndInterleave(const std::vector<std::uint8_t> &data) const;
	
	
	// Draws the given sequence of 8-bit codewords (data and error correction) onto the entire
	// data area of this QR Code. Function modules need to be marked off before this is called.
	private: void drawCodewords(const std::vector<std::uint8_t> &data);
	
	
	// XORs the codeword modules in this QR Code with the given mask pattern.
	// The function modules must be marked and the codeword bits must be drawn
	// before masking. Due to the arithmetic of XOR, calling applyMask() with
	// the same mask value a second time will undo the mask. A final well-formed
	// QR Code needs exactly one (not zero, two, etc.) mask applied.
	private: void applyMask(int msk);
	
	
	// Calculates and returns the penalty score based on state of this QR Code's current modules.
	// This is used by the automatic mask choice algorithm to find the mask pattern that yields the lowest score.
	private: long getPenaltyScore() const;
	
	
	
	/*---- Private helper functions ----*/
	
	// Returns an ascending list of positions of alignment patterns for this version number.
	// Each position is in the range [0,177), and are used on both the x and y axes.
	// This could be implemented as lookup table of 40 variable-length lists, instead of as
	// computed code.
	private: std::vector<int> getAlignmentPatternPositions() const;
	
	
	// Returns the number of data bits that can be stored in a QR Code of the given version number, after
	// all function modules are excluded. This includes remainder bits, so it might not be a multiple of 8.
	// The result is in the range [208, 29648]. This could be implemented as a 40-entry lookup table.
	private: static int getNumRawDataModules(int ver);
	
	
	// Returns the number of 8-bit data (i.e. not error correction) codewords contained in any
	// QR Code of the given version number and error correction level, with remainder bits discarded.
	// This stateless pure function could be implemented as a (40*4) lookup table.
	private: static int getNumDataCodewords(int ver, Ecc ecl);
	
	
	// Returns a Reed-Solomon ECC generator polynomial for the given degree. This could be
	// implemented as a lookup table over all possible parameter values, instead of as an algorithm.
	private: static std::vector<std::uint8_t> reedSolomonComputeDivisor(int degree);
	
	
	// Returns the Reed-Solomon error correction codeword for the given data and divisor polynomials.
	private: static std::vector<std::uint8_t> reedSolomonComputeRemainder(const std::vector<std::uint8_t> &data, const std::vector<std::uint8_t> &divisor);
	
	
	// Returns the product of the two given field elements modulo GF(2^8/0x11D). The arguments and result
	// are unsigned 8-bit integers. This could be implemented as a lookup table of 256*256 entries of uint8.
	private: static std::uint8_t reedSolomonMultiply(std::uint8_t x, std::uint8_t y);
	
	
	// Can only be called immediately after a light run is added, and
	// returns either 0, 1, or 2. A helper function for getPenaltyScore().
	private: int finderPenaltyCountPatterns(const std::array<int,7> &runHistory) const;
	
	
	// Must be called at the end of a line (row or column) of modules. A helper function for getPenaltyScore().
	private: int finderPenaltyTerminateAndCount(bool currentRunColor, int currentRunLength, std::array<int,7> &runHistory) const;
	
	
	// Pushes the given value to the front and drops the last value. A helper function for getPenaltyScore().
	private: void finderPenaltyAddHistory(int currentRunLength, std::array<int,7> &runHistory) const;
	
	
	// Returns true iff the i'th bit of x is set to 1.
	private: static bool getBit(long x, int i);
	
	
	/*---- Constants and tables ----*/
	
	private: static const int PENALTY_N1;
	private: static const int PENALTY_N2;
	private: static const int PENALTY_N3;
	private: static const int PENALTY_N4;
	
	private: static const std::int8_t ECC_CODEWORDS_PER_BLOCK[4][41];
	private: static const std::int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41];
	
	
	/*---- Private helper enumeration ----*/
	
	private: static int getFormatBits(Ecc ecl);
	
};

}
