#ifndef IMAGEFILESMODEL_H
#define IMAGEFILESMODEL_H

#include <QAbstractTableModel>
#include <QtCore/QFileInfo>
#include <QtQml/QQmlEngine>
#include <QReadWriteLock>

class ImageFilesModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    ImageFilesModel(const QString& dirPath = QString(), QObject *parent = nullptr);

    int columnCount(const QModelIndex & = QModelIndex()) const override { return 4; }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    QString getDirectoryPath() const { return m_directoryPath; }
    void setDirectoryPath(QString path);

    Q_INVOKABLE void processFile(int fileIndex);

    Q_PROPERTY(QString directoryPath READ getDirectoryPath WRITE setDirectoryPath NOTIFY directoryPathChanged)

signals:
    void directoryPathChanged();
    void errorMessage(QString message);

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
        Packing,
        Unpacking,
    };

    enum class Type: int
    {
        Unknown,
        BMP,
        PNG,
        Packed
    };

    struct FileItem
    {
        FileItem(const QFileInfo& fileInfo);
        QFileInfo info;
        Type type = Type::Unknown;
        Status status = Status::Normal;
        QSharedPointer<QReadWriteLock> lock;
    };

    void PackImage(FileItem& sourceFile);
    void UnpackImage(FileItem& sourceFile);
    void RefreshModel();

    QString m_directoryPath;
    QVector<FileItem> m_fileItems;
};

#endif // IMAGEFILESMODEL_H
