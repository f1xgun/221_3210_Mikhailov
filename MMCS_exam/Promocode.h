#ifndef PROMOCODE_H
#define PROMOCODE_H

#include <QString>

class Promocode {
public:
    QString code;

    Promocode(const QString &code) : code(code) {}
};


#endif // PROMOCODE_H
