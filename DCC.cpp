#include "DCC.h"

using namespace std;
int DCC::iDCCBitWidth[] = {
	0, 1, 2, 4, 6, 8, 10, 12, 14, 16, 20, 24, 26, 28, 30, 32
};

DCC::DCC(const char* sFileName) {
	// Open the file
	FILE* ptFile = fopen(sFileName, "rb+");
	if (!ptFile) {
		cerr << "could not open " << sFileName << endl;
		return;
	}

	fseek(ptFile, 0L, SEEK_END);
	size_t szFilesize = ftell(ptFile);
	fseek(ptFile, 0L, SEEK_SET);

	ptDCCData = (unsigned char*)malloc(szFilesize);
	if (ptDCCData == nullptr) {
		cerr << "could not allocate " << szFilesize << " bytes for DCC read operation on "
			<< sFileName << endl;
		return;
	}

	size_t szRead = fread(ptDCCData, 1, szFilesize, ptFile);
	if (szRead == 0) {
		cerr << "could not read DCC file " << sFileName << endl;
		return;
	}
	fclose(ptFile);
	ulDCCSize = szFilesize;

	// Transfer 
}

DCC::~DCC() {
	if (ptDCCData != nullptr) {
		free(ptDCCData);
		ptDCCData = nullptr;
	}
	for (int i = 0; i < dccHeader.byNumDirections; i++) {
		for (int j = 0; j < dccHeader.lFramesPerDirection; j++) {
			if (dccFrames[i][j].ptOptionalData != nullptr) {
				free(dccFrames[i][j].ptOptionalData);
				dccFrames[i][j].ptOptionalData = nullptr;
			}
			if (dccFrames[i][j].ptFramebuffers != nullptr) {
				//int numCells = dccFrames[i][j].ulNumCellsW * dccFrames[i][j].ulNumCellsH;
				//for (int k = 0; k < )
				free(dccFrames[i][j].ptFramebuffers);
				dccFrames[i][j].ptFramebuffers = nullptr;
			}
		}
		if (dccDirection[i].ptPixelBuffer != nullptr) {
			free(dccDirection[i].ptPixelBuffer);
			dccDirection[i].ptPixelBuffer = nullptr;
		}
		if (dccDirection[i].ptFrameCells != nullptr) {
			free(dccDirection[i].ptFrameCells);
			dccDirection[i].ptFrameCells = nullptr;
		}
	}
}

