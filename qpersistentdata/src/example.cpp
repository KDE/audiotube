#include "qpersistentdata.h"

#include <QDebug>
#include <QDateTime>

int main() {
    auto data = QPersistentData("/tmp");
    Q_ASSERT(data.isValid());

    data.insert(1, QSize(1,11));
    Q_ASSERT(data.value<QSize>(1) == QSize(1,11));
    data.insert("idList", QVector<int> {1, 2, 3, 4, 5});
    data.insert(u"moinLeude", u"Moin Leude");
    data.insert(QVector<QByteArray>({"moin", "leude"}), "Yo");
    data.insert(QDateTime(), u"Hello World"_qs);
    qDebug() << data.value<QVector<int>>("idList");
}
