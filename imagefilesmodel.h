#ifndef IMAGEFILESMODEL_H
#define IMAGEFILESMODEL_H

#include <QAbstractTableModel>
#include <QtCore/QFileInfo>
#include <QtQml/QQmlEngine>

class ImageFilesModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    ImageFilesModel(const QString& dirPath = QString(), QObject *parent = nullptr);

    int columnCount(const QModelIndex & = QModelIndex()) const override { return 4; }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    enum class Column : int
    {
        Name,
        Type,
        Size,
        Status
    };

    enum class Status: int
    {
        Normal,
        Encoding,
        Decoding
    };

    struct FileItem
    {
        FileItem(const QFileInfo& fileInfo) : info(fileInfo) {}
        QFileInfo info;
        Status status = Status::Normal;
        int progress = 0;
    };

    void RefreshModel();

    QString m_directoryPath;
    QVector<FileItem> m_fileItems;
};

#endif // IMAGEFILESMODEL_H