void DCC::PrintInformation() {
	cout << "file_signature = " << dccHeader.bySignature << endl;
	cout << "version\t\t= " << dccHeader.byVersion << endl;
	cout << "directions\t\t= " << dccHeader.byNumDirections << endl;
	cout << "frames_per_dir = " << dccHeader.lFramesPerDirection << endl;
	cout << "tag\t\t\t= " << dccHeader.lTag << endl;
	cout << "final_dc6_size = " << dccHeader.lFinalDC6Size;

	for (int i = 0; i < dccHeader.byNumDirections; i++) {
		cout << endl << "direction " << i << endl;
		cout << '\t' << "outsize_coded\t\t= " << dccDirection[i].ulOutputSize << " bytes" << endl;
		cout << '\t' << "compression_flag\t= " << dccDirection[i].ulCompressionFlags << endl;
		cout << '\t' << "variable0_bits\t\t= " << dccDirection[i].ulUnknown0 << endl;
		cout << '\t' << "width_bits\t\t\t= " << dccDirection[i].ulWidthBits << endl;
		cout << '\t' << "height_bits\t\t= " << dccDirection[i].ulHeightBits << endl;
		cout << '\t' << "xoffset_bits\t\t= " << dccDirection[i].ulXOffsetBits << endl;
		cout << '\t' << "yoffset_bits\t\t= " << dccDirection[i].ulYOffsetBits << endl;
		cout << '\t' << "optional_bytes_bits = " << dccDirection[i].ulOptionalBytesBits << endl;
		cout << '\t' << "coded_bytes_bits\t= " << dccDirection[i].ulCodedBytesBits << endl;
		cout << '\t' << "cells\t\t\t\t= " << dccDirection[i].ulNumCellsW << " * " << dccDirection[i].ulNumCellsH << endl;
		cout << '\t' << "num_cells\t\t\t= " << dccDirection[i].ulNumCellsW * dccDirection[i].ulNumCellsH << endl;
		cout << '\t' << "equal_cell_bitstream_size\t\t= " << dccDirection[i].dccEqualCellBitstream.ulSize << endl;
		cout << '\t' << "pixel_mask_bitstream_size\t\t= " << dccDirection[i].dccPixelMaskBitstream.ulSize << endl;
		cout << '\t' << "encoding_type_bitstream_size\t= " << dccDirection[i].dccEncodingTypeBitstream.ulSize << endl;
		cout << '\t' << "raw_pixel_bitstream_size\t\t= " << dccDirection[i].dccRawPixelBitstream.ulSize << endl;
		cout << '\t' << "pixel_code_and_displacment_size = " << dccDirection[i].dccPixelCodeDisplacementBitstream.ulSize << endl;

		cout << endl << "\tpixel_values :" << endl;
		for (int j = 0; j < 256; j++) {
			cout << "\t" << dccDirection[i].byPixelValues[j];
			if (!((i + 1) % 16)) {
				cout << endl;
			}
		}
		for (int j = 0; j < dccHeader.lFramesPerDirection; j++) {
			cout << endl << "\tframe " << j << endl;
			if (dccDirection[j].ulUnknown0) {
				cout << "\t\tvariable0\t\t= " << dccFrames[i][j].ulUnknown0 << endl;
			}
			if (dccDirection[j].ulWidthBits) {
				cout << "\t\twidth\t\t\t= " << dccFrames[i][j].ulWidth << endl;
			}
			if (dccDirection[i].ulHeightBits) {
				cout << "\t\theight\t\t= " << dccFrames[i][j].ulHeight << endl;
			}
			if (dccDirection[i].ulXOffsetBits) {
				cout << "\t\txoffset\t\t= " << dccFrames[i][j].lXOffset << endl;
			}
			if (dccDirection[i].ulYOffsetBits) {
				cout << "\t\tyoffset\t\t= " << dccFrames[i][j].lYOffset << endl;
			}
			if (dccDirection[i].ulOptionalBytesBits) {
				cout << "\t\toptional_bytes = " << dccFrames[i][j].ulOptionalBytes << endl;
			}
			if (dccDirection[i].ulCodedBytesBits) {
				cout << "\t\tcoded_bytes\t= " << dccFrames[i][j].ulCodedBytes << endl;
			}

			cout << "\t\tbottom_up\t\t= " << dccFrames[i][j].ulFlipped;
			cout << "\t\tcells\t\t\t= " << dccFrames[i][j].ulNumCellsW << " * " << dccFrames[i][j].ulNumCellsH
				<< " = " << dccFrames[i][j].ulNumCellsW * dccFrames[i][j].ulNumCellsH << endl;

			cout << "\t\t1st cell0_x, y = " << dccFrames[i][j].ptFramebuffers[0].x0
				<< ", " << dccFrames[i][j].ptFramebuffers[0].y0 << endl;

			if (dccFrames[i][j].ulOptionalBytes) {
				cout << "\t\toptional datas =";
				for (int k = 0; k < (int)dccFrames[i][j].ulOptionalBytes; i++) {
					cout << hex << dccFrames[i][j].ptOptionalData[k];
				}
				cout << endl;
			}
		}
	}
}

void DCC::Decode(long lBitfield) {
	ReadHeader();
	for (int i = 0; i < dccHeader.byNumDirections; i++) {
		if (lBitfield & (1 << i)) {
			ReadDirection(i);
		}
	}
}

void DCC::ReadHeader() {
	DCCBitstream bs;
	if (dccHeader.bAlreadyDecoded) {
		return;
	}
	memset(&bs, 0, sizeof(bs));
	bs.ptData = ptDCCData;
	bs.ulSize = 8 * ulDCCSize;

	ReadBytes(&bs, 1, &dccHeader.bySignature);
	ReadBytes(&bs, 1, &dccHeader.byVersion);
	ReadBytes(&bs, 1, &dccHeader.byNumDirections);
	ReadBytes(&bs, 4, &dccHeader.lFramesPerDirection);
	ReadBytes(&bs, 4, &dccHeader.lTag);
	ReadBytes(&bs, 4, &dccHeader.lFinalDC6Size);
	for (int i = 0; i < dccHeader.byNumDirections; i++) {
		ReadBytes(&bs, 4, &dccHeader.lDirOffset[i]);
	}
	dccHeader.bAlreadyDecoded = true;
}

