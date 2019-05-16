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
    cmd_response,
    unknow,
};


using antlv_type = unsigned short;
using antlv_length = size_t;
using antlv_value = char *;


//编译期能得到的长度
constexpr std::size_t head_size() noexcept{
    return (sizeof(antlv_type)+sizeof(antlv_length));
}

static QByteArray make_heartbeat_package(){
    static char data[head_size()]={0x00};

    antlv_type type = static_cast<unsigned short>(package_type::heart_beat);
    memcpy(data, &type, sizeof(type));
    antlv_length length = 0;
    memcpy(data+sizeof(type), &length, sizeof(length));


    return QByteArray::fromRawData(data, head_size());

}

static QByteArray make_cmd_package(const char *v, size_t l){

    QByteArray cmd(head_size()+l, 0x00);

    antlv_type type = static_cast<unsigned short>(package_type::cmd_requst);
    memcpy(cmd.data(), &type, sizeof(type));
    antlv_length length = l;
    memcpy(cmd.data()+sizeof(type), &length, sizeof(length));

    memcpy(cmd.data()+sizeof(type)+sizeof(length), v, l);

    return cmd;
}

/*
static QByteArray make_response_package(const char *v, size_t l){
    size_t len = sizeof(antlv_type)+sizeof(antlv_length);
    char * data = new char[len];

    antlv_type type = static_cast<unsigned short>(package_type::cmd_response);
    memcpy(data, &type, sizeof(type));
    antlv_length length = l;
    memcpy(data+sizeof(type), &length, sizeof(length));

    memcpy(data+sizeof(type)+sizeof(length), v, l);

    return QByteArray::fromRawData(data, len);
}
*/

static unsigned short parse_package(QByteArray& package, QByteArray& data){
    antlv_type type = static_cast<unsigned short>(package_type::unknow);
    antlv_length length = 0;

    if(package.size()>=sizeof(type)){
        memcpy(&type, package.data(), sizeof(type));

        if (static_cast<unsigned short>(package_type::heart_beat)==type){
            package.remove(0, sizeof(type)+sizeof(length));
            data.append(antlv::make_heartbeat_package());
        }

        if (static_cast<unsigned short>(package_type::cmd_requst) ==type){
            if(package.size()>=(sizeof(type)+sizeof(length)) ){
                memcpy(&length, package.data()+sizeof(type), sizeof(length));

                if(package.size()>=(sizeof(type)+sizeof(length)+length)){
                    data.append(package.data()+sizeof(type)+sizeof(length), length);

                    package.remove(0, (sizeof(type)+sizeof(length)+length));
                }
            }
            else{
                type = static_cast<unsigned short>(package_type::unknow);
            }
        }
    }

    return type;
}

}

#endif // ANTLV_H
