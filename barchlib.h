#ifndef BARCHLIB_H
#define BARCHLIB_H

#include <vector>
#include <string>
#include <fstream>


using Byte = unsigned char;

struct RawImageData
{
    int width = -1;         // image width in pixels
    int height = -1;        // image height in pixels
    Byte* data = nullptr;   // Pointer to image data.
                            // data[j * width + i] is color of pixel in row j and column i.
};

enum class Result
{
    OK,
    FileError,
    InvalidSource,
    EmptyPacker
};

class ImagePacker
{
public:
    Result pack(const RawImageData sourceData);
    Result saveToFile(const std::string& fileName) const;

private:
    void packLine(Byte* bits, int lineCount);
    int addBits(std::vector<Byte>& line, Byte* bits, int sizeInBits, int bitOffset);

    bool m_packed = false;
    int m_width = -1;
    int m_height = -1;
    Byte* m_bits = nullptr; // maybe to use RawImageData instead?
    std::vector<int> m_emptyLines;
    std::vector<Byte> m_packedData;
};

class ImageUnpacker
{
public:
    Result unpack(const std::string& fileName);
    std::vector<Byte>& bytes() { return m_data; }
    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    Result unpackLine(Byte*& lineBits, int index);
    Byte get8bits(Byte* bits, int bitOffset);

    int m_width = -1;
    int m_height = -1;
    int m_alignedWidth = -1;
    std::vector<int> m_emptyLines;
    std::vector<Byte> m_data;
};


#endif // BARCHLIB_H
