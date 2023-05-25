#include <QtCore/QDir>
#include <QtGui/QImage>
#include <QThread>

#include "imagefilesmodel.h"
#include "barchlib.h"


ImageFilesModel::ImageFilesModel(const QString &dirPath, QObject *parent)
    : QAbstractTableModel(parent)
    , m_directoryPath(dirPath)      // Current working directory by default
{
//    RefreshModel();
}

int ImageFilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_fileItems.count();
}

QVariant ImageFilesModel::data(const QModelIndex &index, int role) const
{
    QVariant result = QString();

    if (index.isValid() && index.row() < m_fileItems.count())
    {
        const FileItem fileItem = m_fileItems[index.row()];

        if (role == Qt::DisplayRole)
        {
            switch (static_cast<Column>(index.column()))
            {
            case Column::Name:
                result = fileItem.info.fileName();
                break;
            case Column::Type:
                if (fileItem.type == Type::BMP)
                {
                    result = "BMP";
                }
                else if (fileItem.type == Type::PNG)
                {
                    result = "PNG";
                }
                else if (fileItem.type == Type::Packed)
                {
                    result = "Packed";
                }
                break;
            case Column::Size:
                result = fileItem.info.size();
                break;
            case Column::Status:
                if (fileItem.status == Status::Packing)
                {
                    result = QString("Packing: %1%").arg(fileItem.progress);
                }
                else if (fileItem.status == Status::Unpacking)
                {
                    result = QString("Unpacking: %1%").arg(fileItem.progress);
                }
                break;
            }
        }
    }

    return result;
}

QVariant ImageFilesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    QVariant result = QString();

    if (role == Qt::DisplayRole && section >= 0 && section < columnCount())
    {
        static const QMap<Column, QString> headers {
            {Column::Name, "File Name"},
            {Column::Type, "Type"},
            {Column::Size, "Size"},
            {Column::Status, "Status"}
        };

        result = headers[static_cast<Column>(section)];
    }

    return result;
}

QHash<int, QByteArray> ImageFilesModel::roleNames() const
{
    return {{Qt::DisplayRole, "display"}};
}

void ImageFilesModel::setDirectoryPath(QString path)
{
    if (m_directoryPath != path)
    {
        m_directoryPath = path;
        RefreshModel();
        emit directoryPathChanged();
    }
}

void ImageFilesModel::processFile(int fileIndex)
{
    if (fileIndex >= 0 && fileIndex < m_fileItems.size())
    {
        FileItem& fileItem = m_fileItems[fileIndex];
        if (fileItem.status == Status::Normal)
        {
            if (fileItem.type == Type::BMP || fileItem.type == Type::PNG)
            {
                PackImage(fileItem);
            }
            else if (fileItem.type == Type::Packed)
            {
                UnpackImage(fileItem);
            }
        }
    }
}

void ImageFilesModel::PackImage(FileItem &sourceFile)
{
    QImage* sourceImage = new QImage(sourceFile.info.absoluteFilePath());
    static const QList<QImage::Format> supported { QImage::Format_Grayscale8,
                                                   QImage::Format_Indexed8 };

    // TODO: show warning for unsupported format
    if (sourceImage != nullptr &&
        !sourceImage->isNull() &&
        supported.contains(sourceImage->format()))
    {
        sourceFile.status = Status::Packing;
        sourceFile.progress = 0;

        RawImageData srcData;
        srcData.width = sourceImage->width();
        srcData.height = sourceImage->height();
        srcData.data = sourceImage->bits();

        ImagePacker packer(srcData);

        // TODO: create thread and call library function to pack image
        ImagePacker::Result result = packer.Pack();

        if (result == ImagePacker::Result::OK)
        {
            // TODO: save dstData to the file
            packer.SaveToFile();
        }

        delete sourceImage;
    }
}

void ImageFilesModel::UnpackImage(FileItem &sourceFile)
{
    // TODO: load and parse source file
    // TODO: create thread and call library function to unpack image
}

void ImageFilesModel::RefreshModel()
{
    beginResetModel();
    m_fileItems.clear();

    QDir directory(m_directoryPath);
    QStringList nameFilters;
    nameFilters << "*.bmp" << "*.png" << "*.barch";
    QFileInfoList fileInfoList = directory.entryInfoList(nameFilters, QDir::Files);

    for (const QFileInfo &fileInfo : fileInfoList)
    {
        FileItem fileItem(fileInfo);
        if (fileInfo.suffix().toLower() == "bmp")
        {
            fileItem.type = Type::BMP;
        }
        else if (fileInfo.suffix().toLower() == "png")
        {
            fileItem.type = Type::PNG;
        }
        else if (fileInfo.suffix().toLower() == "barch")
        {
            fileItem.type = Type::Packed;
        }
        m_fileItems.append(fileItem);
    }

    endResetModel();
}