void DCC::ReadFrameHeader(DCCBitstream* ptBitstream, const int iDirection, const int iFrame) {
	DCCDirection* ptDirection = &dccDirection[iDirection];
	DCCFrameCell* ptFrame = &dccFrames[iDirection][iFrame];
	
	ReadBits(ptBitstream, iDCCBitWidth[ptDirection->ulUnknown0], false, &ptFrame->ulUnknown0);
	ReadBits(ptBitstream, iDCCBitWidth[ptDirection->ulWidthBits], false, &ptFrame->ulWidth);
	ReadBits(ptBitstream, iDCCBitWidth[ptDirection->ulHeightBits], false, &ptFrame->ulHeight);
	ReadBits(ptBitstream, iDCCBitWidth[ptDirection->ulXOffsetBits], true, (unsigned long*)&ptFrame->lXOffset);
	ReadBits(ptBitstream, iDCCBitWidth[ptDirection->ulYOffsetBits], true, (unsigned long*)&ptFrame->lYOffset);
	ReadBits(ptBitstream, iDCCBitWidth[ptDirection->ulOptionalBytesBits], false, &ptFrame->ulOptionalBytes);
	ReadBits(ptBitstream, iDCCBitWidth[ptDirection->ulCodedBytesBits], true, &ptFrame->ulCodedBytes);
	ReadBits(ptBitstream, iDCCBitWidth[1], false, &ptFrame->ulFlipped);
}

void DCC::ReadDirection(unsigned int iDirection) {
	DCCBitstream bs;
	memset(&bs, 0, sizeof(bs));
	bs.ptData = ptDCCData + dccHeader.lDirOffset[iDirection];
	if (iDirection == dccHeader.byNumDirections - 1) {
		bs.ulSize = 8 * (ulDCCSize - dccHeader.lDirOffset[iDirection]);
	} else {
		bs.ulSize = 8 * (dccHeader.lDirOffset[iDirection + 1] - dccHeader.lDirOffset[iDirection]);
	}
	ReadBits(&bs, 32, false, &dccDirection[iDirection].ulOutputSize);
	ReadBits(&bs, 2, false, &dccDirection[iDirection].ulCompressionFlags);
	ReadBits(&bs, 4, false, &dccDirection[iDirection].ulUnknown0);
	ReadBits(&bs, 4, false, &dccDirection[iDirection].ulWidthBits);
	ReadBits(&bs, 4, false, &dccDirection[iDirection].ulHeightBits);
	ReadBits(&bs, 4, false, &dccDirection[iDirection].ulXOffsetBits);
	ReadBits(&bs, 4, false, &dccDirection[iDirection].ulYOffsetBits);
	ReadBits(&bs, 4, false, &dccDirection[iDirection].ulOptionalBytesBits);
	ReadBits(&bs, 4, false, &dccDirection[iDirection].ulCodedBytesBits);

	// Read frame header
	for (int i = 0; i < dccHeader.lFramesPerDirection; i++) {
		ReadFrameHeader(&bs, iDirection, i);
	}

	// Read optional bytes
	ReadOptionalData(&bs, iDirection);

	// Get size of other bitstreams
	ReadOtherBitstreams(&bs, iDirection);

	// Read the pixels from this direction
	ReadPixelValues(&bs, iDirection);

	// Init the other bitstreams
	InitDirectionalBitstreams(&bs, iDirection);

	// Stage 1: Fill the pixel buffer
	FillPixelBuffer(iDirection);
	// Stage 2: Build the frames
}

