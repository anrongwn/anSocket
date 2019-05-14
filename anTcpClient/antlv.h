#ifndef ANTLV_H
#define ANTLV_H

#include <QByteArray>

namespace antlv{
/*TLV:
 * T type:      unsigned short
 * L lenght:    unsigned int
 * V value:     char *
  */

enum class package_type : unsigned short{
    heart_beat = 0xff00,
    cmd_requst,
    cmd_response
};


using antlv_type = unsigned short;
using antlv_length = size_t;
using antlv_value = char *;

void free_raw_data(QByteArray& raw){
    delete [] raw.data();
}

QByteArray make_heartbeat_package(){
    size_t len = sizeof(antlv_type)+sizeof(antlv_length);
    char * data = new char[len];

    antlv_type type = static_cast<unsigned short>(package_type::heart_beat);
    memcpy(data, &type, sizeof(type));
    antlv_length length = 0;
    memcpy(data+sizeof(type), &length, sizeof(length));


    return QByteArray::fromRawData(data, len);

}

QByteArray make_cmd_package(const char *v, size_t l){
    size_t len = sizeof(antlv_type)+sizeof(antlv_length)+l;
    char * data = new char[len];

    antlv_type type = static_cast<unsigned short>(package_type::cmd_requst);
    memcpy(data, &type, sizeof(type));
    antlv_length length = 0;
    memcpy(data+sizeof(type), &length, sizeof(length));

    memcpy(data+sizeof(type)+sizeof(length), v, l);


    return QByteArray::fromRawData(data, len);;
}

QByteArray make_response_package(const char *v, size_t l){
    size_t len = sizeof(antlv_type)+sizeof(antlv_length);
    char * data = new char[len];

    antlv_type type = static_cast<unsigned short>(package_type::cmd_response);
    memcpy(data, &type, sizeof(type));
    antlv_length length = 0;
    memcpy(data+sizeof(type), &length, sizeof(length));

    memcpy(data+sizeof(type)+sizeof(length), v, l);

    return QByteArray::fromRawData(data, len);
}


}

#endif // ANTLV_H
