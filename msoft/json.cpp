#include "msoft/json.h"

JSONFormatter::JSONFormatter(){

    message = "{}";
    cursorPosition = 1;

    FIRST_ENTRY = true;
    FIRST_ARRAY = true;
    FIRST_MULTIPLE = true;
}

QString JSONFormatter::getMessage(){
    return message;
}

void JSONFormatter::insertLabel(QString label){

    if (FIRST_ENTRY) {
        FIRST_ENTRY = false;
    }else{
        message.insert(cursorPosition, ",");
        cursorPosition++;
    }

    message.insert(cursorPosition, "\"");
    cursorPosition++;

    message.insert(cursorPosition, label);
    cursorPosition += label.length();

    message.insert(cursorPosition, "\"");
    cursorPosition++;

    message.insert(cursorPosition, ": ");
    cursorPosition += 2;
}

void JSONFormatter::insertValue(QString value){

    message.insert(cursorPosition, "\"");
    cursorPosition++;

    message.insert(cursorPosition, value);
    cursorPosition += value.length();

    message.insert(cursorPosition, "\"");
    cursorPosition++;
}

void JSONFormatter::openArray(){

    FIRST_ENTRY = true;

    if (FIRST_ARRAY) {
        FIRST_ARRAY = false;
    }else{
        message.insert(cursorPosition, ",");
        cursorPosition++;
    }

    message.insert(cursorPosition, "[]");
    cursorPosition++;

    FIRST_ENTRY = true;
}

void JSONFormatter::closeArray(){
    cursorPosition++;

    FIRST_ARRAY = true;
    FIRST_MULTIPLE = true;
}

void JSONFormatter::openMultipleValue(){

    FIRST_ENTRY = true;

    if (FIRST_MULTIPLE) {
        FIRST_MULTIPLE = false;
    }else{
        message.insert(cursorPosition, ",");
        cursorPosition++;
    }

    message.insert(cursorPosition, "{}");
    cursorPosition++;
}

void JSONFormatter::closeMultipleValue(){
    cursorPosition++;

    FIRST_ENTRY = false;
}