void DCC::ReadOptionalData(DCCBitstream* ptBitstream, const int iDirection) {
	// Check for optional data
	size_t szOptionalDataSize = 0;
	for (int i = 0; i < dccHeader.lFramesPerDirection; i++) {
		szOptionalDataSize += dccFrames[iDirection][i].ulOptionalBytes;
	}
	if (szOptionalDataSize == 0) {
		return;
	}

	if (ptBitstream->byCurrentBit) {
		// Skip alignment bits if we're at a byte boundary
		ptBitstream->ulCurrentBitNum += 8 - ptBitstream->byCurrentBit;
		ptBitstream->byCurrentBit = 0;
		ptBitstream->ulByteCursor++;
	}

	// Read the data
	for (int i = 0; i < dccHeader.lFramesPerDirection; i++) {
		szOptionalDataSize = dccFrames[iDirection][i].ulOptionalBytes;
		if (szOptionalDataSize > 0) {
			dccFrames[iDirection][i].ptOptionalData = (unsigned char*)malloc(szOptionalDataSize);
			ReadBytes(ptBitstream, szOptionalDataSize, dccFrames[iDirection][i].ptOptionalData);
		}
	}
}

void DCC::ReadOtherBitstreams(DCCBitstream* ptBitstream, const int iDirection) {
	if (dccDirection[iDirection].ulCompressionFlags & 0x02) {
		// Equal Cell Bitstream
		ReadBits(ptBitstream, 20, false, &dccDirection[iDirection].ulEqualCellBitstream);
	}
	// Pixel Mask Bitstream
	ReadBits(ptBitstream, 20, false, &dccDirection[iDirection].ulPixelMaskBitstream);
	if (dccDirection[iDirection].ulCompressionFlags & 0x01) {
		// Encoding type bitstream
		ReadBits(ptBitstream, 20, false, &dccDirection[iDirection].ulEncodingTypeBitstream);
		// Raw Pixel Bitstream
		ReadBits(ptBitstream, 20, false, &dccDirection[iDirection].ulRawPixelBitstream);
	}

}

// TODO: OPTIMIZE ME PLS
void DCC::ReadPixelValues(DCCBitstream* ptBitstream, const int iDirection) {
	unsigned long ulTmp;
	int iPixelIndex = 0;
	for (int i = 0; i < 256; i++) {
		ReadBits(ptBitstream, 1, false, &ulTmp);
		if (ulTmp > 0) {
			dccDirection[iDirection].byPixelValues[iPixelIndex++] = (unsigned char)i;
		}
	}
}

void DCC::InitSingleDirectionalBitstream(DCCBitstream* ptBitstream, DCCBitstream* ptThis, unsigned long* ptCursor, unsigned long* ptSize) {
	if (ptBitstream == nullptr || ptThis == nullptr || ptCursor == nullptr || ptSize == nullptr) {
		return;
	}
	
	ptThis->ptData = ptBitstream->ptData + (*ptCursor / 8);
	ptThis->byCurrentBit = *ptCursor % 8;
	ptThis->ulCurrentBitNum = ptThis->ulByteCursor = 0;
	ptThis->ulSize = *ptSize;
	*ptCursor += *ptSize;
}

void DCC::InitDirectionalBitstreams(DCCBitstream* ptBitstream, const int iDirection) {
	DCCDirection* ptDirection = &dccDirection[iDirection];
	DCCBitstream* ptEqualCell = &ptDirection->dccEqualCellBitstream;
	DCCBitstream* ptPixelMask = &ptDirection->dccPixelMaskBitstream;
	DCCBitstream* ptEncoding = &ptDirection->dccEncodingTypeBitstream;
	DCCBitstream* ptRawPixel = &ptDirection->dccRawPixelBitstream;
	DCCBitstream* ptPixelCode = &ptDirection->dccPixelCodeDisplacementBitstream;

	unsigned long ulSelector = ptBitstream->ulByteCursor * 8 + ptBitstream->byCurrentBit;

	// Equal Cell Bitstream
	if (ptDirection->ulCompressionFlags & 0x02) {
		InitSingleDirectionalBitstream(ptBitstream, ptEqualCell, &ulSelector, &ptDirection->ulEqualCellBitstream);
	}

	// Pixel Mask Bitstream
	InitSingleDirectionalBitstream(ptBitstream, ptPixelMask, &ulSelector, &ptDirection->ulPixelMaskBitstream);

	if (ptDirection->ulCompressionFlags & 0x01) {
		// Encoding Type Bitstream
		InitSingleDirectionalBitstream(ptBitstream, ptEncoding, &ulSelector, &ptDirection->ulEncodingTypeBitstream);
		// Raw Pixel Bitstream
		InitSingleDirectionalBitstream(ptBitstream, ptRawPixel, &ulSelector, &ptDirection->ulRawPixelBitstream);
	}

	// Pixel Code and Displacement Bitstream
	InitSingleDirectionalBitstream(ptBitstream, ptPixelCode, &ulSelector, &ptDirection->ulPixelCodeDisplacementBitstream);
}

