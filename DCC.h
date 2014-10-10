#pragma once
#include "DCCMerge.h"

#define DCC_MAX_DIR		32
#define DCC_MAX_FRAME	256

class DCC {
	struct DCCHeader {
		unsigned char bySignature;
		unsigned char byVersion;
		unsigned char byNumDirections;
		long lFramesPerDirection;
		long lTag;
		long lFinalDC6Size;
		long lDirOffset[DCC_MAX_DIR];
		bool bAlreadyDecoded;
	};

	struct DCCBitstream {
		unsigned char* ptData;
		unsigned long ulSize;
		unsigned long ulByteCursor;
		unsigned char byCurrentBit;
		unsigned long ulCurrentBitNum;
	};

	struct DCCFramebufferCell {
		int	x0, y0, w, h;
		int lastW, lastH;
		int lastx0, lasty0;
	};

	struct DCCFrameCell {
		unsigned long ulUnknown0;
		unsigned long ulWidth;
		unsigned long ulHeight;
		long lXOffset;
		long lYOffset;
		unsigned long ulOptionalBytes;
		unsigned long ulCodedBytes;
		unsigned char *ptOptionalData;
		unsigned long ulFlipped;

		DCCFramebufferCell* ptFramebuffers;
		unsigned long ulNumCellsW;
		unsigned long ulNumCellsH;
	};

	struct DCCPixelBuffer {
		unsigned char byValue[4];
		unsigned long ulFrame;
		unsigned long ulFrameCellIndex;
	};

	struct DCCDirection {
		unsigned long ulOutputSize;
		unsigned long ulCompressionFlags;
		unsigned long ulUnknown0;
		unsigned long ulWidthBits;
		unsigned long ulHeightBits;
		unsigned long ulXOffsetBits;
		unsigned long ulYOffsetBits;
		unsigned long ulOptionalBytesBits;
		unsigned long ulCodedBytesBits;

		// Bitstreams
		unsigned long	ulEqualCellBitstream;
		unsigned long	ulPixelMaskBitstream;
		unsigned long	ulEncodingTypeBitstream;
		unsigned long	ulRawPixelBitstream;
		unsigned long	ulPixelCodeDisplacementBitstream;
		DCCBitstream	dccEqualCellBitstream;
		DCCBitstream	dccPixelMaskBitstream;
		DCCBitstream	dccEncodingTypeBitstream;
		DCCBitstream	dccRawPixelBitstream;
		DCCBitstream	dccPixelCodeDisplacementBitstream;

		// Frame buffer
		unsigned long	ulNumCellsW;
		unsigned long	ulNumCellsH;
		DCCFrameCell	*ptFrameCells;

		// Pixel Buffer
		DCCPixelBuffer	*ptPixelBuffer;
		unsigned long	ulNumPixelBufferBlocks;

		unsigned char	byPixelValues[256];
	};

	unsigned char*	ptDCCData;
	unsigned long	ulDCCSize;
	DCCHeader		dccHeader;
	DCCDirection	dccDirection[DCC_MAX_DIR];
	DCCFrameCell	dccFrames[DCC_MAX_DIR][DCC_MAX_FRAME];

	static int iDCCBitWidth[16];

	void Decode(long lBitfield);
	void ReadHeader();
	void ReadDirection(unsigned int iDirection);
	void ReadFrameHeader(DCCBitstream* ptBitstream, const int iDirection, const int iFrame);
	void ReadOptionalData(DCCBitstream* ptBitstream, const int iDirection);
	void ReadOtherBitstreams(DCCBitstream* ptBitstream, const int iDirection);
	void ReadPixelValues(DCCBitstream* ptBitstream, const int iDirection);
	void InitSingleDirectionalBitstream(DCCBitstream* ptBitstream, DCCBitstream* ptThis, unsigned long* ptCursor, unsigned long* ptSize);
	void InitDirectionalBitstreams(DCCBitstream* ptBitstream, const int iDirection);
	void FillPixelBuffer(const int iDirection);
	// void BuildFrames
	void ReadBytes(DCCBitstream *ptBitstream, const int iNumBytes, void* ptDestination);
	void ReadBits(DCCBitstream *ptBitstream, const int iNumBits, bool bSigned, unsigned long* ptDestination);
public:
	DCC(const char* sFileName);
	~DCC();
	void PrintInformation();
};