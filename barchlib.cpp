#include "barchlib.h"


// Constants
// ----------------------------------------------------------------------------
char kBarchFileId[] = "BA";
int kByteSize = 8;


// ImagePacker methods
// ----------------------------------------------------------------------------
Result ImagePacker::pack(const RawImageData sourceData)
{
    m_bits = sourceData.data;
    m_width = sourceData.width;
    m_height = sourceData.height;

    Result result = Result::OK;
    Byte* current = m_bits;
    int alignedWidth = (m_width + 3) & ~3;  // Pixel lines are 32-bits-aligned

    for (int i = 0; i < m_height; ++i, current += alignedWidth)
    {
        packLine(current, i);
    }

    m_packed = true;
    return result;
}

Result ImagePacker::saveToFile(const std::string& fileName) const
{
    Result result = Result::OK;

    if (m_packed)
    {
        std::ofstream file(fileName, std::ios::binary);
        int emptyLinesCount = m_emptyLines.size();

        if (file.is_open())
        {
            if (!(file.write(kBarchFileId, 2) &&
                  file.write(reinterpret_cast<const char*>(&m_width), sizeof(m_width)) &&
                  file.write(reinterpret_cast<const char*>(&m_height), sizeof(m_height)) &&
                  file.write(reinterpret_cast<const char*>(&emptyLinesCount), sizeof(emptyLinesCount)) &&
                  file.write(reinterpret_cast<const char*>(m_emptyLines.data()), emptyLinesCount * sizeof(int)) &&
                  file.write(reinterpret_cast<const char*>(m_packedData.data()), m_packedData.size() * sizeof(Byte))))
            {
                result = Result::FileError;
            }

            file.close();
        }
        else
        {
            result = Result::FileError;
        }
    }
    else
    {
        result = Result::EmptyPacker;
    }

    return result;
}

void ImagePacker::packLine(Byte* bits, int lineCount)
{
    static Byte blackCode[] = { 0b10000000 };
    static Byte whiteCode[] = { 0b00000000 };
    static Byte otherCode[] = { 0b11000000 };

    std::vector<Byte> line;
    bool isEmpty = true;
    int bitOffset = 0;
    int linePreEnd = m_width - m_width % 4;

    for (int i = 0; i < linePreEnd; i += 4)
    {
        Byte& byte0 = bits[i];
        Byte& byte1 = bits[i + 1];
        Byte& byte2 = bits[i + 2];
        Byte& byte3 = bits[i + 3];

        if ((byte0 & byte1 & byte2 & byte3) == 0xff)
        {
            bitOffset = addBits(line, whiteCode, 2, bitOffset);
        }
        else if ((byte0 | byte1 | byte2 | byte3) == 0x00)
        {
            bitOffset = addBits(line, blackCode, 2, bitOffset);
            isEmpty = false;
        }
        else
        {
            bitOffset = addBits(line, otherCode, 2, bitOffset);
            bitOffset = addBits(line, bits + i, 32, bitOffset);
            isEmpty = false;
        }
    }

    // if m_width % 4 != 0, then write rest of bytes with no packing
    for (int i = linePreEnd; i < m_width; ++i)
    {
        if (bits[i] != 0xff)
        {
            isEmpty = false;
        }
        bitOffset = addBits(line, bits + i, 8, bitOffset);
    }

    if (isEmpty)
    {
        m_emptyLines.push_back(lineCount);
    }
    else
    {
        m_packedData.insert(m_packedData.end(), line.begin(), line.end());
    }
}

int ImagePacker::addBits(std::vector<Byte>& line, Byte *bits, int sizeInBits, int bitOffset)
{
    for (int index = 0; index < sizeInBits;)
    {
        if (bitOffset == 0)
        {
            line.push_back(*bits);
            index += kByteSize;
            ++bits;
        }
        else
        {
            Byte leftMask = ~(0xff >> bitOffset);
            Byte leftBits = ((*bits) >> bitOffset);
            line.back() = (line.back() & leftMask) | leftBits;
            int rightOffset = kByteSize - bitOffset;
            index += rightOffset;
            if (index < sizeInBits)
            {
                Byte rightBits = (*bits) << rightOffset;
                line.push_back(rightBits);
                index += bitOffset;
                ++bits;

                // TODO: cut unused bits in the end (?)
            }
        }
    }

    return (sizeInBits + bitOffset) % kByteSize;
}

