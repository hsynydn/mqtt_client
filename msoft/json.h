#ifndef JSON_H
#define JSON_H

#include <string>
#include <QString>

class JSONFormatter{

    public:

        JSONFormatter();

        void insertLabel(QString label);
        void insertValue(QString value);
        void openArray();
        void closeArray();
        void openMultipleValue();
        void closeMultipleValue();

        QString getMessage();

    private:

        QString message;
        int cursorPosition;

        bool FIRST_ENTRY;
        bool FIRST_ARRAY;
        bool FIRST_MULTIPLE;
};

#endif // JSON_H
