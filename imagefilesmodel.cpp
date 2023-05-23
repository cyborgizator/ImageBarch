#include <QtCore/QDir>

#include "imagefilesmodel.h"


ImageFilesModel::ImageFilesModel(const QString &dirPath, QObject *parent)
    : QAbstractTableModel(parent)
    , m_directoryPath(dirPath)      // Current working directory by default
{
    RefreshModel();
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
                result = fileItem.info.suffix();
                break;
            case Column::Size:
                result = fileItem.info.size();
                break;
            case Column::Status:
                if (fileItem.status == Status::Encoding)
                {
                    result = QString("Encoding: %1%").arg(fileItem.progress);
                }
                else if (fileItem.status == Status::Decoding)
                {
                    result = QString("Decoding: %1%").arg(fileItem.progress);
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

void ImageFilesModel::RefreshModel()
{
    beginResetModel();
    m_fileItems.clear();

    QDir directory(m_directoryPath);
    QStringList nameFilters;
    nameFilters << "*.*"; // << "*.bmp" << "*.barch";
    QFileInfoList fileInfoList = directory.entryInfoList(nameFilters, QDir::Files);

    for (const QFileInfo &fileInfo : fileInfoList)
    {
        m_fileItems.append(FileItem(fileInfo));
    }

    endResetModel();
}