// ImageUnpacker methods
// ----------------------------------------------------------------------------
Result ImageUnpacker::unpack(const std::string &fileName)
{
    Result result = Result::OK;
    std::ifstream file(fileName, std::ios::binary);

    if (file.is_open())
    {
        file.seekg(0, file.end);
        int fileSize = file.tellg();
        file.seekg(0, file.beg);
        m_emptyLines.clear();
        char fileId[2];
        file.read(fileId, sizeof(fileId));

        if (fileId[0] == kBarchFileId[0] && fileId[1] == kBarchFileId[1])
        {
            int emptyLinesCount = 0;

            file.read(reinterpret_cast<char*>(&m_width), sizeof(m_width));
            file.read(reinterpret_cast<char*>(&m_height), sizeof(m_height));
            file.read(reinterpret_cast<char*>(&emptyLinesCount), sizeof(emptyLinesCount));
            m_alignedWidth = (m_width + 3) & ~3;

            int* emptyLinesBuffer = new int[emptyLinesCount];
            file.read(reinterpret_cast<char*>(emptyLinesBuffer), emptyLinesCount * sizeof(int));
            m_emptyLines.insert(m_emptyLines.end(), emptyLinesBuffer, emptyLinesBuffer + emptyLinesCount);
            delete[] emptyLinesBuffer;

            int bitsSize = fileSize - static_cast<int>(file.tellg());
            Byte* packedBuffer = new Byte[bitsSize];
            file.read(reinterpret_cast<char*>(packedBuffer), bitsSize);
            Byte* currentLineBits = packedBuffer;

            for (int i = 0; i < m_height; ++i)
            {
                Result tempResult = unpackLine(currentLineBits, i);
                if (tempResult != Result::OK)
                {
                    result = tempResult;
                    break;
                }
            }

            delete[] packedBuffer;
        }
        else
        {
            result = Result::InvalidSource;
        }

        file.close();
    }
    else
    {
        result = Result::FileError;
    }

    return result;
}

Result ImageUnpacker::unpackLine(Byte*& lineBits, int index)
{
    Result result = Result::OK;

    if (std::find(m_emptyLines.begin(), m_emptyLines.end(), index) == m_emptyLines.end())
    {
        int byteCount = 0;
        int bitOffset = 0;

        while (byteCount < m_width / 4 * 4)
        {
            Byte code = ((*lineBits) << bitOffset) & 0b11000000;
            bitOffset += 2;

            if (bitOffset >= 8)
            {
                bitOffset -= 8;
                ++lineBits;
            }

            if (code == 0b00000000)
            {
                // insert 4 white pixels
                m_data.insert(m_data.end(), 4, 0xff);
            }
            else if (code == 0b10000000)
            {
                // insert 4 black pixels
                m_data.insert(m_data.end(), 4, 0x00);
            }
            else if (code == 0b11000000)
            {
                // insert next 32 bits
                Byte bytes[4] = {
                    get8bits(lineBits, bitOffset),
                    get8bits(lineBits + 1, bitOffset),
                    get8bits(lineBits + 2, bitOffset),
                    get8bits(lineBits + 3, bitOffset)
                };
                m_data.insert(m_data.end(), bytes, bytes + 4);
                lineBits += 4;
            }
            else
            {
                result = Result::InvalidSource;
                break;
            }

            byteCount += 4;
        }

        for (; byteCount < m_width; ++byteCount)
        {
            Byte byte = get8bits(lineBits, bitOffset);
            m_data.push_back(byte);
            ++lineBits;
        }

        if (bitOffset != 0)
        {
            ++lineBits;
        }

        if (m_width < m_alignedWidth)
        {
            m_data.insert(m_data.end(), m_alignedWidth - m_width, 0x00);
        }
    }
    else
    {
        m_data.insert(m_data.end(), m_width, 0xff);
        m_data.insert(m_data.end(), m_alignedWidth - m_width, 0x00);
    }

    return result;
}

Byte ImageUnpacker::get8bits(Byte *bits, int bitOffset)
{
    Byte byte;

    if (bitOffset == 0)
    {
        byte = *bits;
    }
    else
    {
        byte = (bits[0] << bitOffset) | (bits[1] >> (kByteSize - bitOffset));
    }

    return byte;
}
