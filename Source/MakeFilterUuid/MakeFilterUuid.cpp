

#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{

  QString libName = QString::fromLatin1(argv[1]);
  QString className = QString::fromLatin1(argv[2]);

  uint l = 100;
  ushort w1 = 200;
  ushort w2 = 300;
  uchar b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  int32_t i = 0;
  while(i < 8 && i < libName.size())
  {
    b[i] = static_cast<uint8_t>(libName.at(i).toLatin1());
    i++;
  }
  QUuid uuid = QUuid(l, w1, w2, b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
  QString nameSpace;
  QTextStream out(&nameSpace);
  out << className << " " << libName;
  QUuid p1 = QUuid::createUuidV5(uuid, nameSpace);

  qDebug() << p1.toString();
  return 0;
}