void DCC::FillPixelBuffer(const int iDirection) {
	DCCDirection* ptDirection = &dccDirection[iDirection];
	size_t szBufferWidth = ptDirection->ulNumCellsW;
	size_t szBufferHeight = ptDirection->ulNumCellsH;
	size_t szNumberCells = szBufferWidth * szBufferHeight;
	size_t szMemToAllocate = szNumberCells * sizeof(DCCPixelBuffer*);
	DCCPixelBuffer** ptptCellBuffer = (DCCPixelBuffer**)malloc(szMemToAllocate);

	if (ptptCellBuffer == nullptr) {
		cerr << "could not allocate " << szMemToAllocate << " bytes for direction " << iDirection << '\n';
		return;
	}
	memset(ptptCellBuffer, 0, szMemToAllocate);

	// Work on each frame for this direction
	for (int i = 0; i < dccHeader.lFramesPerDirection; i++) {
		DCCFrameCell* ptFrame = &dccFrames[iDirection][i];
		size_t szCellWidth = ptFrame->ulNumCellsW;
		size_t szCellHeight = ptFrame->ulNumCellsH;

		// Work on each cell (pixel??) of this frame
		for (int j = 0; j < szCellHeight; j++) {
			for (int k = 0; k < szCellWidth; k++) {
			}
		}
	}
}

void DCC::ReadBytes(DCCBitstream* ptBitstream, const int iNumBytes, void* ptDestination) {
	if (ptBitstream->ulCurrentBitNum) {
		cerr << "DCC::ReadBytes(): bytes must be aligned on a byte boundary" << endl;
		return;
	}
	size_t szCopySize = iNumBytes * 8;
	if (ptBitstream->ulCurrentBitNum + szCopySize >= ptBitstream->ulSize) {
		cerr << "DCC::ReadBytes: attempted to overread!" << endl;
		return;
	}
	memcpy(ptDestination, ptBitstream->ptData + ptBitstream->ulByteCursor, iNumBytes);
	ptBitstream->ulByteCursor += iNumBytes;
	ptBitstream->ulCurrentBitNum += szCopySize;
}

void DCC::ReadBits(DCCBitstream* ptBitstream, int iNumBits, bool bSigned, unsigned long* ptDestination) {
	if (iNumBits <= 0 || iNumBits > 32) {
		cerr << "DCC::ReadBits(): invalid iNumBits (" << iNumBits << ")" << endl;
		return;
	}
	unsigned int iDestinationByte = 0;
	unsigned int iDestinationBit = 0;
	for (int i = 0; i < iNumBits; i++) {
		if (ptBitstream->ulCurrentBitNum >= ptBitstream->ulSize) {
			return;
		}
		// Copy a bit.
		if (ptBitstream->ptData[ptBitstream->ulByteCursor] & (1 << ptBitstream->byCurrentBit)) {
			*(ptDestination + iDestinationByte) |= (1 << iDestinationBit);
		}

		// Prepare the next bit
		iDestinationBit++;
		if (iDestinationBit >= 8) {
			iDestinationBit = 0;
			iDestinationByte++;
		}
		ptBitstream->ulCurrentBitNum++;
		ptBitstream->byCurrentBit++;
		if (ptBitstream->byCurrentBit >= 8) {
			ptBitstream->byCurrentBit = 0;
			ptBitstream->ulByteCursor++;
		}
	}

	// If it's unsigned, then we need to do nothing further.
	if (!bSigned) {
		return;
	}

	if ((*ptDestination) & (1 << (iNumBits - 1))) {
		// It's negative, so extend the sign
		(*ptDestination) |= ~((1 << iNumBits) - 1);
	}
}