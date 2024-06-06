#include <alib/_arapidjson.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/allocators.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace rapidjson;

struct alib::ng::Document::Impl{
    rapidjson::Document doc;
};

extern std::string GVToString(Value & v);

std::string alib::ng::Document::getValue(std::string name){
    if(!data->doc.HasMember(name.c_str()))return name;
    return GVToString(data->doc[name.c_str()]);
}

std::string alib::ng::Document::getValueI(std::vector<std::string> names){
    rapidjson::Document rdoc;
    rdoc.CopyFrom(data->doc,rdoc.GetAllocator());
    rapidjson::Value o = data->doc.GetObject();
    for(std::string & name : names){
        if(!o.HasMember(name.c_str()))return name;
        o = o[name.c_str()];
    }
    data->doc.CopyFrom(rdoc,data->doc.GetAllocator());
    return GVToString(o);
}

bool alib::ng::Document::parse(std::string da){
    data->doc.Parse(da.c_str());
    return !data->doc.HasParseError();
}

alib::ng::Document::Document(){
    this->data = new Impl();
    data->doc.SetObject();
}

void alib::ng::Document::foreach(void (*fun)(std::string,std::string,void*),void* reserved){
    for(auto i = data->doc.MemberBegin();i < data->doc.MemberEnd();++i){
        fun(GVToString((*i).name),GVToString((*i).value),reserved);
    }
}

alib::ng::Document::~Document(){
    delete data;
}
